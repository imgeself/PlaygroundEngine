#pragma once

#include "../Core.h"
#include "HWRendererAPI.h"
#include "../PGResource.h"

#include <string>

class PGTexture : public IResource {
public:
    PGTexture(Texture2DDesc* initParams, SubresourceData* subresources, bool generateMips);
    ~PGTexture();

    static PGTexture* CreateTexture2D(const std::string& filepath, bool generateMips);
    static PGTexture* CreateTextureFromDDSFile(const std::string& filepath);

    inline HWTexture2D* GetHWTexture() { return m_HWTexture2D; }
    inline HWShaderResourceView* GetHWResourceView() { return m_HWShaderResourceView; }

private:

    HWTexture2D* m_HWTexture2D = nullptr;
    HWShaderResourceView* m_HWShaderResourceView = nullptr;

};

