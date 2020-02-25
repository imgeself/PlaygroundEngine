#include "PGTexture.h"
#include "PGRenderer.h"

#include "../Utility/tinyddsloader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Utility/stb_image.h"

PGTexture* PGTexture::CreateTexture2D(const std::string& filepath) {
    int32_t width, height, bytesPerPixel;
    stbi_set_flip_vertically_on_load(true);
    void* imageData = stbi_load(filepath.c_str(), &width, &height, &bytesPerPixel, STBI_rgb_alpha);

    // Subresources
    TextureSubresourceData subresource;
    subresource.data = imageData;
    subresource.memPitch = width * 4;
    subresource.memSlicePitch = 0;


    Texture2DDesc initParams = {};
    initParams.arraySize = 1;
    initParams.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    initParams.width = width;
    initParams.height = height;
    initParams.sampleCount = 1;
    initParams.mipCount = 1;
    initParams.flags = HWResourceFlags::BIND_SHADER_RESOURCE | HWResourceFlags::USAGE_IMMUTABLE;

    PGTexture* texture = new PGTexture(&initParams, &subresource);

    stbi_image_free(imageData);
    return texture;
}

PGTexture* PGTexture::CreateTextureFromDDSFile(const std::string& filepath) {
    tinyddsloader::DDSFile ddsFile;
    tinyddsloader::Result result = ddsFile.Load(filepath.c_str());
    PG_ASSERT(result == tinyddsloader::Result::Success, "Error at loading dds file");

    uint32_t arraySize = ddsFile.GetArraySize();
    uint32_t mipCount = ddsFile.GetMipCount();

    // Subresources
    TextureSubresourceData* subresources = (TextureSubresourceData*) alloca(sizeof(TextureSubresourceData) * arraySize * mipCount);
    for (uint32_t arrayIndex = 0; arrayIndex < arraySize; ++arrayIndex) {
        for (uint32_t mipIndex = 0; mipIndex < mipCount; ++mipIndex) {
            const tinyddsloader::DDSFile::ImageData* imageData = ddsFile.GetImageData(mipIndex, arrayIndex);
            TextureSubresourceData* subresource = subresources + (arrayIndex * mipCount + mipIndex);
            subresource->data = imageData->m_mem;
            subresource->memPitch = imageData->m_memPitch;
            subresource->memSlicePitch = imageData->m_memSlicePitch;
        }
    }


    Texture2DDesc initParams = {};
    initParams.arraySize = arraySize;
    initParams.format = (DXGI_FORMAT) ddsFile.GetFormat();
    initParams.width = ddsFile.GetWidth();
    initParams.height = ddsFile.GetHeight();
    initParams.sampleCount = 1;
    initParams.mipCount = mipCount;
    initParams.flags = HWResourceFlags::BIND_SHADER_RESOURCE | HWResourceFlags::USAGE_IMMUTABLE;

    if (ddsFile.IsCubemap()) {
        initParams.flags |= HWResourceFlags::MISC_TEXTURE_CUBE;
    }

    PGTexture* texture = new PGTexture(&initParams, subresources);
    return texture;
}

PGTexture::PGTexture(Texture2DDesc* initParams, TextureSubresourceData* subresources) {
    HWRendererAPI* rendererAPI = PGRenderer::GetRendererAPI();
    m_HWTexture2D = rendererAPI->CreateTexture2D(initParams, subresources);
    m_HWShaderResourceView = rendererAPI->CreateShaderResourceView(m_HWTexture2D);
}


PGTexture::~PGTexture() {
    delete m_HWShaderResourceView;
    delete m_HWTexture2D;
}

