#pragma once

#include "Core.h"
#include "Renderer/HWRendererAPI.h"

#include <string>
#include <vector>

struct ProfileData {
    std::string name;
    uint64_t startNanoSeconds;
    uint64_t durationNanoSeconds;
};

inline bool operator==(const ProfileData& left, const ProfileData& right) {
    return left.name == right.name;
}

typedef std::vector<ProfileData> ProfileList;

class PG_API PGProfileTimer {
public:
    PGProfileTimer(const char* name);
    ~PGProfileTimer();
    void Stop();

    // public members
    const char* name;
    uint64_t startNanoSeconds;
    bool isStopped;

    static ProfileList s_ProfileResults;
};

#define PG_PROFILE(name, line) PGProfileTimer timer##line(name);
// Need some extra layer of indirection for concatenating a line number instead of "__LINE__" string
// https://stackoverflow.com/a/1597129
#define PG_PROFILE2(name, line) PG_PROFILE(name, line)
#define PG_PROFILE_SCOPE(name) PG_PROFILE2(name, __LINE__)
#define PG_PROFILE_FUNCTION() PG_PROFILE_SCOPE(__FUNCTION__)

#define GPU_PROFILER_FRAME_LATENCY 4
#define GPU_PROFILER_MAX_QUERY_COUNT 20

class PG_API PGGPUProfilerTimer {
public:
    PGGPUProfilerTimer(HWRendererAPI* rendererAPI, const char* name);
    ~PGGPUProfilerTimer();
    void Stop(HWRendererAPI* rendererAPI, const char* name);

    // public members
    const char* name;
    HWRendererAPI* rendererAPI;
    bool isStopped;

// Static members
public:
    static void BeginFrame(HWRendererAPI* rendererAPI);
    static void EndFrame(HWRendererAPI* rendererAPI);

    static void Initialize(HWRendererAPI* rendererAPI);
    static void Destroy();

    static void GetProfileList(HWRendererAPI* rendererAPI, ProfileList& outProfileList);

private:
    static std::unordered_map<std::string, size_t> s_ProfileEntries;

    static bool s_FirstPass;
    static size_t s_CurrentFrameIndex;
    static size_t s_FreeQueryIndex;
    static HWQuery* s_TimestampDisjointQueries[GPU_PROFILER_FRAME_LATENCY];

    static HWQuery* s_StartQueries[GPU_PROFILER_MAX_QUERY_COUNT][GPU_PROFILER_FRAME_LATENCY];
    static HWQuery* s_EndQueries[GPU_PROFILER_MAX_QUERY_COUNT][GPU_PROFILER_FRAME_LATENCY];
};

#define PG_GPU_PROFILE(pRendererAPI, name, line) PGGPUProfilerTimer gputimer##line(pRendererAPI, name);
// Need some extra layer of indirection for concatenating a line number instead of "__LINE__" string
// https://stackoverflow.com/a/1597129
#define PG_GPU_PROFILE2(pRendererAPI, name, line) PG_GPU_PROFILE(pRendererAPI, name, line)
#define PG_GPU_PROFILE_SCOPE(pRendererAPI, name) PG_GPU_PROFILE2(pRendererAPI, name, __LINE__)
#define PG_GPU_PROFILE_FUNCTION(pRendererAPI) PG_GPU_PROFILE_SCOPE(pRendererAPI, __FUNCTION__)

