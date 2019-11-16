#include "DX11Texture2D.h"

static inline D3D11_BIND_FLAG GetBindFlagsFromResourceFlags(uint32_t resourceFlags) {
    return D3D11_BIND_FLAG(
        ((resourceFlags & TextureResourceFlags::BIND_DEPTH_STENCIL) ? D3D11_BIND_DEPTH_STENCIL : 0) |
        ((resourceFlags & TextureResourceFlags::BIND_RENDER_TARGET) ? D3D11_BIND_RENDER_TARGET : 0) |
        ((resourceFlags & TextureResourceFlags::BIND_SHADER_RESOURCE) ? D3D11_BIND_SHADER_RESOURCE : 0)
    );
}

static inline D3D11_USAGE GetUsageFromResourceFlags(uint32_t resourceFlags) {
    return D3D11_USAGE(
        ((resourceFlags & TextureResourceFlags::USAGE_CPU_READ)  ? D3D11_USAGE_STAGING :
        ((resourceFlags & TextureResourceFlags::USAGE_CPU_WRITE) ? D3D11_USAGE_DYNAMIC :
                                                                   D3D11_USAGE_DEFAULT))
    );
}

static inline D3D11_CPU_ACCESS_FLAG GetCPUAccessFlagFromResourceFlags(uint32_t resourceFlags) {
    return D3D11_CPU_ACCESS_FLAG(
        ((resourceFlags & TextureResourceFlags::USAGE_CPU_READ) ? D3D11_CPU_ACCESS_READ : 0) |
        ((resourceFlags & TextureResourceFlags::USAGE_CPU_WRITE) ? D3D11_CPU_ACCESS_WRITE : 0)
    );
}

DX11Texture2D::DX11Texture2D(ID3D11Device* device, Texture2DInitParams* initParams) {
    D3D11_TEXTURE2D_DESC shadowMapTextureDesc = {};
    shadowMapTextureDesc.Width = (UINT) initParams->width;
    shadowMapTextureDesc.Height = (UINT) initParams->height;
    shadowMapTextureDesc.MipLevels = 1;
    shadowMapTextureDesc.ArraySize = (UINT) initParams->arraySize;
    shadowMapTextureDesc.Format = initParams->format;
    shadowMapTextureDesc.SampleDesc.Count = (UINT) initParams->sampleCount;
    shadowMapTextureDesc.SampleDesc.Quality = 0;
    shadowMapTextureDesc.Usage = GetUsageFromResourceFlags(initParams->flags);
    shadowMapTextureDesc.BindFlags = GetBindFlagsFromResourceFlags(initParams->flags);
    shadowMapTextureDesc.CPUAccessFlags = GetCPUAccessFlagFromResourceFlags(initParams->flags);
    shadowMapTextureDesc.MiscFlags = 0;

    if (initParams->data) {
        // TODO: Calculate pitch using format and width;
        D3D11_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pSysMem = initParams->data;

        HRESULT result = device->CreateTexture2D(&shadowMapTextureDesc, &subresourceData, &m_Texture);
        PG_ASSERT(SUCCEEDED(result), "Error at creating texture");
    } else {
        HRESULT result = device->CreateTexture2D(&shadowMapTextureDesc, nullptr, &m_Texture);
        PG_ASSERT(SUCCEEDED(result), "Error at creating texture");
    }
    
}

DX11Texture2D::~DX11Texture2D() {
    SAFE_RELEASE(m_Texture);
}

