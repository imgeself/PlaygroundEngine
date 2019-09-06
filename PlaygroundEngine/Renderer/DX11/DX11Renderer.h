#pragma once

#include "../PGRenderer.h"

#include "DX11Buffer.h"
#include "DX11Shader.h"
#include "DX11InputLayout.h"

#include <d3d11.h>

class DX11Renderer : public IRenderer {
public:
    DX11Renderer(PGWindow* window);
    ~DX11Renderer() override;

    void ClearScreen(const float* color) override;
    void Render() override;
    void EndFrame() override;

    IVertexBuffer* CreateVertexBuffer(void* bufferData, size_t size) override;
    IIndexBuffer* CreateIndexBuffer(void* bufferData, size_t size) override;
    IShaderProgram* CreateShaderProgram(const char* vertexShaderFileName, const char* pixelShaderFileName) override;
    IVertexInputLayout* CreateVertexInputLayout(std::vector<VertexInputElement> inputElements, IShaderProgram* shaderProgram) override;

private:
    IDXGISwapChain* m_SwapChain = nullptr;
    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_DeviceContext = nullptr;
    ID3D11RenderTargetView* m_BackbufferRenderTargetView = nullptr;
    ID3D11DepthStencilView* m_BackbufferDepthStencilView = nullptr;

};