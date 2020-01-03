#pragma once

#include "../HWRendererAPI.h"

#include "DX11Buffer.h"
#include "DX11Shader.h"
#include "DX11InputLayout.h"
#include "DX11RenderTargets.h"
#include "DX11Texture2D.h"
#include "DX11ShaderResourceView.h"
#include "DX11SamplerState.h"

#include <d3d11.h>

class DX11RendererAPI : public HWRendererAPI {
    friend class SceneRenderPass;
public:
    DX11RendererAPI(PGWindow* window);
    ~DX11RendererAPI() override;

    void ClearScreen(const float* color) override;
    void Draw(HWVertexBuffer* vertexBuffer) override;
    void Draw(size_t vertexCount, size_t vertexBaseLocation) override;
    void DrawIndexed(HWIndexBuffer* indexBuffer) override;
    void Present() override;

    HWRenderTargetView* GetBackbufferRenderTargetView() override;
    HWViewport GetDefaultViewport() override;

    HWConstantBuffer* CreateConstantBuffer(void* bufferData, size_t size) override;
    HWVertexBuffer* CreateVertexBuffer(void* bufferData, size_t size, size_t strideSize) override;
    HWIndexBuffer* CreateIndexBuffer(uint32_t* bufferData, size_t count) override;
    HWShaderProgram* CreateShaderProgramFromBinarySource(ShaderFileData* vertexShaderFileData, ShaderFileData* pixelShaderFileData) override;
    HWVertexInputLayout* CreateVertexInputLayout(std::vector<VertexInputElement> inputElements, HWShaderProgram* shaderProgram) override;
    HWTexture2D* CreateTexture2D(Texture2DInitParams* initParams) override;
    HWRenderTargetView* CreateRenderTargetView(HWTexture2D* texture, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount) override;
    HWDepthStencilView* CreateDepthStencilView(HWTexture2D* texture, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount) override;
    HWShaderResourceView* CreateShaderResourceView(HWTexture2D* texture) override;
    HWSamplerState* CreateSamplerState(SamplerStateInitParams* initParams) override;


    void SetVertexBuffer(HWVertexBuffer* vertexBuffer, size_t stride) override;
    void SetIndexBuffer(HWIndexBuffer* indexBuffer) override;
    void SetInputLayout(HWVertexInputLayout* vertexInputLayout) override;
    void SetShaderProgram(HWShaderProgram* shaderProgram) override;
    void SetRenderTargets(HWRenderTargetView** renderTargets, size_t renderTargetCount, HWDepthStencilView* depthStencilView) override;
    void SetShaderResourcesVS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) override;
    void SetShaderResourcesPS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) override;
    void SetSamplerStatesVS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) override;
    void SetSamplerStatesPS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) override;

    void SetViewport(HWViewport* viewport) override;

    void SetConstanBuffersVS(size_t startSlot, HWConstantBuffer** constantBuffers, size_t count) override;
    void SetConstanBuffersPS(size_t startSlot, HWConstantBuffer** constantBuffers, size_t count) override;

    void ClearRenderTarget(HWRenderTargetView* renderTargetView, float color[4]) override;
    void ClearDepthStencilView(HWDepthStencilView* depthStencilView, bool clearStencil, float depthClearData, uint8_t stencilClearData) override;

    void* Map(HWConstantBuffer* resource) override;
    void Unmap(HWConstantBuffer* resource) override;

    void MSAAResolve(HWTexture2D* dest, HWTexture2D* source) override;


    ID3D11Device* GetDX11Device() { return m_Device; }
    ID3D11DeviceContext* GetDX11DeviceContext() { return m_DeviceContext; }

    size_t GetWidth() override { return m_ClientWidth; }
    size_t GetHeight() override { return m_ClientHeight; }

private:
    IDXGISwapChain* m_SwapChain = nullptr;
    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_DeviceContext = nullptr;

    DX11RenderTargetView* m_BackbufferRenderTargetView = nullptr;
    D3D11_VIEWPORT m_DefaultViewport;

    size_t m_ClientWidth;
    size_t m_ClientHeight;
};