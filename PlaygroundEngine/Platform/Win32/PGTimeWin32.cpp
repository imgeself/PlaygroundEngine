#include "../PGTime.h"

#include <Windows.h>

inline uint64_t PGTime::GetTimeNanoseconds() {
    // TODO: This operation may not be success. Check return value.
    LARGE_INTEGER time, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&time);

    time.QuadPart *= 1000000000;
    time.QuadPart /= frequency.QuadPart;
    return time.QuadPart;
}

inline uint64_t PGTime::GetTimeMacroseconds() {
    uint64_t nanoseconds = GetTimeNanoseconds();
    return nanoseconds / 1000;
}

inline uint64_t PGTime::GetTimeMilliseconds() {
    uint64_t macroseconds = GetTimeMacroseconds();
    return macroseconds / 1000;
}

inline uint64_t PGTime::GetTimeSeconds() {
    uint64_t milliseconds = GetTimeMilliseconds();
    return milliseconds / 1000;
}
