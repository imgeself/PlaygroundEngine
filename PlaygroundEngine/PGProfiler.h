#pragma once

#include "Core.h"

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

#define PG_PROFILE_SCOPE(name) PGProfileTimer timer##__LINE__(name);
#define PG_PROFILE_FUNCTION() PG_PROFILE_SCOPE(__FUNCTION__)


