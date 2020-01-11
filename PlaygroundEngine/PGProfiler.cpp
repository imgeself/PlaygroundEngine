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
    auto search = std::find(s_ProfileResults.begin(), s_ProfileResults.end(), profileData);
    // Update data if exists, otherwise add into the list.
    // It is a set data structure behavior but we implemented using vector because we need to sort the list after
    if (search != s_ProfileResults.end()) {
        *search = profileData;
    } else {
        s_ProfileResults.push_back(profileData);
    }

    isStopped = true;
}



