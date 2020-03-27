#pragma once

#include "../HWRendererAPI.h"
#include "../../Platform/PGWindow.h"

#include "DX11Buffer.h"
#include "DX11RenderTargets.h"
#include "DX11Texture2D.h"
#include "DX11ShaderResourceView.h"
#include "DX11SamplerState.h"
#include "DX11PipelineStates.h"

#include <d3d11_4.h>

class DX11RendererAPI : public HWRendererAPI {
public:
    DX11RendererAPI(PGWindow* window);
    ~DX11RendererAPI() override;

    void ResizeBackBuffer(size_t clientWidth, size_t clientHeight) override;

    void ClearScreen(const float* color) override;
    void Draw(size_t vertexCount, size_t vertexBaseLocation) override;
    void DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertexIndex) override;
    void Dispatch(uint32_t threadGroupX, uint32_t threadGroupY, uint32_t threadGroupZ) override;
    void Present() override;

    HWRenderTargetView* GetBackbufferRenderTargetView() override;
    HWViewport GetDefaultViewport() override;

    HWBuffer* CreateBuffer(SubresourceData* subresource, size_t size, uint32_t flags, const char* debugName) override;
    HWTexture2D* CreateTexture2D(Texture2DDesc* initParams, SubresourceData* subresources, const char* debugName) override;
    HWRenderTargetView* CreateRenderTargetView(HWTexture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName) override;
    HWDepthStencilView* CreateDepthStencilView(HWTexture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName) override;
    HWShaderResourceView* CreateShaderResourceView(HWTexture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName) override;
    HWUnorderedAccessView* CreateUnorderedAccessView(HWTexture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName) override;
    HWSamplerState* CreateSamplerState(SamplerStateInitParams* initParams, const char* debugName) override;

    HWGraphicsPipelineState* CreateGraphicsPipelineState(const HWGraphicsPipelineStateDesc& pipelineDesc, const char* debugName = 0) override;
    HWComputePipelineState* CreateComputePipelineState(const HWComputePipelineStateDesc& pipelineDesc, const char* debugName = 0) override;

    void SetVertexBuffers(HWBuffer** vertexBuffers, size_t vertexBufferCount, uint32_t* strideByteCounts, uint32_t* offsets) override;
    void SetIndexBuffer(HWBuffer* indexBuffer, uint32_t strideByteCount, uint32_t offset) override;
    void SetRenderTargets(HWRenderTargetView** renderTargets, size_t renderTargetCount, HWDepthStencilView* depthStencilView) override;

    void SetShaderResourcesVS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) override;
    void SetShaderResourcesPS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) override;
    void SetShaderResourcesCS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) override;

    void SetUnorderedAcessViewsCS(size_t startSlot, HWUnorderedAccessView** unorderedAccessViews, size_t uavCount, uint32_t* uavInitialCounts) override;

    void SetSamplerStatesVS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) override;
    void SetSamplerStatesPS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) override;
    void SetSamplerStatesCS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) override;

    void SetGraphicsPipelineState(HWGraphicsPipelineState* pipelineState) override;
    void SetComputePipelineState(HWComputePipelineState* pipelineState) override;

    void SetViewport(HWViewport* viewport) override;

    void SetConstantBuffersVS(size_t startSlot, HWBuffer** constantBuffers, size_t count) override;
    void SetConstantBuffersPS(size_t startSlot, HWBuffer** constantBuffers, size_t count) override;
    void SetConstantBuffersCS(size_t startSlot, HWBuffer** constantBuffers, size_t count) override;

    void ClearRenderTarget(HWRenderTargetView* renderTargetView, float color[4]) override;
    void ClearDepthStencilView(HWDepthStencilView* depthStencilView, bool clearStencil, float depthClearData, uint8_t stencilClearData) override;

    void* Map(HWResource* resource) override;
    void Unmap(HWResource* resource) override;
    void UpdateSubresource(HWResource* resource, uint32_t dstSubresource, const HWBox* updateBox, const SubresourceData& subresourceData) override;

    void MSAAResolve(HWTexture2D* dest, HWTexture2D* source) override;
    void GenerateMips(HWShaderResourceView* shaderResourceView) override;

    void BeginEvent(const char* eventName) override;
    void EndEvent() override;

    ID3D11Device* GetDX11Device() { return m_Device; }
    ID3D11DeviceContext* GetDX11DeviceContext() { return m_DeviceContext; }

    size_t GetWidth() override { return m_ClientWidth; }
    size_t GetHeight() override { return m_ClientHeight; }

private:
    IDXGISwapChain* m_SwapChain = nullptr;
    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_DeviceContext = nullptr;
    ID3D11Debug* m_Debug = nullptr;
    ID3DUserDefinedAnnotation* m_UserDefinedAnnotation = nullptr;

    DX11RenderTargetView* m_BackbufferRenderTargetView = nullptr;
    D3D11_VIEWPORT m_DefaultViewport;

    size_t m_ClientWidth;
    size_t m_ClientHeight;
};