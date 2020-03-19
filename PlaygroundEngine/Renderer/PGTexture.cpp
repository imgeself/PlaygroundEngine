#include "PGTexture.h"
#include "PGRenderer.h"

#include "../Utility/tinyddsloader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Utility/stb_image.h"

PGTexture* PGTexture::CreateTexture2D(const std::string& filepath, bool generateMips) {
    int32_t width, height, bytesPerPixel;
    void* imageData = stbi_load(filepath.c_str(), &width, &height, &bytesPerPixel, STBI_rgb_alpha);

    // Subresources
    SubresourceData subresource;
    subresource.data = imageData;
    subresource.memPitch = width * 4;
    subresource.memSlicePitch = 0;


    Texture2DDesc initParams = {};
    initParams.arraySize = 1;
    initParams.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    initParams.width = width;
    initParams.height = height;
    initParams.sampleCount = 1;
    initParams.mipCount = generateMips ? 0 : 1;
    initParams.flags = HWResourceFlags::BIND_SHADER_RESOURCE;
    if (generateMips) {
        initParams.flags |= (HWResourceFlags::MISC_GENERATE_MIPS | HWResourceFlags::BIND_RENDER_TARGET);
    } else {
        initParams.flags |= HWResourceFlags::USAGE_IMMUTABLE;
    }

    PGTexture* texture = new PGTexture(&initParams, &subresource, generateMips);

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
    SubresourceData* subresources = (SubresourceData*) alloca(sizeof(SubresourceData) * arraySize * mipCount);
    for (uint32_t arrayIndex = 0; arrayIndex < arraySize; ++arrayIndex) {
        for (uint32_t mipIndex = 0; mipIndex < mipCount; ++mipIndex) {
            const tinyddsloader::DDSFile::ImageData* imageData = ddsFile.GetImageData(mipIndex, arrayIndex);
            SubresourceData* subresource = subresources + (arrayIndex * mipCount + mipIndex);
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

    PGTexture* texture = new PGTexture(&initParams, subresources, false);
    return texture;
}

PGTexture::PGTexture(Texture2DDesc* initParams, SubresourceData* subresources, bool generateMips) {
    HWRendererAPI* rendererAPI = PGRenderer::GetRendererAPI();
    if (generateMips) {
        m_HWTexture2D = rendererAPI->CreateTexture2D(initParams, nullptr);
        m_HWShaderResourceView = rendererAPI->CreateShaderResourceView(m_HWTexture2D);

        rendererAPI->UpdateSubresource(m_HWTexture2D, 0, nullptr, subresources[0]);
        rendererAPI->GenerateMips(m_HWShaderResourceView);
    } else {
        m_HWTexture2D = rendererAPI->CreateTexture2D(initParams, subresources);
        m_HWShaderResourceView = rendererAPI->CreateShaderResourceView(m_HWTexture2D);
    }
}


PGTexture::~PGTexture() {
    delete m_HWShaderResourceView;
    delete m_HWTexture2D;
}

