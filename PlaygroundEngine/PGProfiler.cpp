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



