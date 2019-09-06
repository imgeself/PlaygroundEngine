#pragma once

#include "../Core.h"
#include "../Platform/PGWindow.h"

#include "PGBuffer.h"
#include "PGShader.h"
#include "PGInputLayout.h"

#include <vector>

class IRendererAPI {
public:
    virtual ~IRendererAPI() = default;

    virtual void ClearScreen(const float* color) = 0;
    virtual void Render() = 0;
    virtual void EndFrame() = 0;

    virtual IVertexBuffer* CreateVertexBuffer(void* bufferData, size_t size) = 0;
    virtual IIndexBuffer* CreateIndexBuffer(void* bufferData, size_t size) = 0;
    virtual IShaderProgram* CreateShaderProgram(const char* vertexShaderFileName, const char* pixelShaderFileName) = 0;
    virtual IVertexInputLayout* CreateVertexInputLayout(std::vector<VertexInputElement> inputElements, IShaderProgram* shaderProgram) = 0;
};

