#include "DX11ShaderResourceView.h"

DX11ShaderResourceView::DX11ShaderResourceView(ID3D11Device* device, ID3D11Texture2D* texture) {
    D3D11_TEXTURE2D_DESC textureDesc;
    texture->GetDesc(&textureDesc);

    DXGI_FORMAT textureFormat = textureDesc.Format;
    DXGI_FORMAT shaderResourceViewFormat;
    switch (textureFormat) {
        case DXGI_FORMAT_R32_TYPELESS:
            shaderResourceViewFormat = DXGI_FORMAT_R32_FLOAT;
            break;
        default:
            // TODO: Handle other typeless formats
            shaderResourceViewFormat = textureFormat;
            break;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResouceViewDesc = {};
    shaderResouceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    shaderResouceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResouceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;

    HRESULT result = device->CreateShaderResourceView(texture, &shaderResouceViewDesc, &m_ShaderResourceView);
    PG_ASSERT(SUCCEEDED(result), "Error at creating shader resource view");
}

DX11ShaderResourceView::~DX11ShaderResourceView() {
    SAFE_RELEASE(m_ShaderResourceView);
}

