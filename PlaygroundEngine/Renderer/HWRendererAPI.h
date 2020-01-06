#pragma once

#include "../Core.h"
#include "../Platform/PGWindow.h"

#include "HWBuffer.h"
#include "HWShader.h"
#include "HWInputLayout.h"
#include "HWTexture2D.h"
#include "HWRenderTarget.h"
#include "HWShaderResourceView.h"
#include "HWSamplerState.h"

#include <vector>

struct HWViewport {
    float topLeftX;
    float topLeftY;
    float width;
    float height;
};

class HWRendererAPI {
public:
    virtual ~HWRendererAPI() = default;

    virtual void ResizeBackBuffer(size_t clientWidth, size_t clientHeight) = 0;

    virtual void ClearScreen(const float* color) = 0;
    virtual void Draw(HWVertexBuffer* indexBuffer) = 0;
    virtual void Draw(size_t vertexCount, size_t vertexBaseLocation) = 0;
    virtual void DrawIndexed(HWIndexBuffer* indexBuffer) = 0;
    virtual void Present() = 0;

    virtual HWRenderTargetView* GetBackbufferRenderTargetView() = 0;
    virtual HWViewport GetDefaultViewport() = 0;

    virtual HWConstantBuffer* CreateConstantBuffer(void* bufferData, size_t size) = 0;
    virtual HWVertexBuffer* CreateVertexBuffer(void* bufferData, size_t size, size_t strideSize) = 0;
    virtual HWIndexBuffer* CreateIndexBuffer(uint32_t* bufferData, size_t count) = 0;
    virtual HWShaderProgram* CreateShaderProgramFromBinarySource(ShaderFileData* vertexShaderFileData, ShaderFileData* pixelShaderFileData) = 0;
    virtual HWVertexInputLayout* CreateVertexInputLayout(std::vector<VertexInputElement> inputElements, HWShaderProgram* shaderProgram) = 0;
    virtual HWTexture2D* CreateTexture2D(Texture2DDesc* initParams) = 0;
    virtual HWRenderTargetView* CreateRenderTargetView(HWTexture2D* texture, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip = 0, uint32_t mipCount = 1) = 0;
    virtual HWDepthStencilView* CreateDepthStencilView(HWTexture2D* texture, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip = 0, uint32_t mipCount = 1) = 0;
    virtual HWShaderResourceView* CreateShaderResourceView(HWTexture2D* texture) = 0;
    virtual HWSamplerState* CreateSamplerState(SamplerStateInitParams* initParams) = 0;

    virtual void SetVertexBuffer(HWVertexBuffer* vertexBuffer, size_t stride) = 0;
    virtual void SetIndexBuffer(HWIndexBuffer* indexBuffer) = 0;
    virtual void SetInputLayout(HWVertexInputLayout* vertexInputLayout) = 0;
    virtual void SetShaderProgram(HWShaderProgram* shaderProgram) = 0;
    virtual void SetRenderTargets(HWRenderTargetView** renderTargets, size_t renderTargetCount, HWDepthStencilView* depthStencilView) = 0;

    virtual void SetShaderResourcesVS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) = 0;
    virtual void SetShaderResourcesPS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) = 0;
    virtual void SetSamplerStatesVS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) = 0;
    virtual void SetSamplerStatesPS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) = 0;

    virtual void SetViewport(HWViewport* viewport) = 0;
    
    virtual void SetConstanBuffersVS(size_t startSlot, HWConstantBuffer** constantBuffers, size_t count) = 0;
    virtual void SetConstanBuffersPS(size_t startSlot, HWConstantBuffer** constantBuffers, size_t count) = 0;

    virtual void ClearRenderTarget(HWRenderTargetView* renderTargetView, float color[4]) = 0;
    virtual void ClearDepthStencilView(HWDepthStencilView* depthStencilView, bool clearStencil, float depthClearData, uint8_t stencilClearData) = 0;

    virtual void* Map(HWConstantBuffer* resource) = 0;
    virtual void Unmap(HWConstantBuffer* resource) = 0;

    virtual void MSAAResolve(HWTexture2D* dest, HWTexture2D* source) = 0;

    virtual size_t GetWidth() = 0;
    virtual size_t GetHeight() = 0;
};

