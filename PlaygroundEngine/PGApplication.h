#pragma once

#include "Events/PGEvent.h"

class IApplication : public ISystemEventListener {
public:
    virtual ~IApplication() = default;

    virtual void OnSystemEvent(SystemEvent event, uint64_t param1, uint64_t param2) = 0;

    virtual void OnInit() = 0;
    virtual void OnUpdate(float deltaTime) = 0;
    virtual void OnUIRender() = 0;
    virtual void OnExit() = 0;
};

