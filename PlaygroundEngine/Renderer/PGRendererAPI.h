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
    virtual void DrawIndexed(IIndexBuffer* indexBuffer) = 0;
    virtual void Present() = 0;

    virtual IVertexBuffer* CreateVertexBuffer(void* bufferData, size_t size) = 0;
    virtual IIndexBuffer* CreateIndexBuffer(uint32_t* bufferData, uint32_t count) = 0;
    virtual IShaderProgram* CreateShaderProgram(const char* vertexShaderFileName, const char* pixelShaderFileName) = 0;
    virtual IVertexInputLayout* CreateVertexInputLayout(std::vector<VertexInputElement> inputElements, IShaderProgram* shaderProgram) = 0;

    virtual void SetVertexBuffer(IVertexBuffer* vertexBuffer, uint32_t stride) = 0;
    virtual void SetIndexBuffer(IIndexBuffer* indexBuffer) = 0;
    virtual void SetInputLayout(IVertexInputLayout* vertexInputLayout) = 0;
    virtual void SetShaderProgram(IShaderProgram* shaderProgram) = 0;
};

