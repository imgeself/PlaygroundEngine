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
    void DrawIndexed(HWIndexBuffer* indexBuffer) override;
    void Present() override;

    HWRenderTargetView* GetBackbufferRenderTargetView() override;
    HWDepthStencilView* GetBackbufferDepthStencilView() override;
    HWViewport GetDefaultViewport() override;

    HWConstantBuffer* CreateConstantBuffer(void* bufferData, size_t size) override;
    HWVertexBuffer* CreateVertexBuffer(void* bufferData, size_t size, size_t strideSize) override;
    HWIndexBuffer* CreateIndexBuffer(uint32_t* bufferData, size_t count) override;
    HWShaderProgram* CreateShaderProgramFromSource(ShaderFileData* shaderFileData) override;
    HWShaderProgram* CreateShaderProgramFromBinarySource(ShaderFileData* vertexShaderFileData, ShaderFileData* pixelShaderFileData) override;
    HWVertexInputLayout* CreateVertexInputLayout(std::vector<VertexInputElement> inputElements, HWShaderProgram* shaderProgram) override;
    HWTexture2D* CreateTexture2D(Texture2DInitParams* initParams) override;
    HWRenderTargetView* CreateRenderTargetView(HWTexture2D* texture) override;
    HWDepthStencilView* CreateDepthStencilView(HWTexture2D* texture) override;
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


    ID3D11Device* GetDX11Device() { return m_Device; }
    ID3D11DeviceContext* GetDX11DeviceContext() { return m_DeviceContext; }

private:
    IDXGISwapChain* m_SwapChain = nullptr;
    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_DeviceContext = nullptr;
    //ID3D11RenderTargetView* m_BackbufferRenderTargetView = nullptr;
    //ID3D11DepthStencilView* m_BackbufferDepthStencilView = nullptr;

    DX11RenderTargetView* m_BackbufferRenderTargetView = nullptr;
    DX11DepthStencilView* m_BackbufferDepthStencilView = nullptr;
    D3D11_VIEWPORT m_DefaultViewport;

};