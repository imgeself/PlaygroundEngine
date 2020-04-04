#pragma once
#include "../Core.h"

#include "HWRendererAPI.h"
#include "PGShaderLib.h"
#include "../Assets/Shaders/ShaderDefinitions.h"

#include <array>

struct PGRendererConfig {
    uint32_t width = 1280;
    uint32_t height = 720;

    float exposure = 1.0f;
    float gamma = 2.2f;

    uint32_t shadowMapSize = 1024;
    uint8_t shadowCascadeCount = 4;

    uint8_t msaaSampleCount = 1;

    bool debugDrawBoundingBoxes = false;
    bool debugDrawCascadeColor = false;
};

struct GPUResource {
    HWResource* resource = nullptr;
    HWShaderResourceView* srv = nullptr;
    HWUnorderedAccessView* uav = nullptr;
    HWDepthStencilView* dsv = nullptr;
    HWRenderTargetView* rtv = nullptr;

    enum CreationFlags : uint8_t {
        CREATE_USING_BIND_FLAGS = 0, // Default

        CREATE_SHADER_RESOURCE_VIEW  = BIT(1),
        CREATE_UNORDERED_ACCESS_VIEW = BIT(2),
        CREATE_RENDER_TARGET_VIEW    = BIT(3),
        CREATE_DEPTH_STENCIL_VIEW    = BIT(4)
    };

    GPUResource(HWRendererAPI* rendererAPI, Texture2DDesc* initParams, SubresourceData* subresources, uint8_t creationFlags, const char* debugName) {
        HWTexture2D* texture = rendererAPI->CreateTexture2D(initParams, subresources, debugName);
        resource = (HWResource*)texture;

        HWResourceViewDesc resourceViewDesc;
        resourceViewDesc.firstArraySlice = 0;
        resourceViewDesc.sliceArrayCount = (uint32_t) initParams->arraySize;
        resourceViewDesc.firstMip = 0;
        resourceViewDesc.mipCount = (uint32_t) initParams->mipCount;
        if (creationFlags == CREATE_USING_BIND_FLAGS) {
            uint32_t flags = initParams->flags;
            if (flags & HWResourceFlags::BIND_SHADER_RESOURCE) {
                srv = rendererAPI->CreateShaderResourceView(texture, resourceViewDesc);
            }
            if (flags & HWResourceFlags::BIND_UNORDERED_ACCESS) {
                uav = rendererAPI->CreateUnorderedAccessView(texture, resourceViewDesc);
            }
            if (flags & HWResourceFlags::BIND_RENDER_TARGET) {
                rtv = rendererAPI->CreateRenderTargetView(texture, resourceViewDesc);
            }
            if (flags & HWResourceFlags::BIND_DEPTH_STENCIL) {
                dsv = rendererAPI->CreateDepthStencilView(texture, resourceViewDesc);
            }
        } else {
            if (creationFlags & CreationFlags::CREATE_SHADER_RESOURCE_VIEW) {
                srv = rendererAPI->CreateShaderResourceView(texture, resourceViewDesc);
            }
            if (creationFlags & CreationFlags::CREATE_UNORDERED_ACCESS_VIEW) {
                uav = rendererAPI->CreateUnorderedAccessView(texture, resourceViewDesc);
            }
            if (creationFlags & CreationFlags::CREATE_RENDER_TARGET_VIEW) {
                rtv = rendererAPI->CreateRenderTargetView(texture, resourceViewDesc);
            }
            if (creationFlags & CreationFlags::CREATE_DEPTH_STENCIL_VIEW) {
                dsv = rendererAPI->CreateDepthStencilView(texture, resourceViewDesc);
            }
        }
    }

    ~GPUResource() {
        SAFE_DELETE(srv);
        SAFE_DELETE(uav);
        SAFE_DELETE(rtv);
        SAFE_DELETE(dsv);
        SAFE_DELETE(resource);
    }
};


enum InputLayoutType : uint8_t {
    POS,
    POS_TC,
    POS_NOR_TC,
    POS_NOR_TC_TANGENT,

    INPUT_TYPE_COUNT
};

struct PGPipelineDesc {
    PGShader* shader;
    uint32_t shaderFlags;
    uint8_t doubleSided;
    InputLayoutType layoutType;

    bool blendEnable;

    uint8_t writeDepth;
    HWComparionsFunc depthFunction;
};

// TODO: Pipeline state objects is going to be part of HWRendererAPI when we redesign the RHI. 
struct PGCachedPipelineState {
    HWGraphicsPipelineState* graphicsPipelineState = nullptr;
    HWGraphicsPipelineStateDesc graphicsPipelineDesc;
    uint32_t shaderFlags = 0;
    size_t hash = 0;
};

const size_t RENDERER_DEFAULT_SAMPLER_SIZE = 6;
const size_t RENDERER_DEFAULT_INPUT_LAYOUT_SIZE = InputLayoutType::INPUT_TYPE_COUNT;

const size_t MAX_CACHED_PIPELINE_STATE_PER_STAGE = 4;

enum SceneRenderPassType {
    DEPTH_PASS, // Used for depth prepass, shadow mapping, etc.
    FORWARD,

    SCENE_PASS_TYPE_COUNT
};

struct PGRendererResources {
    static HWBuffer* s_PerFrameGlobalConstantBuffer;
    static HWBuffer* s_PerMaterialGlobalConstantBuffer;
    static HWBuffer* s_PerViewGlobalConstantBuffer;
    static HWBuffer* s_PerDrawGlobalConstantBuffer;
    static HWBuffer* s_PostProcessConstantBuffer;
    static HWBuffer* s_RendererVarsConstantBuffer;

    static std::array<HWInputLayoutDesc, InputLayoutType::INPUT_TYPE_COUNT> s_DefaultInputLayoutDescs;
    static std::array<HWSamplerState*, RENDERER_DEFAULT_SAMPLER_SIZE> s_DefaultSamplers;

    static PGCachedPipelineState s_CachedPipelineStates[SCENE_PASS_TYPE_COUNT][MAX_CACHED_PIPELINE_STATE_PER_STAGE];

    static uint8_t CreateCachedPipelineState(HWRendererAPI* rendererAPI, SceneRenderPassType scenePassType, const PGPipelineDesc& pipelineDesc);
    static void UpdateShaders(HWRendererAPI* rendererAPI);

    static GPUResource* s_HDRRenderTarget;
    static GPUResource* s_DepthStencilTarget;
    static GPUResource* s_ResolvedHDRRenderTarget; // if msaa enabled
    static GPUResource* s_ShadowMapCascadesTexture;

    static void CreateDefaultBuffers(HWRendererAPI* rendererAPI, const PGRendererConfig& rendererConfig);
    static void CreateDefaultSamplerStates(HWRendererAPI* rendererAPI);
    static void CreateDefaultInputLayout(HWRendererAPI* rendererAPI);
    static void CreateSizeDependentResources(HWRendererAPI* rendererAPI, const PGRendererConfig& rendererConfig);
    static void CreateShadowMapResources(HWRendererAPI* rendererAPI, const PGRendererConfig& rendererConfig);

    static void ClearResources();

};

