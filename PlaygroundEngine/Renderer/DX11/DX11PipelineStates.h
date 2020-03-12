#pragma once

#include "../HWPipelineStates.h"

#include <d3d11.h>

class DX11BlendState : public HWBlendState {
public:
    DX11BlendState(ID3D11Device* device, const HWBlendDesc& blendDesc, const char* debugName);
    ~DX11BlendState() override;

    inline ID3D11BlendState* GetDXBlendState() { return m_BlendState; }

private:
    ID3D11BlendState* m_BlendState = nullptr;
};

class DX11RasterizerState : public HWRasterizerState {
public:
    DX11RasterizerState(ID3D11Device* device, const HWRasterizerDesc& blendDesc, const char* debugName);
    ~DX11RasterizerState() override;

    inline ID3D11RasterizerState* GetDXRasterizerState() { return m_RasterizerState; }

private:
    ID3D11RasterizerState* m_RasterizerState = nullptr;
};
