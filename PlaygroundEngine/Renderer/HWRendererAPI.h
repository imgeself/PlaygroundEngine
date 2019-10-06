#pragma once

#include "../Core.h"
#include "../Platform/PGWindow.h"

#include "HWBuffer.h"
#include "HWShader.h"
#include "HWInputLayout.h"

#include <vector>

class HWRendererAPI {
public:
    virtual ~HWRendererAPI() = default;

    virtual void ClearScreen(const float* color) = 0;
    virtual void Draw(HWVertexBuffer* indexBuffer) = 0;
    virtual void DrawIndexed(HWIndexBuffer* indexBuffer) = 0;
    virtual void Present() = 0;

    virtual HWConstantBuffer* CreateConstantBuffer(void* bufferData, size_t size) = 0;
    virtual HWVertexBuffer* CreateVertexBuffer(void* bufferData, size_t size, size_t strideSize) = 0;
    virtual HWIndexBuffer* CreateIndexBuffer(uint32_t* bufferData, size_t count) = 0;
    virtual HWShaderProgram* CreateShaderProgram(ShaderFileData shaderFileData) = 0;
    virtual HWVertexInputLayout* CreateVertexInputLayout(std::vector<VertexInputElement> inputElements, HWShaderProgram* shaderProgram) = 0;

    virtual void SetVertexBuffer(HWVertexBuffer* vertexBuffer, size_t stride) = 0;
    virtual void SetIndexBuffer(HWIndexBuffer* indexBuffer) = 0;
    virtual void SetInputLayout(HWVertexInputLayout* vertexInputLayout) = 0;
    virtual void SetShaderProgram(HWShaderProgram* shaderProgram) = 0;
    
    virtual void SetConstanBuffersVS(HWConstantBuffer** constantBuffers, size_t count) = 0;
    virtual void SetConstanBuffersPS(HWConstantBuffer** constantBuffers, size_t count) = 0;

    virtual void SetShaderConstantBuffers(HWShaderProgram* shaderProgram) = 0;
};

