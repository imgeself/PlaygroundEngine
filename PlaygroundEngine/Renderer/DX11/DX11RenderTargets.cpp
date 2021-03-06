#include "DX11RenderTargets.h"

DX11RenderTargetView::DX11RenderTargetView(ID3D11Device* device, ID3D11Texture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName) {
    D3D11_TEXTURE2D_DESC textureDesc;
    texture->GetDesc(&textureDesc);

    DXGI_FORMAT textureFormat = textureDesc.Format;
    DXGI_FORMAT renderTargetViewFormat;
    switch (textureFormat) {
        case DXGI_FORMAT_R32_TYPELESS:
            renderTargetViewFormat = DXGI_FORMAT_R32_FLOAT;
            break;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            renderTargetViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        default:
            // TODO: Handle other typeless formats
            renderTargetViewFormat = textureFormat;
            break;
    }

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
    renderTargetViewDesc.Format = renderTargetViewFormat;
    if (textureDesc.SampleDesc.Count > 1) {
        if (textureDesc.ArraySize > 1) {
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
            renderTargetViewDesc.Texture2DMSArray.ArraySize = resourceViewDesc.sliceArrayCount;
            renderTargetViewDesc.Texture2DMSArray.FirstArraySlice = resourceViewDesc.firstArraySlice;
        } else {
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
        }
    } else {
        if (textureDesc.ArraySize > 1) {
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            renderTargetViewDesc.Texture2DArray.ArraySize = resourceViewDesc.sliceArrayCount;
            renderTargetViewDesc.Texture2DArray.FirstArraySlice = resourceViewDesc.firstArraySlice;
            renderTargetViewDesc.Texture2DArray.MipSlice = resourceViewDesc.firstMip;
        }
        else {
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = resourceViewDesc.firstMip;
        }
    }

    HRESULT result = device->CreateRenderTargetView(texture, &renderTargetViewDesc, &m_RenderTargetView);
    PG_ASSERT(SUCCEEDED(result), "Error at creating render target view");

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_RenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) debugNameLen, debugName);
    }
#endif
}

DX11RenderTargetView::~DX11RenderTargetView() {
    SAFE_RELEASE(m_RenderTargetView);
}

DX11DepthStencilView::DX11DepthStencilView(ID3D11Device* device, ID3D11Texture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName) {
    D3D11_TEXTURE2D_DESC textureDesc;
    texture->GetDesc(&textureDesc);

    DXGI_FORMAT textureFormat = textureDesc.Format;
    DXGI_FORMAT depthStencilViewFormat;
    switch (textureFormat) {
    case DXGI_FORMAT_R32_TYPELESS:
        depthStencilViewFormat = DXGI_FORMAT_D32_FLOAT;
        break;
    case DXGI_FORMAT_R16_TYPELESS:
        depthStencilViewFormat = DXGI_FORMAT_D16_UNORM;
        break;
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        depthStencilViewFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        break;
    default:
        // TODO: Handle other typeless formats
        depthStencilViewFormat = textureFormat;
        break;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = depthStencilViewFormat;
    if (textureDesc.SampleDesc.Count > 1) {
        if (textureDesc.ArraySize > 1) {
            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
            depthStencilViewDesc.Texture2DMSArray.ArraySize = resourceViewDesc.sliceArrayCount;
            depthStencilViewDesc.Texture2DMSArray.FirstArraySlice = resourceViewDesc.firstArraySlice;
        } else {
            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        }
    } else {
        if (textureDesc.ArraySize > 1) {
            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            depthStencilViewDesc.Texture2DArray.ArraySize = resourceViewDesc.sliceArrayCount;
            depthStencilViewDesc.Texture2DArray.FirstArraySlice = resourceViewDesc.firstArraySlice;
            depthStencilViewDesc.Texture2DArray.MipSlice = resourceViewDesc.firstMip;
        } else {
            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            depthStencilViewDesc.Texture2D.MipSlice = resourceViewDesc.firstMip;
        }
    }

    HRESULT result = device->CreateDepthStencilView(texture, &depthStencilViewDesc, &m_DepthStencilView);
    PG_ASSERT(SUCCEEDED(result), "Error at creating depth-stencil view");

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) debugNameLen, debugName);
    }
#endif
}

DX11DepthStencilView::~DX11DepthStencilView() {
    SAFE_RELEASE(m_DepthStencilView);
}

