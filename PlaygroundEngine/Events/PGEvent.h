#pragma once

#include "../Core.h"
#include <stdint.h>

enum SystemEvent {
    INITIALIZE,
    // Trigger resize event when window resized
    // param1 = client width, param2 = client height
    RESIZE,
    CLOSE,
};

class PG_API ISystemEventListener {
public:
    virtual ~ISystemEventListener() = default;
    virtual void OnSystemEvent(SystemEvent event, uint64_t param1, uint64_t param2) = 0;
};
