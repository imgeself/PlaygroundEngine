#include "PGTexture.h"
#include "PGRenderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Utility/stb_image.h"

PGTexture::PGTexture(const std::string& filepath) {
    int32_t width, height, comp;
    stbi_set_flip_vertically_on_load(true);
    void* imageData = stbi_load(filepath.c_str(), &width, &height, &comp, STBI_rgb_alpha);

    Texture2DInitParams initParams = {};
    initParams.arraySize = 1;
    initParams.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    initParams.width = width;
    initParams.height = height;
    initParams.sampleCount = 1;
    initParams.flags = TextureResourceFlags::BIND_SHADER_RESOURCE;
    initParams.data = imageData;

    HWRendererAPI* rendererAPI = PGRenderer::GetRendererAPI();
    m_HWTexture2D = rendererAPI->CreateTexture2D(&initParams);
    m_HWShaderResourceView = rendererAPI->CreateShaderResourceView(m_HWTexture2D);

    stbi_image_free(imageData);
}

PGTexture::~PGTexture() {
    delete m_HWShaderResourceView;
    delete m_HWTexture2D;
}

