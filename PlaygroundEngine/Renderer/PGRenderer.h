#pragma once

#include "../Core.h"
#include "../Platform/PGWindow.h"

class IRenderer {
public:
    
    virtual ~IRenderer() = default;

    virtual void ClearScreen(const float* color) = 0;
    virtual void Render() = 0;
    virtual void EndFrame() = 0;
};

