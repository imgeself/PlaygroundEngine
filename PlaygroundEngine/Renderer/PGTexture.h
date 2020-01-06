#pragma once

#include "../Core.h"
#include "HWRendererAPI.h"
#include "../PGResource.h"

#include <unordered_map>
#include <string>
#include <array>

class PGTexture : public IResource {
public:
    ~PGTexture();

    static PGTexture* CreateTexture2D(const std::string& filepath);
    static PGTexture* CreateTextureFromDDSFile(const std::string& filepath);

    inline HWTexture2D* GetHWTexture() { return m_HWTexture2D; }
    inline HWShaderResourceView* GetHWResourceView() { return m_HWShaderResourceView; }

private:
    PGTexture(Texture2DDesc* initParams, TextureSubresourceData* subresources);

    HWTexture2D* m_HWTexture2D = nullptr;
    HWShaderResourceView* m_HWShaderResourceView = nullptr;

};

