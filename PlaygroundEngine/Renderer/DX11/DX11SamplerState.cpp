#include "DX11SamplerState.h"

static inline D3D11_TEXTURE_ADDRESS_MODE GetD3DAddressModeFromAddressMode(SamplerStateTextureAddressMode mode) {
    D3D11_TEXTURE_ADDRESS_MODE result;
    switch (mode) {
        case TextureAddressMode_WRAP:
            result = D3D11_TEXTURE_ADDRESS_WRAP;
            break;
        case TextureAddressMode_MIRROR:
            result = D3D11_TEXTURE_ADDRESS_MIRROR;
            break;
        case TextureAddressMode_CLAMP:
            result = D3D11_TEXTURE_ADDRESS_CLAMP;
            break;
        case TextureAddressMode_BORDER:
            result = D3D11_TEXTURE_ADDRESS_BORDER;
            break;
        default:
            PG_ASSERT(false, "Unhandled sampler state texture address mode!");
            break;
    }

    return result;
}

static inline D3D11_FILTER GetD3DFilterFromFilterMode(SamplerStateTextureFilterMode mode) {
    // TODO; Handle mode filters
    D3D11_FILTER result;
    switch (mode) {
        case TextureFilterMode_POINT:
            result = D3D11_FILTER_MIN_MAG_MIP_POINT;
            break;
        case TextureFilterMode_LINEAR:
            result = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            break;
        default:
            PG_ASSERT(false, "Unhandled sampler state texture filter mode!");
            break;
    }

    return result;
}

DX11SamplerState::DX11SamplerState(ID3D11Device* device, SamplerStateInitParams* initParams) {
    D3D11_SAMPLER_DESC samplerStateDesc = {};
    samplerStateDesc.Filter = GetD3DFilterFromFilterMode(initParams->filterMode);
    samplerStateDesc.AddressU = GetD3DAddressModeFromAddressMode(initParams->AddressModeU);
    samplerStateDesc.AddressV = GetD3DAddressModeFromAddressMode(initParams->AddressModeV);
    samplerStateDesc.AddressW = GetD3DAddressModeFromAddressMode(initParams->AddressModeW);
    memcpy(samplerStateDesc.BorderColor, initParams->borderColor, sizeof(float) * 4);

    HRESULT result = device->CreateSamplerState(&samplerStateDesc, &m_SamplerState);
    PG_ASSERT(SUCCEEDED(result), "Error at creating sampler state");
}

DX11SamplerState::~DX11SamplerState() {
    SAFE_RELEASE(m_SamplerState);
}
