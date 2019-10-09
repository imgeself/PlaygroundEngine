#pragma once

#include "../HWRendererAPI.h"

#include "DX11Buffer.h"
#include "DX11Shader.h"
#include "DX11InputLayout.h"

#include <d3d11.h>

class DX11RendererAPI : public HWRendererAPI {
public:
    DX11RendererAPI(PGWindow* window);
    ~DX11RendererAPI() override;

    void ClearScreen(const float* color) override;
    void Draw(HWVertexBuffer* vertexBuffer) override;
    void DrawIndexed(HWIndexBuffer* indexBuffer) override;
    void Present() override;

    HWConstantBuffer* CreateConstantBuffer(void* bufferData, size_t size) override;
    HWVertexBuffer* CreateVertexBuffer(void* bufferData, size_t size, size_t strideSize) override;
    HWIndexBuffer* CreateIndexBuffer(uint32_t* bufferData, size_t count) override;
    HWShaderProgram* CreateShaderProgramFromSource(ShaderFileData* shaderFileData) override;
    HWShaderProgram* CreateShaderProgramFromBinarySource(ShaderFileData* vertexShaderFileData, ShaderFileData* pixelShaderFileData) override;
    HWVertexInputLayout* CreateVertexInputLayout(std::vector<VertexInputElement> inputElements, HWShaderProgram* shaderProgram) override;

    void SetVertexBuffer(HWVertexBuffer* vertexBuffer, size_t stride) override;
    void SetIndexBuffer(HWIndexBuffer* indexBuffer) override;
    void SetInputLayout(HWVertexInputLayout* vertexInputLayout) override;
    void SetShaderProgram(HWShaderProgram* shaderProgram) override;

    void SetConstanBuffersVS(HWConstantBuffer** constantBuffers, size_t count) override;
    void SetConstanBuffersPS(HWConstantBuffer** constantBuffers, size_t count) override;

    void* Map(HWConstantBuffer* resource) override;
    void Unmap(HWConstantBuffer* resource) override;


    ID3D11Device* GetDX11Device() { return m_Device; }
    ID3D11DeviceContext* GetDX11DeviceContext() { return m_DeviceContext; }

private:
    IDXGISwapChain* m_SwapChain = nullptr;
    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_DeviceContext = nullptr;
    ID3D11RenderTargetView* m_BackbufferRenderTargetView = nullptr;
    ID3D11DepthStencilView* m_BackbufferDepthStencilView = nullptr;

};