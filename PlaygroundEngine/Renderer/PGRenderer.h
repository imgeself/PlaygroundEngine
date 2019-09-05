#pragma once

#include "../Core.h"
#include "../Platform/PGWindow.h"

#include "PGBuffer.h"

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void ClearScreen(const float* color) = 0;
    virtual void Render() = 0;
    virtual void EndFrame() = 0;

    virtual IVertexBuffer* CreateVertexBuffer(void* bufferData, size_t size) = 0;
    virtual IIndexBuffer* CreateIndexBuffer(void* bufferData, size_t size) = 0;
};

