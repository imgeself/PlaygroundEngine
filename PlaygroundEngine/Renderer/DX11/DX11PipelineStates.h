#pragma once

#include "../HWPipelineStates.h"

#include <d3d11.h>

class DX11GraphicsPipelineState : public HWGraphicsPipelineState {
public:
    DX11GraphicsPipelineState(ID3D11Device* device, const HWGraphicsPipelineStateDesc& pipelineDesc, const char* debugName);
    ~DX11GraphicsPipelineState() override;

    inline ID3D11VertexShader* GetDXVertexShader() { return m_VertexShader; }
    inline ID3D11PixelShader* GetDXPixelShader() { return m_PixelShader; }

    inline ID3D11BlendState* GetDXBlendState() { return m_BlendState; }
    inline ID3D11RasterizerState* GetDXRasterizerState() { return m_RasterizerState; }
    inline ID3D11InputLayout* GetDXInputLayout() { return m_InputLayout; }
    inline ID3D11DepthStencilState* GetDXDepthStencilState() { return m_DepthStencilState; }

    inline D3D11_PRIMITIVE_TOPOLOGY GetDXPrimitiveTopology() { return m_PrimitiveTopology; }
    inline uint32_t GetSampleMask() { return m_SampleMask; }

private:
    ID3D11VertexShader* m_VertexShader = nullptr;
    ID3D11PixelShader* m_PixelShader = nullptr;

    ID3D11BlendState* m_BlendState = nullptr;
    ID3D11RasterizerState* m_RasterizerState = nullptr;
    ID3D11InputLayout* m_InputLayout = nullptr;
    ID3D11DepthStencilState* m_DepthStencilState = nullptr;

    D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
    uint32_t m_SampleMask;
};
