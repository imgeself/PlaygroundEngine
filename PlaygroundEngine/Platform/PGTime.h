#pragma once

#include "../Core.h"

// Returns time since the device was last rebooted
class PG_API PGTime {
public:
    static uint64_t GetTimeNanoseconds();
    static uint64_t GetTimeSeconds();
    static uint64_t GetTimeMilliseconds();
    static uint64_t GetTimeMacroseconds();
};

