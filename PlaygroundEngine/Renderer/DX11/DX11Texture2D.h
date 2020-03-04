#pragma once

#include "../HWTexture2D.h"

#include <d3d11.h>

class DX11Texture2D : public HWTexture2D {
public:
    DX11Texture2D(ID3D11Device* device, Texture2DDesc* initParams, SubresourceData* subresources, const char* debugName = 0);
    ~DX11Texture2D();

    Texture2DDesc GetDesc() override;

    inline ID3D11Texture2D* GetDXTexture2D() { return m_Texture; }
    void* GetResourceHandle() override { return (void*) m_Texture; }

private:
    ID3D11Texture2D* m_Texture;
    Texture2DDesc m_Desc;
};

