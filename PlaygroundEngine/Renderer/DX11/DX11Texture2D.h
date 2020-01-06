#pragma once

#include "../HWTexture2D.h"

#include <d3d11.h>

class DX11Texture2D : public HWTexture2D {
public:
    DX11Texture2D(ID3D11Device* device, Texture2DDesc* initParams, TextureSubresourceData* subresources);
    ~DX11Texture2D();

    Texture2DDesc GetDesc() override;

    inline ID3D11Texture2D* GetDXTexture2D() { return m_Texture; }

private:
    ID3D11Texture2D* m_Texture;
    Texture2DDesc m_Desc;
};

