#pragma once

#include "Core.h"

class IApplication {
public:
    virtual ~IApplication() = default;

    virtual void OnInit() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnRender() = 0;
    virtual void OnExit() = 0;
};

