#pragma once

#include "Core.h"

class IApplication {
public:
    virtual ~IApplication() = default;

    virtual void OnInit() = 0;
    virtual void OnUpdate(float deltaTime) = 0;
    virtual void OnUIRender() = 0;
    virtual void OnExit() = 0;
};

