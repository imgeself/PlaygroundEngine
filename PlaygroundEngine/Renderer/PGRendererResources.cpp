#include "PGRendererResources.h"

HWBuffer* PGRendererResources::s_PerFrameGlobalConstantBuffer;
HWBuffer* PGRendererResources::s_PerDrawGlobalConstantBuffer;
HWBuffer* PGRendererResources::s_PostProcessConstantBuffer;
HWBuffer* PGRendererResources::s_RendererVarsConstantBuffer;

std::array<HWSamplerState*, RENDERER_DEFAULT_SAMPLER_SIZE> PGRendererResources::s_DefaultSamplers;
std::array<HWVertexInputLayout*, RENDERER_DEFAULT_INPUT_LAYOUT_SIZE> PGRendererResources::s_DefaultInputLayouts;

PGCachedPipelineState* PGRendererResources::s_CachedPipelineStates[SCENE_PASS_TYPE_COUNT][MAX_CACHED_PIPELINE_STATE_PER_STAGE] = {0};

GPUResource* PGRendererResources::s_HDRRenderTarget;
GPUResource* PGRendererResources::s_DepthStencilTarget;
GPUResource* PGRendererResources::s_ResolvedHDRRenderTarget; // if msaa enabled
GPUResource* PGRendererResources::s_ShadowMapCascadesTexture;

uint8_t PGRendererResources::CreatePipelineState(HWRendererAPI* rendererAPI, SceneRenderPassType scenePassType, const PGPipelineDesc& pipelineDesc) {
    size_t checksumPSO = Hash((const uint8_t*)&pipelineDesc, sizeof(PGPipelineDesc));
    for (uint8_t i = 0; i < MAX_CACHED_PIPELINE_STATE_PER_STAGE; i++) {
        PGCachedPipelineState* ps = s_CachedPipelineStates[scenePassType][i];
        if (ps) {
            if (ps->hash == checksumPSO) {
                return i;
            }
        } else {
            PGCachedPipelineState* ps = new PGCachedPipelineState;
            ps->hash = checksumPSO;

            HWBlendDesc blendDesc;
            HWBlendState* blendState = rendererAPI->CreateBlendState(blendDesc);
            ps->blendState = blendState;

            HWRasterizerDesc rasterizerDesc;
            rasterizerDesc.multisampleEnable = true;
            if (pipelineDesc.doubleSided) {
                rasterizerDesc.cullMode = CULL_NONE;
            } else {
                rasterizerDesc.cullMode = CULL_BACK;
            }

            HWRasterizerState* rasterizerState = rendererAPI->CreateRasterizerState(rasterizerDesc);
            ps->rasterizerState = rasterizerState;

            HWVertexInputLayout* inputLayout = s_DefaultInputLayouts[pipelineDesc.layoutType];
            ps->inputLayout = inputLayout;

            ps->shader = pipelineDesc.shader;

            s_CachedPipelineStates[scenePassType][i] = ps;
            return i;
        }
    }

    PG_ASSERT(false, "Cached pipeline array is full!");
    return 0xFF;
}

void PGRendererResources::CreateDefaultBuffers(HWRendererAPI* rendererAPI, const PGRendererConfig& rendererConfig) {
    // Default resources init
    uint32_t constantBufferFlags = HWResourceFlags::USAGE_DYNAMIC | HWResourceFlags::CPU_ACCESS_WRITE | HWResourceFlags::BIND_CONSTANT_BUFFER;
    s_PerFrameGlobalConstantBuffer = rendererAPI->CreateBuffer(nullptr, sizeof(PerFrameGlobalConstantBuffer), constantBufferFlags);
    s_PerDrawGlobalConstantBuffer = rendererAPI->CreateBuffer(nullptr, sizeof(PerDrawGlobalConstantBuffer), constantBufferFlags);

    PostProcessConstantBuffer postProcessConstantBuffer = {};
    postProcessConstantBuffer.g_PPExposure = rendererConfig.exposure;
    postProcessConstantBuffer.g_PPGamma = rendererConfig.gamma;

    SubresourceData constantSubresourceData = {};
    constantSubresourceData.data = &postProcessConstantBuffer;
    s_PostProcessConstantBuffer = rendererAPI->CreateBuffer(&constantSubresourceData, sizeof(PostProcessConstantBuffer), constantBufferFlags);

    RendererVariablesConstantBuffer rendererVariablesConstantBuffer = {};
    rendererVariablesConstantBuffer.g_ShadowCascadeCount = rendererConfig.shadowCascadeCount;
    rendererVariablesConstantBuffer.g_ShadowMapSize = rendererConfig.shadowMapSize;

    constantSubresourceData.data = &rendererVariablesConstantBuffer;
    s_RendererVarsConstantBuffer = rendererAPI->CreateBuffer(&constantSubresourceData, sizeof(RendererVariablesConstantBuffer), constantBufferFlags);
}

void PGRendererResources::CreateDefaultSamplerStates(HWRendererAPI* rendererAPI) {
    // Default sampler states
    SamplerStateInitParams shadowSamplerStateInitParams = {};
    shadowSamplerStateInitParams.filterMode = TextureFilterMode_MIN_MAG_LINEAR_MIP_POINT;
    shadowSamplerStateInitParams.addressModeU = TextureAddressMode_CLAMP;
    shadowSamplerStateInitParams.addressModeV = TextureAddressMode_CLAMP;
    shadowSamplerStateInitParams.addressModeW = TextureAddressMode_CLAMP;
    shadowSamplerStateInitParams.comparisonFunction = ComparisonFunction::LESS;
    s_DefaultSamplers[SHADOW_SAMPLER_COMPARISON_STATE_SLOT] = rendererAPI->CreateSamplerState(&shadowSamplerStateInitParams);

    SamplerStateInitParams samplerStateInitParams = {};
    samplerStateInitParams.filterMode = TextureFilterMode_MIN_MAG_MIP_POINT;
    samplerStateInitParams.addressModeU = TextureAddressMode_CLAMP;
    samplerStateInitParams.addressModeV = TextureAddressMode_CLAMP;
    samplerStateInitParams.addressModeW = TextureAddressMode_CLAMP;
    s_DefaultSamplers[POINT_CLAMP_SAMPLER_STATE_SLOT] = rendererAPI->CreateSamplerState(&samplerStateInitParams);

    samplerStateInitParams.filterMode = TextureFilterMode_MIN_MAG_MIP_LINEAR;
    s_DefaultSamplers[LINEAR_CLAMP_SAMPLER_STATE_SLOT] = rendererAPI->CreateSamplerState(&samplerStateInitParams);

    samplerStateInitParams.addressModeU = TextureAddressMode_WRAP;
    samplerStateInitParams.addressModeV = TextureAddressMode_WRAP;
    samplerStateInitParams.addressModeW = TextureAddressMode_WRAP;
    s_DefaultSamplers[LINEAR_WRAP_SAMPLER_STATE_SLOT] = rendererAPI->CreateSamplerState(&samplerStateInitParams);

    samplerStateInitParams.filterMode = TextureFilterMode_MIN_MAG_MIP_POINT;
    s_DefaultSamplers[POINT_WRAP_SAMPLER_STATE_SLOT] = rendererAPI->CreateSamplerState(&samplerStateInitParams);
}

void PGRendererResources::CreateDefaultInputLayout(HWRendererAPI* rendererAPI, PGShaderLib* shaderLib) {
    std::vector<VertexInputElement> posInputElements = {
        { "POSITION", 0, VertexDataFormat_FLOAT3, 0, PER_VERTEX_DATA, 0 }
    };

    PGShader* shadowGenShader = shaderLib->GetDefaultShader("ShadowGen");
    s_DefaultInputLayouts[InputLayoutType::POS] = rendererAPI->CreateVertexInputLayout(posInputElements, shadowGenShader->GetHWVertexShader());

    std::vector<VertexInputElement> posTCInputElements = {
        { "POSITION", 0, VertexDataFormat_FLOAT3, 0, PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, VertexDataFormat_FLOAT2, 2, PER_VERTEX_DATA, 0 }
    };

    PGShader* shadowGenAlphaShader = shaderLib->GetDefaultShader("ShadowGenAlphaTest");
    s_DefaultInputLayouts[InputLayoutType::POS_TC] = rendererAPI->CreateVertexInputLayout(posTCInputElements, shadowGenAlphaShader->GetHWVertexShader());

    std::vector<VertexInputElement> inputElements = {
        { "POSITION", 0, VertexDataFormat_FLOAT3, 0, PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, VertexDataFormat_FLOAT3, 1, PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, VertexDataFormat_FLOAT2, 2, PER_VERTEX_DATA, 0 }
    };

    PGShader* pbrForwardShader = shaderLib->GetDefaultShader("PBRForward");
    s_DefaultInputLayouts[InputLayoutType::POS_NOR_TC] = rendererAPI->CreateVertexInputLayout(inputElements, pbrForwardShader->GetHWVertexShader());
}

void PGRendererResources::CreateSizeDependentResources(HWRendererAPI* rendererAPI, const PGRendererConfig& rendererConfig) {
    SAFE_DELETE(s_HDRRenderTarget);
    SAFE_DELETE(s_DepthStencilTarget);
    SAFE_DELETE(s_ResolvedHDRRenderTarget);

    // HDR mainbuffer
    Texture2DDesc hdrBufferInitParams = {};
    hdrBufferInitParams.arraySize = 1;
    hdrBufferInitParams.format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    hdrBufferInitParams.width = rendererConfig.width;
    hdrBufferInitParams.height = rendererConfig.height;
    hdrBufferInitParams.sampleCount = rendererConfig.msaaSampleCount;
    hdrBufferInitParams.mipCount = 1;
    hdrBufferInitParams.flags = HWResourceFlags::BIND_SHADER_RESOURCE | HWResourceFlags::BIND_RENDER_TARGET;
    s_HDRRenderTarget = new GPUResource(rendererAPI, &hdrBufferInitParams, nullptr, "MainHDRTexture");

    Texture2DDesc depthTextureInitParams = {};
    depthTextureInitParams.arraySize = 1;
    depthTextureInitParams.format = DXGI_FORMAT_D32_FLOAT;
    depthTextureInitParams.width = hdrBufferInitParams.width;
    depthTextureInitParams.height = hdrBufferInitParams.height;
    depthTextureInitParams.sampleCount = hdrBufferInitParams.sampleCount;
    depthTextureInitParams.mipCount = 1;
    depthTextureInitParams.flags = HWResourceFlags::BIND_DEPTH_STENCIL;
    s_DepthStencilTarget = new GPUResource(rendererAPI, &depthTextureInitParams, nullptr, "MainDepthStencilTexture");

    if (rendererConfig.msaaSampleCount > 1) {
        Texture2DDesc resolvedBufferInitParams = {};
        resolvedBufferInitParams.arraySize = 1;
        resolvedBufferInitParams.format = hdrBufferInitParams.format;
        resolvedBufferInitParams.width = hdrBufferInitParams.width;
        resolvedBufferInitParams.height = hdrBufferInitParams.height;
        resolvedBufferInitParams.sampleCount = 1;
        resolvedBufferInitParams.mipCount = 1;
        resolvedBufferInitParams.flags = HWResourceFlags::BIND_SHADER_RESOURCE | HWResourceFlags::BIND_RENDER_TARGET;
        s_ResolvedHDRRenderTarget = new GPUResource(rendererAPI, &resolvedBufferInitParams, nullptr, "MainResolvedHDRTexture");
    }
}

void PGRendererResources::CreateShadowMapResources(HWRendererAPI* rendererAPI, const PGRendererConfig& rendererConfig) {
    SAFE_DELETE(s_ShadowMapCascadesTexture);

    Texture2DDesc initParams = {};
    initParams.width = rendererConfig.shadowMapSize;
    initParams.height = rendererConfig.shadowMapSize;
    initParams.format = DXGI_FORMAT_R16_TYPELESS;
    initParams.sampleCount = 1;
    initParams.mipCount = 1;
    initParams.arraySize = rendererConfig.shadowCascadeCount;
    initParams.flags = HWResourceFlags::BIND_DEPTH_STENCIL | HWResourceFlags::BIND_SHADER_RESOURCE;
    s_ShadowMapCascadesTexture = new GPUResource(rendererAPI, &initParams, nullptr, "ShadowMap");
}

void PGRendererResources::ClearResources() {
    delete s_PerFrameGlobalConstantBuffer;
    delete s_PerDrawGlobalConstantBuffer;
    delete s_RendererVarsConstantBuffer;
    delete s_PostProcessConstantBuffer;

    SAFE_DELETE(s_HDRRenderTarget);
    SAFE_DELETE(s_DepthStencilTarget);
    SAFE_DELETE(s_ResolvedHDRRenderTarget);
    SAFE_DELETE(s_ShadowMapCascadesTexture);

    for (HWSamplerState* samplerState : s_DefaultSamplers) {
        if (samplerState) {
            delete samplerState;
        }
    }

    for (HWVertexInputLayout* inputLayout : s_DefaultInputLayouts) {
        if (inputLayout) {
            delete inputLayout;
        }
    }
}


