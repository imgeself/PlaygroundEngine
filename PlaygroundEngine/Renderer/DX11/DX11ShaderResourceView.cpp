#include "DX11ShaderResourceView.h"

DX11ShaderResourceView::DX11ShaderResourceView(ID3D11Device* device, ID3D11Texture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName) {
    D3D11_TEXTURE2D_DESC textureDesc;
    texture->GetDesc(&textureDesc);

    DXGI_FORMAT textureFormat = textureDesc.Format;
    DXGI_FORMAT shaderResourceViewFormat;
    switch (textureFormat) {
        case DXGI_FORMAT_R32_TYPELESS:
            shaderResourceViewFormat = DXGI_FORMAT_R32_FLOAT;
            break;
        case DXGI_FORMAT_R16_TYPELESS:
            shaderResourceViewFormat = DXGI_FORMAT_R16_UNORM;
            break;
        default:
            // TODO: Handle other typeless formats
            shaderResourceViewFormat = textureFormat;
            break;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResouceViewDesc = {};
    shaderResouceViewDesc.Format = shaderResourceViewFormat;
    if (textureDesc.SampleDesc.Count > 1) {
        if (textureDesc.ArraySize > 1) {
            shaderResouceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
            shaderResouceViewDesc.Texture2DMSArray.FirstArraySlice = resourceViewDesc.firstArraySlice;
            shaderResouceViewDesc.Texture2DMSArray.ArraySize = resourceViewDesc.sliceArrayCount;
        } else {
            shaderResouceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
        }
    } else {
        if (textureDesc.ArraySize > 6 && textureDesc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) {
            shaderResouceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
            shaderResouceViewDesc.TextureCubeArray.First2DArrayFace = resourceViewDesc.firstArraySlice;
            shaderResouceViewDesc.TextureCubeArray.NumCubes = resourceViewDesc.sliceArrayCount / 6;
            shaderResouceViewDesc.TextureCubeArray.MostDetailedMip = resourceViewDesc.firstMip;
            shaderResouceViewDesc.TextureCubeArray.MipLevels = resourceViewDesc.mipCount;
        } else if (textureDesc.ArraySize > 1 && textureDesc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) {
            shaderResouceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            shaderResouceViewDesc.TextureCube.MostDetailedMip = resourceViewDesc.firstMip;
            shaderResouceViewDesc.TextureCube.MipLevels = resourceViewDesc.mipCount;
        } else if (textureDesc.ArraySize > 1) {
            shaderResouceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            shaderResouceViewDesc.Texture2DArray.FirstArraySlice = resourceViewDesc.firstArraySlice;
            shaderResouceViewDesc.Texture2DArray.ArraySize = resourceViewDesc.sliceArrayCount;
            shaderResouceViewDesc.Texture2DArray.MostDetailedMip = resourceViewDesc.firstMip;
            shaderResouceViewDesc.Texture2DArray.MipLevels = resourceViewDesc.mipCount;
        } else {
            shaderResouceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResouceViewDesc.Texture2D.MostDetailedMip = resourceViewDesc.firstMip;
            shaderResouceViewDesc.Texture2D.MipLevels = resourceViewDesc.mipCount;
        }
    }

    HRESULT result = device->CreateShaderResourceView(texture, &shaderResouceViewDesc, &m_ShaderResourceView);
    PG_ASSERT(SUCCEEDED(result), "Error at creating shader resource view");

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_ShaderResourceView->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) debugNameLen, debugName);
    }
#endif
}

DX11ShaderResourceView::~DX11ShaderResourceView() {
    SAFE_RELEASE(m_ShaderResourceView);
}

DX11UnorderedAccessView::DX11UnorderedAccessView(ID3D11Device* device, ID3D11Texture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName) {
    D3D11_TEXTURE2D_DESC textureDesc;
    texture->GetDesc(&textureDesc);

    DXGI_FORMAT textureFormat = textureDesc.Format;
    DXGI_FORMAT shaderResourceViewFormat;
    switch (textureFormat) {
    case DXGI_FORMAT_R32_TYPELESS:
        shaderResourceViewFormat = DXGI_FORMAT_R32_FLOAT;
        break;
    case DXGI_FORMAT_R16_TYPELESS:
        shaderResourceViewFormat = DXGI_FORMAT_R16_UNORM;
        break;
    default:
        // TODO: Handle other typeless formats
        shaderResourceViewFormat = textureFormat;
        break;
    }

    D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc = {};
    unorderedAccessViewDesc.Format = shaderResourceViewFormat;
    if (textureDesc.ArraySize > 1) {
        unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
        unorderedAccessViewDesc.Texture2DArray.FirstArraySlice = resourceViewDesc.firstArraySlice;
        unorderedAccessViewDesc.Texture2DArray.ArraySize = resourceViewDesc.sliceArrayCount;
        unorderedAccessViewDesc.Texture2DArray.MipSlice = resourceViewDesc.firstMip;
    } else {
        unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
        unorderedAccessViewDesc.Texture2D.MipSlice = resourceViewDesc.firstMip;
    }

    HRESULT result = device->CreateUnorderedAccessView(texture, &unorderedAccessViewDesc, &m_UnorderedAccessView);
    PG_ASSERT(SUCCEEDED(result), "Error at creating unordered access view");

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_UnorderedAccessView->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)debugNameLen, debugName);
    }
#endif
}

DX11UnorderedAccessView::~DX11UnorderedAccessView() {
    SAFE_RELEASE(m_UnorderedAccessView);
}

