#include "DX11Texture2D.h"

#include <malloc.h>

static inline UINT GetMiscFlagsFromResourceFlags(uint32_t resourceFlags) {
    return UINT(
        ((resourceFlags & TextureResourceFlags::MISC_TEXTURE_CUBE) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0) |
        ((resourceFlags & TextureResourceFlags::MISC_GENERATE_MIPS) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0)
    );
}

static inline D3D11_BIND_FLAG GetBindFlagsFromResourceFlags(uint32_t resourceFlags) {
    return D3D11_BIND_FLAG(
        ((resourceFlags & TextureResourceFlags::BIND_DEPTH_STENCIL) ? D3D11_BIND_DEPTH_STENCIL : 0) |
        ((resourceFlags & TextureResourceFlags::BIND_RENDER_TARGET) ? D3D11_BIND_RENDER_TARGET : 0) |
        ((resourceFlags & TextureResourceFlags::BIND_SHADER_RESOURCE) ? D3D11_BIND_SHADER_RESOURCE : 0)
    );
}

static inline D3D11_USAGE GetUsageFromResourceFlags(uint32_t resourceFlags) {
    return D3D11_USAGE(
        ((resourceFlags & TextureResourceFlags::USAGE_STAGING)    ? D3D11_USAGE_STAGING :
        ((resourceFlags & TextureResourceFlags::USAGE_DYNAMIC)    ? D3D11_USAGE_DYNAMIC :
        ((resourceFlags & TextureResourceFlags::USAGE_IMMUTABLE)  ? D3D11_USAGE_IMMUTABLE :
                                                                    D3D11_USAGE_DEFAULT)))
    );
}

static inline D3D11_CPU_ACCESS_FLAG GetCPUAccessFlagFromResourceFlags(uint32_t resourceFlags) {
    return D3D11_CPU_ACCESS_FLAG(
        ((resourceFlags & TextureResourceFlags::CPU_ACCESS_READ) ? D3D11_CPU_ACCESS_READ : 0) |
        ((resourceFlags & TextureResourceFlags::CPU_ACCESS_WRITE) ? D3D11_CPU_ACCESS_WRITE : 0)
    );
}

DX11Texture2D::DX11Texture2D(ID3D11Device* device, Texture2DInitParams* initParams) {
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = (UINT) initParams->width;
    textureDesc.Height = (UINT) initParams->height;
    textureDesc.MipLevels = (UINT) initParams->mipCount;
    textureDesc.ArraySize = (UINT) initParams->arraySize;
    textureDesc.Format = initParams->format;
    textureDesc.SampleDesc.Count = (UINT) initParams->sampleCount;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = GetUsageFromResourceFlags(initParams->flags);
    textureDesc.BindFlags = GetBindFlagsFromResourceFlags(initParams->flags);
    textureDesc.CPUAccessFlags = GetCPUAccessFlagFromResourceFlags(initParams->flags);
    textureDesc.MiscFlags = GetMiscFlagsFromResourceFlags(initParams->flags);

    if (initParams->subresources) {
        D3D11_SUBRESOURCE_DATA* subresourceDatas = (D3D11_SUBRESOURCE_DATA*) alloca(sizeof(D3D11_SUBRESOURCE_DATA) * initParams->arraySize * initParams->mipCount);
        TextureSubresourceData* textureSubresource = initParams->subresources;
        for (size_t arrayIndex = 0; arrayIndex < initParams->arraySize; ++arrayIndex) {
            for (size_t mipIndex = 0; mipIndex < initParams->mipCount; ++mipIndex) {
                D3D11_SUBRESOURCE_DATA* subresourceData = subresourceDatas + (arrayIndex * initParams->mipCount + mipIndex);
                subresourceData->pSysMem = textureSubresource->data;
                subresourceData->SysMemPitch = textureSubresource->memPitch;
                subresourceData->SysMemSlicePitch = textureSubresource->memSlicePitch;
                ++textureSubresource;
            }
        }

        HRESULT result = device->CreateTexture2D(&textureDesc, subresourceDatas, &m_Texture);
        PG_ASSERT(SUCCEEDED(result), "Error at creating texture");
    } else {
        HRESULT result = device->CreateTexture2D(&textureDesc, nullptr, &m_Texture);
        PG_ASSERT(SUCCEEDED(result), "Error at creating texture");
    }
    
}

DX11Texture2D::~DX11Texture2D() {
    SAFE_RELEASE(m_Texture);
}

