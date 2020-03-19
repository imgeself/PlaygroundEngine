#include "DX11SamplerState.h"

static inline D3D11_TEXTURE_ADDRESS_MODE GetD3DAddressModeFromAddressMode(SamplerStateTextureAddressMode mode) {
    D3D11_TEXTURE_ADDRESS_MODE result = {};
    switch (mode) {
        case TextureAddressMode_NONE:
            break;
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

static inline D3D11_FILTER GetD3DFilterFromFilterMode(SamplerStateTextureFilterMode mode, ComparisonFunction cmpFunc) {
    // TODO; Handle mode filters
    D3D11_FILTER result;
    bool isComparison = cmpFunc != ComparisonFunction::NONE;
    switch (mode) {
        case TextureFilterMode_MIN_MAG_MIP_POINT:
            result = isComparison ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_POINT;
            break;
        case TextureFilterMode_MIN_MAG_MIP_LINEAR:
            result = isComparison ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            break;
        case TextureFilterMode_MIN_MAG_LINEAR_MIP_POINT:
            result = isComparison ? D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT : D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            break;
        case TextureFilterMode_ANISOTROPIC:
            result = isComparison ? D3D11_FILTER_COMPARISON_ANISOTROPIC: D3D11_FILTER_ANISOTROPIC;
            break;
        default:
            PG_ASSERT(false, "Unhandled sampler state texture filter mode!");
            break;
    }

    return result;
}

static inline D3D11_COMPARISON_FUNC GetD3DComparisonFunc(ComparisonFunction func) {
    D3D11_COMPARISON_FUNC result = {};
    switch (func) {
        case NONE:
            break;
        case NEVER:
            result = D3D11_COMPARISON_NEVER;
            break;
        case LESS:
            result = D3D11_COMPARISON_LESS;
            break;
        case EQUAL:
            result = D3D11_COMPARISON_EQUAL;
            break;
        case LESS_EQUAL:
            result = D3D11_COMPARISON_LESS_EQUAL;
            break;
        case GREATER:
            result = D3D11_COMPARISON_GREATER;
            break;
        case NOT_EQUAL:
            result = D3D11_COMPARISON_NOT_EQUAL;
            break;
        case GREATER_EQUAL:
            result = D3D11_COMPARISON_GREATER_EQUAL;
            break;
        case ALWAYS:
            result = D3D11_COMPARISON_ALWAYS;
            break;
        default:
            PG_ASSERT(false, "Unhandled sampler state comparison function!");
            break;
    }

    return result;
}

DX11SamplerState::DX11SamplerState(ID3D11Device* device, SamplerStateInitParams* initParams, const char* debugName) {
    D3D11_SAMPLER_DESC samplerStateDesc = {};
    samplerStateDesc.Filter = GetD3DFilterFromFilterMode(initParams->filterMode, initParams->comparisonFunction);
    samplerStateDesc.AddressU = GetD3DAddressModeFromAddressMode(initParams->addressModeU);
    samplerStateDesc.AddressV = GetD3DAddressModeFromAddressMode(initParams->addressModeV);
    samplerStateDesc.AddressW = GetD3DAddressModeFromAddressMode(initParams->addressModeW);
    samplerStateDesc.ComparisonFunc = GetD3DComparisonFunc(initParams->comparisonFunction);
    samplerStateDesc.MaxAnisotropy = initParams->maxAnisotropy;
    samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;
    memcpy(samplerStateDesc.BorderColor, initParams->borderColor, sizeof(float) * 4);

    HRESULT result = device->CreateSamplerState(&samplerStateDesc, &m_SamplerState);
    PG_ASSERT(SUCCEEDED(result), "Error at creating sampler state");

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_SamplerState->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) debugNameLen, debugName);
    }
#endif
}

DX11SamplerState::~DX11SamplerState() {
    SAFE_RELEASE(m_SamplerState);
}
