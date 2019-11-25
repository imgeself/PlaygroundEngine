#pragma once

#include "HWRendererAPI.h"
#include "../PGResource.h"

#include <unordered_map>
#include <string>

class PGTexture : public IResource {
public:
    PGTexture(const std::string& filepath);
    ~PGTexture();

    inline HWTexture2D* GetHWTexture() { return m_HWTexture2D; }
    inline HWShaderResourceView* GetHWResourceView() { return m_HWShaderResourceView; }

private:
    HWTexture2D* m_HWTexture2D = nullptr;
    HWShaderResourceView* m_HWShaderResourceView = nullptr;

};

