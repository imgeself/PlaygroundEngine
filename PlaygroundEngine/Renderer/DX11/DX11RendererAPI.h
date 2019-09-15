#pragma once

#include "../PGRendererAPI.h"

#include "DX11Buffer.h"
#include "DX11Shader.h"
#include "DX11InputLayout.h"

#include <d3d11.h>

class DX11RendererAPI : public IRendererAPI {
public:
    DX11RendererAPI(PGWindow* window);
    ~DX11RendererAPI() override;

    void ClearScreen(const float* color) override;
    void Draw(IVertexBuffer* vertexBuffer) override;
    void DrawIndexed(IIndexBuffer* indexBuffer) override;
    void Present() override;

    IConstantBuffer* CreateConstantBuffer(void* bufferData, size_t size) override;
    IVertexBuffer* CreateVertexBuffer(void* bufferData, size_t size, size_t strideSize) override;
    IIndexBuffer* CreateIndexBuffer(uint32_t* bufferData, uint32_t count) override;
    IShaderProgram* CreateShaderProgram(const char* vertexShaderFileName, const char* pixelShaderFileName) override;
    IVertexInputLayout* CreateVertexInputLayout(std::vector<VertexInputElement> inputElements, IShaderProgram* shaderProgram) override;

    void SetVertexBuffer(IVertexBuffer* vertexBuffer, uint32_t stride) override;
    void SetIndexBuffer(IIndexBuffer* indexBuffer) override;
    void SetInputLayout(IVertexInputLayout* vertexInputLayout) override;
    void SetShaderProgram(IShaderProgram* shaderProgram) override;

    void SetConstanBufferVS(IConstantBuffer* constantBuffer) override;
    void SetConstanBufferPS(IConstantBuffer* constantBuffer) override;

    ID3D11Device* GetDX11Device() { return m_Device; }
    ID3D11DeviceContext* GetDX11DeviceContext() { return m_DeviceContext; }

private:
    IDXGISwapChain* m_SwapChain = nullptr;
    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_DeviceContext = nullptr;
    ID3D11RenderTargetView* m_BackbufferRenderTargetView = nullptr;
    ID3D11DepthStencilView* m_BackbufferDepthStencilView = nullptr;

};