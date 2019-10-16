#pragma once

#include "../HWSamplerState.h"

#include <d3d11.h>

class DX11SamplerState : public HWSamplerState {
public:
    DX11SamplerState(ID3D11Device* device, SamplerStateInitParams* initParams);
    ~DX11SamplerState();

    inline ID3D11SamplerState* GetDXSamplerState() { return m_SamplerState; }

private:
    ID3D11SamplerState* m_SamplerState = nullptr;
};

