#include "PGProfiler.h"
#include "Platform/PGTime.h"

ProfileList PGProfileTimer::s_ProfileResults = ProfileList();

PGProfileTimer::PGProfileTimer(const char* name)
    : name(name), isStopped(false) {
    startNanoSeconds = PGTime::GetTimeNanoseconds();
}

PGProfileTimer::~PGProfileTimer() {
    if (!isStopped) {
        Stop();
    }
}

void PGProfileTimer::Stop() {
    uint64_t endNanoSeconds = PGTime::GetTimeNanoseconds();
    uint64_t deltaNanoSeconds = endNanoSeconds - startNanoSeconds;

    ProfileData profileData = { name, startNanoSeconds, deltaNanoSeconds };
    s_ProfileResults.push_back(profileData);

    isStopped = true;
}


std::unordered_map<std::string, size_t> PGGPUProfilerTimer::s_ProfileEntries;

bool PGGPUProfilerTimer::s_FirstPass = true;
size_t PGGPUProfilerTimer::s_CurrentFrameIndex = 0;
size_t PGGPUProfilerTimer::s_FreeQueryIndex = 1; // 0 is reserved for frame timer

HWQuery* PGGPUProfilerTimer::s_TimestampDisjointQueries[GPU_PROFILER_FRAME_LATENCY] = {0};

HWQuery* PGGPUProfilerTimer::s_StartQueries[GPU_PROFILER_MAX_QUERY_COUNT][GPU_PROFILER_FRAME_LATENCY] = {0};
HWQuery* PGGPUProfilerTimer::s_EndQueries[GPU_PROFILER_MAX_QUERY_COUNT][GPU_PROFILER_FRAME_LATENCY] = {0};

PGGPUProfilerTimer::PGGPUProfilerTimer(HWRendererAPI* rendererAPI, const char* name)
    : name(name), isStopped(false), rendererAPI(rendererAPI) {

    size_t queryIndex;
    auto searchEntry = s_ProfileEntries.find(name);
    if (searchEntry != s_ProfileEntries.end()) {
        queryIndex = searchEntry->second;
    } else {
        queryIndex = s_FreeQueryIndex++;
        s_ProfileEntries[name] = queryIndex;
    }

    rendererAPI->EndQuery(s_StartQueries[queryIndex][s_CurrentFrameIndex]);
}

PGGPUProfilerTimer::~PGGPUProfilerTimer() {
    if (!isStopped) {
        Stop(rendererAPI, name);
    }
}

void PGGPUProfilerTimer::Stop(HWRendererAPI* rendererAPI, const char* name) {
    auto searchEntry = s_ProfileEntries.find(name);
    if (searchEntry != s_ProfileEntries.end()) {
        size_t queryIndex = searchEntry->second;
        rendererAPI->EndQuery(s_EndQueries[queryIndex][s_CurrentFrameIndex]);
    }
    else {
        PG_ASSERT(false, "Can't find a GPU profile entry with this name");
    }

    isStopped = true;
}

void PGGPUProfilerTimer::Initialize(HWRendererAPI* rendererAPI) {
    for (size_t i = 0; i < GPU_PROFILER_FRAME_LATENCY; i++) {
        HWQueryDesc queryDesc;
        queryDesc.queryType = QUERY_TIMESTAMP_DISJOINT;
        queryDesc.flags = 0;

        s_TimestampDisjointQueries[i] = rendererAPI->CreateQuery(queryDesc);
    }

    for (size_t i = 0; i < GPU_PROFILER_MAX_QUERY_COUNT; i++) {
        for (size_t j = 0; j < GPU_PROFILER_FRAME_LATENCY; j++) {
            HWQueryDesc queryDesc;
            queryDesc.queryType = QUERY_TIMESTAMP;
            queryDesc.flags = 0;

            s_StartQueries[i][j] = rendererAPI->CreateQuery(queryDesc);
            s_EndQueries[i][j] = rendererAPI->CreateQuery(queryDesc);
        }
    }
}

void PGGPUProfilerTimer::Destroy() {
    for (size_t i = 0; i < GPU_PROFILER_FRAME_LATENCY; i++) {
        delete s_TimestampDisjointQueries[i];
    }

    for (size_t i = 0; i < GPU_PROFILER_MAX_QUERY_COUNT; i++) {
        for (size_t j = 0; j < GPU_PROFILER_FRAME_LATENCY; j++) {
            delete s_StartQueries[i][j];
            delete s_EndQueries[i][j];
        }
    }
}

void PGGPUProfilerTimer::BeginFrame(HWRendererAPI* rendererAPI) {
    rendererAPI->BeginQuery(s_TimestampDisjointQueries[s_CurrentFrameIndex]);
    rendererAPI->EndQuery(s_StartQueries[0][s_CurrentFrameIndex]);
}

void PGGPUProfilerTimer::EndFrame(HWRendererAPI* rendererAPI) {
    rendererAPI->EndQuery(s_EndQueries[0][s_CurrentFrameIndex]);
    rendererAPI->EndQuery(s_TimestampDisjointQueries[s_CurrentFrameIndex]);
    if (s_FirstPass && s_CurrentFrameIndex == (GPU_PROFILER_FRAME_LATENCY - 1)) {
        s_FirstPass = false;
    }
    s_CurrentFrameIndex = ++s_CurrentFrameIndex % GPU_PROFILER_FRAME_LATENCY;
}

void PGGPUProfilerTimer::GetProfileList(HWRendererAPI* rendererAPI, ProfileList& outProfileList) {
    if (s_FirstPass) {
        return;
    }

    size_t fetchFrameIndex = s_CurrentFrameIndex;
    D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
    while (!rendererAPI->GetDataQuery(s_TimestampDisjointQueries[fetchFrameIndex], &disjointData, sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT), 0)) {}

    if (disjointData.Disjoint) {
        return;
    }

    // Frame timer
    {
        uint64_t tsStart, tsEnd;
        rendererAPI->GetDataQuery(s_StartQueries[0][fetchFrameIndex], &tsStart, sizeof(uint64_t), 0);
        rendererAPI->GetDataQuery(s_EndQueries[0][fetchFrameIndex], &tsEnd, sizeof(uint64_t), 0);

        uint64_t startNanoSeconds = (tsStart * 1000000000) / disjointData.Frequency;
        uint64_t durationNanoSeconds = ((tsEnd - tsStart) * 1000000000) / disjointData.Frequency;

        ProfileData profileData;
        profileData.name = "GPU Frame";
        profileData.startNanoSeconds = startNanoSeconds;
        profileData.durationNanoSeconds = durationNanoSeconds;

        outProfileList.push_back(profileData);
    }

    for (auto& entry : s_ProfileEntries) {
        size_t queryIndex = entry.second;
        uint64_t tsStart, tsEnd;
        rendererAPI->GetDataQuery(s_StartQueries[queryIndex][fetchFrameIndex], &tsStart, sizeof(uint64_t), 0);
        rendererAPI->GetDataQuery(s_EndQueries[queryIndex][fetchFrameIndex], &tsEnd, sizeof(uint64_t), 0);

        uint64_t startNanoSeconds = (tsStart * 1000000000) / disjointData.Frequency;
        uint64_t durationNanoSeconds = ((tsEnd - tsStart) * 1000000000) / disjointData.Frequency;

        ProfileData profileData;
        profileData.name = entry.first;
        profileData.startNanoSeconds = startNanoSeconds;
        profileData.durationNanoSeconds = durationNanoSeconds;

        outProfileList.push_back(profileData);
    }
}


