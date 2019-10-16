#include "DX11RenderTargets.h"

DX11RenderTargetView::DX11RenderTargetView(ID3D11Device* device, ID3D11Texture2D* texture) {
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
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    HRESULT result = device->CreateRenderTargetView(texture, &renderTargetViewDesc, &m_RenderTargetView);
    PG_ASSERT(SUCCEEDED(result), "Error at creating render target view");
}

DX11RenderTargetView::~DX11RenderTargetView() {
    SAFE_RELEASE(m_RenderTargetView);
}

DX11DepthStencilView::DX11DepthStencilView(ID3D11Device* device, ID3D11Texture2D* texture) {
    D3D11_TEXTURE2D_DESC textureDesc;
    texture->GetDesc(&textureDesc);

    DXGI_FORMAT textureFormat = textureDesc.Format;
    DXGI_FORMAT depthStencilViewFormat;
    switch (textureFormat) {
    case DXGI_FORMAT_R32_TYPELESS:
        depthStencilViewFormat = DXGI_FORMAT_D32_FLOAT;
        break;
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        depthStencilViewFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        break;
    default:
        // TODO: Handle other typeless formats
        depthStencilViewFormat = textureFormat;
        break;
    }

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = depthStencilViewFormat;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    HRESULT result = device->CreateDepthStencilView(texture, &depthStencilViewDesc, &m_DepthStencilView);
    PG_ASSERT(SUCCEEDED(result), "Error at creating depth-stencil view");
}

DX11DepthStencilView::~DX11DepthStencilView() {
    SAFE_RELEASE(m_DepthStencilView);
}

