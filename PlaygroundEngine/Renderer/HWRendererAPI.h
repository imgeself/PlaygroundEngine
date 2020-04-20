#pragma once

#include "HWBuffer.h"
#include "HWTexture2D.h"
#include "HWRenderTarget.h"
#include "HWShaderResourceView.h"
#include "HWSamplerState.h"
#include "HWPipelineStates.h"
#include "HWQuery.h"

#include <vector>
#include <string>

struct HWViewport {
    float topLeftX;
    float topLeftY;
    float width;
    float height;
};

struct HWBox {
    uint32_t left;
    uint32_t top;
    uint32_t front;
    uint32_t right;
    uint32_t bottom;
    uint32_t back;
};

class HWRendererAPI {
public:
    virtual ~HWRendererAPI() = default;

    virtual void ResizeBackBuffer(size_t clientWidth, size_t clientHeight) = 0;

    virtual void ClearScreen(const float* color) = 0;
    virtual void Draw(size_t vertexCount, size_t vertexBaseLocation) = 0;
    virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertexIndex) = 0;
    virtual void Dispatch(uint32_t threadGroupX, uint32_t threadGroupY, uint32_t threadGroupZ) = 0;
    virtual void Present() = 0;

    virtual HWRenderTargetView* GetBackbufferRenderTargetView() = 0;
    virtual HWViewport GetDefaultViewport() = 0;

    virtual HWBuffer* CreateBuffer(SubresourceData* subresource, size_t size, uint32_t flags, const char* debugName = 0) = 0;
    virtual HWTexture2D* CreateTexture2D(Texture2DDesc* initParams, SubresourceData* subresources, const char* debugName = 0) = 0;
    virtual HWRenderTargetView* CreateRenderTargetView(HWTexture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName = 0) = 0;
    virtual HWDepthStencilView* CreateDepthStencilView(HWTexture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName = 0) = 0;
    virtual HWShaderResourceView* CreateShaderResourceView(HWTexture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName = 0) = 0;
    virtual HWUnorderedAccessView* CreateUnorderedAccessView(HWTexture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName = 0) = 0;
    virtual HWSamplerState* CreateSamplerState(SamplerStateInitParams* initParams, const char* debugName = 0) = 0;
    virtual HWQuery* CreateQuery(const HWQueryDesc& queryDesc, const char* debugName = 0) = 0;

    virtual HWGraphicsPipelineState* CreateGraphicsPipelineState(const HWGraphicsPipelineStateDesc& pipelineDesc, const char* debugName = 0) = 0;
    virtual HWComputePipelineState* CreateComputePipelineState(const HWComputePipelineStateDesc& pipelineDesc, const char* debugName = 0) = 0;

    virtual void SetVertexBuffers(HWBuffer** vertexBuffers, size_t vertexBufferCount, uint32_t* strideByteCounts, uint32_t* offsets) = 0;
    virtual void SetIndexBuffer(HWBuffer* indexBuffer, uint32_t strideByteCount, uint32_t offset) = 0;
    virtual void SetRenderTargets(HWRenderTargetView** renderTargets, size_t renderTargetCount, HWDepthStencilView* depthStencilView) = 0;

    virtual void SetShaderResourcesVS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) = 0;
    virtual void SetShaderResourcesPS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) = 0;
    virtual void SetShaderResourcesCS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) = 0;

    virtual void SetUnorderedAcessViewsCS(size_t startSlot, HWUnorderedAccessView** unorderedAccessViews, size_t uavCount, uint32_t* uavInitialCounts) = 0;

    virtual void SetSamplerStatesVS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) = 0;
    virtual void SetSamplerStatesPS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) = 0;
    virtual void SetSamplerStatesCS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) = 0;

    virtual void SetGraphicsPipelineState(HWGraphicsPipelineState* pipelineState) = 0;
    virtual void SetComputePipelineState(HWComputePipelineState* pipelineState) = 0;

    virtual void SetViewport(HWViewport* viewport) = 0;
    
    virtual void SetConstantBuffersVS(size_t startSlot, HWBuffer** constantBuffers, size_t count) = 0;
    virtual void SetConstantBuffersPS(size_t startSlot, HWBuffer** constantBuffers, size_t count) = 0;
    virtual void SetConstantBuffersCS(size_t startSlot, HWBuffer** constantBuffers, size_t count) = 0;

    virtual void ClearRenderTarget(HWRenderTargetView* renderTargetView, float color[4]) = 0;
    virtual void ClearDepthStencilView(HWDepthStencilView* depthStencilView, bool clearStencil, float depthClearData, uint8_t stencilClearData) = 0;

    virtual void* Map(HWResource* resource) = 0;
    virtual void Unmap(HWResource* resource) = 0;
    virtual void UpdateSubresource(HWResource* resource, uint32_t dstSubresource, const HWBox* updateBox, const SubresourceData& subresourceData) = 0;

    virtual void MSAAResolve(HWTexture2D* dest, HWTexture2D* source) = 0;
    virtual void GenerateMips(HWShaderResourceView* shaderResourceView) = 0;

    virtual void BeginEvent(const char* eventName) = 0;
    virtual void EndEvent() = 0;

    virtual void BeginQuery(HWQuery* query) = 0;
    virtual void EndQuery(HWQuery* query) = 0;
    virtual bool GetDataQuery(HWQuery* query, void* outData, uint32_t size, uint32_t flags) = 0;

    virtual size_t GetWidth() = 0;
    virtual size_t GetHeight() = 0;
};

