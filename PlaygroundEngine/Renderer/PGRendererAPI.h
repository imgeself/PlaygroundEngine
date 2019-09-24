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
    virtual void Draw(IVertexBuffer* indexBuffer) = 0;
    virtual void DrawIndexed(IIndexBuffer* indexBuffer) = 0;
    virtual void Present() = 0;

    virtual IConstantBuffer* CreateConstantBuffer(void* bufferData, size_t size) = 0;
    virtual IVertexBuffer* CreateVertexBuffer(void* bufferData, size_t size, size_t strideSize) = 0;
    virtual IIndexBuffer* CreateIndexBuffer(uint32_t* bufferData, uint32_t count) = 0;
    virtual IShaderProgram* CreateShaderProgram(ShaderFileData vertexShaderFileData, ShaderFileData pixelShaderFileData) = 0;
    virtual IVertexInputLayout* CreateVertexInputLayout(std::vector<VertexInputElement> inputElements, IShaderProgram* shaderProgram) = 0;

    virtual void SetVertexBuffer(IVertexBuffer* vertexBuffer, uint32_t stride) = 0;
    virtual void SetIndexBuffer(IIndexBuffer* indexBuffer) = 0;
    virtual void SetInputLayout(IVertexInputLayout* vertexInputLayout) = 0;
    virtual void SetShaderProgram(IShaderProgram* shaderProgram) = 0;
    
    virtual void SetConstanBufferVS(IConstantBuffer* constantBuffer) = 0;
    virtual void SetConstanBufferPS(IConstantBuffer* constantBuffer) = 0;
};

