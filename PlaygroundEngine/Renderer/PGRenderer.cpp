#include "PGRenderer.h"
#include "Skybox.h"
#include "../PGProfiler.h"

#include "DX11/DX11RendererAPI.h"

#include <memory>

HWRendererAPI* PGRenderer::s_RendererAPI = nullptr;
PGShaderLib* PGRenderer::s_ShaderLib = nullptr;
std::vector<PGRenderObject*> PGRenderer::s_RenderObjects = std::vector<PGRenderObject*>();
PGScene* PGRenderer::s_ActiveSceneData = nullptr;

HWConstantBuffer* PGRenderer::s_PerFrameGlobalConstantBuffer = nullptr;
HWConstantBuffer* PGRenderer::s_PerDrawGlobalConstantBuffer = nullptr;
HWConstantBuffer* PGRenderer::s_RendererVarsConstantBuffer = nullptr;
HWConstantBuffer* PGRenderer::s_PostProcessConstantBuffer = nullptr;

std::array<HWSamplerState*, 5> PGRenderer::s_DefaultSamplers = std::array<HWSamplerState*, 5>();

ShadowGenStage PGRenderer::s_ShadowGenStage = ShadowGenStage();
SceneRenderPass PGRenderer::s_SceneRenderPass = SceneRenderPass();
FullscreenPass PGRenderer::s_ToneMappingPass = FullscreenPass();

const size_t SHADOW_MAP_SIZE = 1024;

struct GPUResource {
    HWResource* resource = nullptr;
    HWShaderResourceView* srv = nullptr;
    HWDepthStencilView* dsv = nullptr;
    HWRenderTargetView* rtv = nullptr;

    GPUResource(HWRendererAPI* rendererAPI, Texture2DDesc* initParams, TextureSubresourceData* subresources, const char* debugName) {
        HWTexture2D* texture = rendererAPI->CreateTexture2D(initParams, subresources, debugName);
        resource = (HWResource*) texture;

        uint32_t flags = initParams->flags;
        if (flags & TextureResourceFlags::BIND_SHADER_RESOURCE) {
            srv = rendererAPI->CreateShaderResourceView(texture);
        }
        if (flags & TextureResourceFlags::BIND_RENDER_TARGET) {
            rtv = rendererAPI->CreateRenderTargetView(texture, 0, initParams->arraySize, 0, initParams->mipCount);
        }
        if (flags & TextureResourceFlags::BIND_DEPTH_STENCIL) {
            dsv = rendererAPI->CreateDepthStencilView(texture, 0, initParams->arraySize, 0, initParams->mipCount);
        }
    }

    ~GPUResource() {
        SAFE_DELETE(srv);
        SAFE_DELETE(rtv);
        SAFE_DELETE(dsv);
        SAFE_DELETE(resource);
    }
};

// Renderer resources
GPUResource* s_HDRRenderTarget = nullptr;
GPUResource* s_DepthStencilTarget = nullptr;
GPUResource* s_ResolvedHDRRenderTarget = nullptr; // if msaa enabled
GPUResource* s_ShadowMapCascadesTexture = nullptr;

// Renderer variables
uint32_t s_MSAASampleCount = 4; // 1 if MSAA disabled

void PGRenderer::Uninitialize() {
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

    for (PGRenderObject* renderObject : s_RenderObjects) {
        delete renderObject;
    }

    delete s_ShaderLib;
    delete s_RendererAPI;
}

void CalculateCascadeProjMatrices(PGCamera* camera, Matrix4 lightView, size_t shadowMapSize,
                                  Matrix4* outMatricesData) {
    PG_PROFILE_FUNCTION();
    Matrix4 inverseProjectionViewMatrix = camera->GetInverseProjectionViewMatrix();

    // Get frustum points in world space
    Vector4 frustumPoints[] = {
        (inverseProjectionViewMatrix * Vector4(-1, -1, 0, 1)), // left-bottom-near
        (inverseProjectionViewMatrix * Vector4(-1, 1, 0, 1)),  // left-top-near
        (inverseProjectionViewMatrix * Vector4(1, -1, 0, 1)),  // right-bottom-near
        (inverseProjectionViewMatrix * Vector4(1, 1, 0, 1)),   // right-top-near
        (inverseProjectionViewMatrix * Vector4(-1, -1, 1, 1)), // left-bottom-far
        (inverseProjectionViewMatrix * Vector4(-1, 1, 1, 1)),  // left-top-far
        (inverseProjectionViewMatrix * Vector4(1, -1, 1, 1)),  // right-bottom-far
        (inverseProjectionViewMatrix * Vector4(1, 1, 1, 1)),   // right-top-far
    };

    Vector4 unprojectedPoints[] = {
        (frustumPoints[0] / frustumPoints[0].w),
        (frustumPoints[1] / frustumPoints[1].w),
        (frustumPoints[2] / frustumPoints[2].w),
        (frustumPoints[3] / frustumPoints[3].w),
        (frustumPoints[4] / frustumPoints[4].w),
        (frustumPoints[5] / frustumPoints[5].w),
        (frustumPoints[6] / frustumPoints[6].w),
        (frustumPoints[7] / frustumPoints[7].w)
    };

    float splits[CASCADE_COUNT + 1] = {
        0.0f,
        0.05f,
        0.1f,
        0.25f,
        1.0f
    };

    Matrix4 cascadeProjMatrices[CASCADE_COUNT];
    for (uint32_t cascadeIndex = 0; cascadeIndex < CASCADE_COUNT; ++cascadeIndex) {
        float nearSplit = splits[cascadeIndex];
        float farSplit = splits[cascadeIndex + 1];

        Vector4 points[] = {
            (lightView * Lerp(unprojectedPoints[0], nearSplit, unprojectedPoints[4])), // left-bottom-near
            (lightView * Lerp(unprojectedPoints[1], nearSplit, unprojectedPoints[5])), // left-top-near
            (lightView * Lerp(unprojectedPoints[2], nearSplit, unprojectedPoints[6])), // right-bottom-near
            (lightView * Lerp(unprojectedPoints[3], nearSplit, unprojectedPoints[7])), // right-top-near
            (lightView * Lerp(unprojectedPoints[0], farSplit, unprojectedPoints[4])), // left-bottom-far
            (lightView * Lerp(unprojectedPoints[1], farSplit, unprojectedPoints[5])), // left-top-far
            (lightView * Lerp(unprojectedPoints[2], farSplit, unprojectedPoints[6])), // right-bottom-far
            (lightView * Lerp(unprojectedPoints[3], farSplit, unprojectedPoints[7])), // right-top-far
        };

        Vector3 center(0.0f, 0.0f, 0.0f);
        for (size_t i = 0; i < ARRAYSIZE(points); i++) {
            center += points[i].xyz();
        }
        center = center / ARRAYSIZE(points);

        float radius = 0;
        for (size_t i = 0; i < ARRAYSIZE(points); i++) {
            radius = max(radius, Lenght(points[i].xyz() - center));
        }

        // We make cascade projection matrix as a cube so that we can snap cascade to texel size
        Vector3 minCorner = center + Vector3(-radius, -radius, -radius);
        Vector3 maxCorner = center + Vector3(radius, radius, radius);

        // Extrude min/max z values for avoid shadow camera being too close to scene and causing clipping
        // TODO: Instead of this method we need to clamp max/min z values to scene's bounding box's top and bottom lines
        minCorner.z = min(minCorner.z, -50.0f);
        maxCorner.z = min(maxCorner.z, 50.0f);

        float texelSize = 1.0f / shadowMapSize;
        Vector3 extend = maxCorner - minCorner;
        Vector3 worldUnitsForTexelSize = extend * texelSize;
        minCorner = Floor(minCorner / worldUnitsForTexelSize) * worldUnitsForTexelSize;
        maxCorner = Floor(maxCorner / worldUnitsForTexelSize) * worldUnitsForTexelSize;


        cascadeProjMatrices[cascadeIndex] = OrthoMatrixOffCenterLH(minCorner.x, maxCorner.x, maxCorner.y, minCorner.y, minCorner.z, maxCorner.z);
    }

    memcpy(outMatricesData, cascadeProjMatrices, ARRAYSIZE(cascadeProjMatrices) * sizeof(Matrix4));

}

void PGRenderer::CreateDefaultSamplerStates() {
    // Default sampler states
    SamplerStateInitParams shadowSamplerStateInitParams = {};
    shadowSamplerStateInitParams.filterMode = TextureFilterMode_MIN_MAG_LINEAR_MIP_POINT;
    shadowSamplerStateInitParams.addressModeU = TextureAddressMode_CLAMP;
    shadowSamplerStateInitParams.addressModeV = TextureAddressMode_CLAMP;
    shadowSamplerStateInitParams.addressModeW = TextureAddressMode_CLAMP;
    shadowSamplerStateInitParams.comparisonFunction = ComparisonFunction::LESS;
    s_DefaultSamplers[SHADOW_SAMPLER_COMPARISON_STATE_SLOT] = s_RendererAPI->CreateSamplerState(&shadowSamplerStateInitParams);

    SamplerStateInitParams samplerStateInitParams = {};
    samplerStateInitParams.filterMode = TextureFilterMode_MIN_MAG_MIP_POINT;
    samplerStateInitParams.addressModeU = TextureAddressMode_CLAMP;
    samplerStateInitParams.addressModeV = TextureAddressMode_CLAMP;
    samplerStateInitParams.addressModeW = TextureAddressMode_CLAMP;
    s_DefaultSamplers[POINT_CLAMP_SAMPLER_STATE_SLOT] = s_RendererAPI->CreateSamplerState(&samplerStateInitParams);

    samplerStateInitParams.filterMode = TextureFilterMode_MIN_MAG_MIP_LINEAR;
    s_DefaultSamplers[LINEAR_CLAMP_SAMPLER_STATE_SLOT] = s_RendererAPI->CreateSamplerState(&samplerStateInitParams);

    samplerStateInitParams.addressModeU = TextureAddressMode_WRAP;
    samplerStateInitParams.addressModeV = TextureAddressMode_WRAP;
    samplerStateInitParams.addressModeW = TextureAddressMode_WRAP;
    s_DefaultSamplers[LINEAR_WRAP_SAMPLER_STATE_SLOT] = s_RendererAPI->CreateSamplerState(&samplerStateInitParams);

    samplerStateInitParams.filterMode = TextureFilterMode_MIN_MAG_MIP_POINT;
    s_DefaultSamplers[POINT_WRAP_SAMPLER_STATE_SLOT] = s_RendererAPI->CreateSamplerState(&samplerStateInitParams);
}

bool PGRenderer::Initialize(PGWindow* window) {
    s_RendererAPI = new DX11RendererAPI(window);
    s_ShaderLib = new PGShaderLib(s_RendererAPI);
    s_ShaderLib->LoadDefaultShaders();

    // Default resources init
    PerFrameGlobalConstantBuffer perFrameGlobalConstantBuffer = {};
    s_PerFrameGlobalConstantBuffer = s_RendererAPI->CreateConstantBuffer(&perFrameGlobalConstantBuffer, sizeof(PerFrameGlobalConstantBuffer));

    PerDrawGlobalConstantBuffer perDrawGlobalConstantBuffer = {};
    s_PerDrawGlobalConstantBuffer = s_RendererAPI->CreateConstantBuffer(&perDrawGlobalConstantBuffer, sizeof(PerDrawGlobalConstantBuffer));

    PostProcessConstantBuffer postProcessConstantBuffer = {};
    postProcessConstantBuffer.g_PPExposure = 1.0f;
    postProcessConstantBuffer.g_PPGamma = 2.2f;
    s_PostProcessConstantBuffer = s_RendererAPI->CreateConstantBuffer(&postProcessConstantBuffer, sizeof(PostProcessConstantBuffer));

    RendererVariablesConstantBuffer rendererVariablesConstantBuffer = {};
    rendererVariablesConstantBuffer.g_ShadowCascadeCount = CASCADE_COUNT;
    rendererVariablesConstantBuffer.g_ShadowMapSize = SHADOW_MAP_SIZE;
    s_RendererVarsConstantBuffer = s_RendererAPI->CreateConstantBuffer(&rendererVariablesConstantBuffer, sizeof(RendererVariablesConstantBuffer));

    HWConstantBuffer* constantBuffers[8] = {0};
    constantBuffers[PER_DRAW_CBUFFER_SLOT] = s_PerDrawGlobalConstantBuffer;
    constantBuffers[PER_FRAME_CBUFFER_SLOT] = s_PerFrameGlobalConstantBuffer;
    constantBuffers[RENDERER_VARIABLES_CBUFFER_SLOT] = s_RendererVarsConstantBuffer;
    constantBuffers[POST_PROCESS_CBUFFER_SLOT] = s_PostProcessConstantBuffer;
    s_RendererAPI->SetConstanBuffersVS(0, constantBuffers, ARRAYSIZE(constantBuffers));
    s_RendererAPI->SetConstanBuffersPS(0, constantBuffers, ARRAYSIZE(constantBuffers));

    CreateDefaultSamplerStates();
    // Bind default sampler states
    s_RendererAPI->SetSamplerStatesPS(0, s_DefaultSamplers.data(), s_DefaultSamplers.max_size());

    // init shadow mapping
    Texture2DDesc initParams = {};
    initParams.width = SHADOW_MAP_SIZE;
    initParams.height = SHADOW_MAP_SIZE;
    initParams.format = DXGI_FORMAT_R16_TYPELESS;
    initParams.sampleCount = 1;
    initParams.mipCount = 1;
    initParams.arraySize = CASCADE_COUNT;
    initParams.flags = TextureResourceFlags::BIND_DEPTH_STENCIL | TextureResourceFlags::BIND_SHADER_RESOURCE;
    s_ShadowMapCascadesTexture = new GPUResource(s_RendererAPI, &initParams, nullptr, "ShadowMap");

    s_ShadowGenStage.Initialize(s_RendererAPI, s_ShaderLib, SHADOW_MAP_SIZE);
    s_ShadowGenStage.SetShadowMapDSV(s_ShadowMapCascadesTexture->dsv);
    s_SceneRenderPass.SetShaderResource(SHADOW_MAP_TEXTURE2D_SLOT, s_ShadowMapCascadesTexture->srv, ShaderStage::PIXEL);

    return true;
}

void PGRenderer::ResizeResources(size_t newWidth, size_t newHeight) {
    s_RendererAPI->ResizeBackBuffer(newWidth, newHeight);

    SAFE_DELETE(s_HDRRenderTarget);
    SAFE_DELETE(s_DepthStencilTarget);
    SAFE_DELETE(s_ResolvedHDRRenderTarget);

    // HDR mainbuffer
    Texture2DDesc hdrBufferInitParams = {};
    hdrBufferInitParams.arraySize = 1;
    hdrBufferInitParams.format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    hdrBufferInitParams.width = newWidth;
    hdrBufferInitParams.height = newHeight;
    hdrBufferInitParams.sampleCount = s_MSAASampleCount;
    hdrBufferInitParams.mipCount = 1;
    hdrBufferInitParams.flags = TextureResourceFlags::BIND_SHADER_RESOURCE | TextureResourceFlags::BIND_RENDER_TARGET;
    s_HDRRenderTarget = new GPUResource(s_RendererAPI, &hdrBufferInitParams, nullptr, "MainHDRTexture");

    Texture2DDesc depthTextureInitParams = {};
    depthTextureInitParams.arraySize = 1;
    depthTextureInitParams.format = DXGI_FORMAT_D32_FLOAT;
    depthTextureInitParams.width = newWidth;
    depthTextureInitParams.height = newHeight;
    depthTextureInitParams.sampleCount = hdrBufferInitParams.sampleCount;
    depthTextureInitParams.mipCount = 1;
    depthTextureInitParams.flags = TextureResourceFlags::BIND_DEPTH_STENCIL;
    s_DepthStencilTarget = new GPUResource(s_RendererAPI, &depthTextureInitParams, nullptr, "MainDepthStencilTexture");

    if (s_MSAASampleCount > 1) {
        Texture2DDesc resolvedBufferInitParams = {};
        resolvedBufferInitParams.arraySize = 1;
        resolvedBufferInitParams.format = hdrBufferInitParams.format;
        resolvedBufferInitParams.width = hdrBufferInitParams.width;
        resolvedBufferInitParams.height = hdrBufferInitParams.height;
        resolvedBufferInitParams.sampleCount = 1;
        resolvedBufferInitParams.mipCount = 1;
        resolvedBufferInitParams.flags = TextureResourceFlags::BIND_SHADER_RESOURCE | TextureResourceFlags::BIND_RENDER_TARGET;
        s_ResolvedHDRRenderTarget = new GPUResource(s_RendererAPI, &resolvedBufferInitParams, nullptr, "MainResolvedHDRTexture");
    }

    // Render passes
    PGShader* pbrForward = s_ShaderLib->GetDefaultShader("PBRForward");
    s_SceneRenderPass.SetRenderTarget(0, s_HDRRenderTarget->rtv);
    s_SceneRenderPass.SetDepthStencilTarget(s_DepthStencilTarget->dsv);
    HWViewport defaultViewport = s_RendererAPI->GetDefaultViewport();
    s_SceneRenderPass.SetViewport(defaultViewport);
    s_SceneRenderPass.SetShader(pbrForward);

    // Post process
    PGShader* tonemappingShader = s_ShaderLib->GetDefaultShader("HDRPostProcess");
    s_ToneMappingPass.SetRenderTarget(0, s_RendererAPI->GetBackbufferRenderTargetView());
    s_ToneMappingPass.SetViewport(defaultViewport);
    s_ToneMappingPass.SetShader(tonemappingShader);
    if (s_MSAASampleCount > 1) {
        s_ToneMappingPass.SetShaderResource(POST_PROCESS_TEXTURE0_SLOT, s_ResolvedHDRRenderTarget->srv, ShaderStage::PIXEL);
    } else {
        s_ToneMappingPass.SetShaderResource(POST_PROCESS_TEXTURE0_SLOT, s_HDRRenderTarget->srv, ShaderStage::PIXEL);
    }

}

void PGRenderer::BeginFrame() {
}

void PGRenderer::RenderFrame() {
    s_ShaderLib->ReloadShadersIfNeeded();

    //TODO: This function should be create gpu commands and pass them to render thread.
    PerFrameGlobalConstantBuffer perFrameGlobalConstantBuffer = {};
    perFrameGlobalConstantBuffer.g_ViewMatrix = s_ActiveSceneData->camera->GetViewMatrix();
    perFrameGlobalConstantBuffer.g_ProjMatrix = s_ActiveSceneData->camera->GetProjectionMatrix();
    perFrameGlobalConstantBuffer.g_CameraPos = Vector4(s_ActiveSceneData->camera->GetPosition(), 1.0f);
    perFrameGlobalConstantBuffer.g_InverseViewProjMatrix = s_ActiveSceneData->camera->GetInverseProjectionViewMatrix();

    perFrameGlobalConstantBuffer.g_LightPos = Vector4(s_ActiveSceneData->light->position, 1.0f);
    Matrix4 lightView = LookAtLH(Vector3(0.0f, 0.0f, 0.0f), Normalize(-s_ActiveSceneData->light->position), Vector3(0.0f, 0.0f, 1.0f));
    perFrameGlobalConstantBuffer.g_LightViewMatrix = lightView;
    CalculateCascadeProjMatrices(s_ActiveSceneData->camera, lightView, SHADOW_MAP_SIZE,
                                 perFrameGlobalConstantBuffer.g_LightProjMatrices);

    void* data = s_RendererAPI->Map(s_PerFrameGlobalConstantBuffer);
    memcpy(data, &perFrameGlobalConstantBuffer, sizeof(PerFrameGlobalConstantBuffer));
    s_RendererAPI->Unmap(s_PerFrameGlobalConstantBuffer);

    s_ShadowGenStage.Execute(s_RendererAPI, s_RenderObjects);

    s_SceneRenderPass.Execute(s_RendererAPI);

    // Render skybox
    if (s_ActiveSceneData->skybox) {
        s_ActiveSceneData->skybox->RenderSkybox();
    }

    if (s_MSAASampleCount > 1) {
        s_RendererAPI->MSAAResolve((HWTexture2D*)s_ResolvedHDRRenderTarget->resource, (HWTexture2D*)s_HDRRenderTarget->resource);
    }

    // PostProcess
    s_ToneMappingPass.Execute(s_RendererAPI);


    // Clear all texture slots
    HWShaderResourceView* nullView[16] = { 0 };
    s_RendererAPI->SetShaderResourcesPS(0, nullView, ARRAYSIZE(nullView));
}

void PGRenderer::EndFrame() {
    s_RendererAPI->Present();
}

void PGRenderer::BeginScene(PGScene* sceneData) {
    s_ActiveSceneData = sceneData;
}

void PGRenderer::AddMesh(const MeshRef& renderMesh) {
    if (renderMesh.get()) {
        PGRenderObject* object = new PGRenderObject(renderMesh, s_RendererAPI);
        object->perDrawConstantBuffer = s_PerDrawGlobalConstantBuffer;
        s_RenderObjects.push_back(object);
    }
}

void PGRenderer::EndScene() {
    // TODO: sorting, bathcing, etc
    s_SceneRenderPass.SetRenderObjects(s_RenderObjects);
}
