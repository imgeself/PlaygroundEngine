#pragma once

#include "HWBuffer.h"
#include "HWTexture2D.h"
#include "HWRenderTarget.h"
#include "HWShaderResourceView.h"
#include "HWSamplerState.h"
#include "HWPipelineStates.h"

#include <vector>
#include <string>

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
    virtual void Draw(size_t vertexCount, size_t vertexBaseLocation) = 0;
    virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertexIndex) = 0;
    virtual void Present() = 0;

    virtual HWRenderTargetView* GetBackbufferRenderTargetView() = 0;
    virtual HWViewport GetDefaultViewport() = 0;

    virtual HWBuffer* CreateBuffer(SubresourceData* subresource, size_t size, uint32_t flags, const char* debugName = 0) = 0;
    virtual HWTexture2D* CreateTexture2D(Texture2DDesc* initParams, SubresourceData* subresources, const char* debugName = 0) = 0;
    virtual HWRenderTargetView* CreateRenderTargetView(HWTexture2D* texture, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip = 0, uint32_t mipCount = 1, const char* debugName = 0) = 0;
    virtual HWDepthStencilView* CreateDepthStencilView(HWTexture2D* texture, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip = 0, uint32_t mipCount = 1, const char* debugName = 0) = 0;
    virtual HWShaderResourceView* CreateShaderResourceView(HWTexture2D* texture, const char* debugName = 0) = 0;
    virtual HWSamplerState* CreateSamplerState(SamplerStateInitParams* initParams, const char* debugName = 0) = 0;

    virtual HWPipelineState* CreatePipelineState(const HWPipelineStateDesc& pipelineDesc, const char* debugName = 0) = 0;

    virtual void SetVertexBuffers(HWBuffer** vertexBuffers, size_t vertexBufferCount, uint32_t* strideByteCounts, uint32_t* offsets) = 0;
    virtual void SetIndexBuffer(HWBuffer* indexBuffer, uint32_t strideByteCount, uint32_t offset) = 0;
    virtual void SetRenderTargets(HWRenderTargetView** renderTargets, size_t renderTargetCount, HWDepthStencilView* depthStencilView) = 0;

    virtual void SetShaderResourcesVS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) = 0;
    virtual void SetShaderResourcesPS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) = 0;
    virtual void SetSamplerStatesVS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) = 0;
    virtual void SetSamplerStatesPS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) = 0;

    virtual void SetPipelineState(HWPipelineState* pipelineState) = 0;

    virtual void SetViewport(HWViewport* viewport) = 0;
    
    virtual void SetConstantBuffersVS(size_t startSlot, HWBuffer** constantBuffers, size_t count) = 0;
    virtual void SetConstantBuffersPS(size_t startSlot, HWBuffer** constantBuffers, size_t count) = 0;

    virtual void ClearRenderTarget(HWRenderTargetView* renderTargetView, float color[4]) = 0;
    virtual void ClearDepthStencilView(HWDepthStencilView* depthStencilView, bool clearStencil, float depthClearData, uint8_t stencilClearData) = 0;

    virtual void* Map(HWResource* resource) = 0;
    virtual void Unmap(HWResource* resource) = 0;

    virtual void MSAAResolve(HWTexture2D* dest, HWTexture2D* source) = 0;

    virtual size_t GetWidth() = 0;
    virtual size_t GetHeight() = 0;
};

