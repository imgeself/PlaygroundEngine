#include "PGRenderer.h"
#include "Skybox.h"
#include "../PGProfiler.h"

#include "DX11/DX11RendererAPI.h"

#include <memory>

HWRendererAPI* PGRenderer::s_RendererAPI = nullptr;
PGShaderLib* PGRenderer::s_ShaderLib = nullptr;
PGScene* PGRenderer::s_ActiveSceneData = nullptr;

PGRendererConfig PGRenderer::s_RendererConfig;
RenderList PGRenderer::s_RenderList;

ShadowGenStage PGRenderer::s_ShadowGenStage = ShadowGenStage();
SceneRenderPass PGRenderer::s_SceneZPrePass = SceneRenderPass();
SceneRenderPass PGRenderer::s_SceneRenderPass = SceneRenderPass();
FullscreenPass PGRenderer::s_ToneMappingPass = FullscreenPass();

class DebugSceneRenderer {
public:
    DebugSceneRenderer(HWRendererAPI* rendererAPI, PGShaderLib* shaderLib, const PGRendererConfig& rendererConfig) {
        m_VertexBuffer = rendererAPI->CreateBuffer(nullptr, sizeof(Vector3) * 24, HWResourceFlags::BIND_VERTEX_BUFFER | HWResourceFlags::USAGE_DYNAMIC | HWResourceFlags::CPU_ACCESS_WRITE);

        HWRasterizerDesc rasterizerDesc = {};
        rasterizerDesc.cullMode = CULL_NONE;
        if (rendererConfig.msaaSampleCount > 1) {
            rasterizerDesc.antialiasedLineEnable = true;
            rasterizerDesc.multisampleEnable = true;
        }

        PGShader* debugShader = shaderLib->GetDefaultShader("DebugShader");

        HWGraphicsPipelineStateDesc pipelineStateDesc;
        pipelineStateDesc.rasterizerDesc = rasterizerDesc;
        pipelineStateDesc.inputLayoutDesc = PGRendererResources::s_DefaultInputLayoutDescs[InputLayoutType::POS];
        pipelineStateDesc.primitiveTopology = PRIMITIVE_TOPOLOGY_LINELIST;

        pipelineStateDesc.vertexShader = debugShader->GetVertexBytecode();
        pipelineStateDesc.pixelShader = debugShader->GetPixelBytecode();

        m_PipelineState = rendererAPI->CreateGraphicsPipelineState(pipelineStateDesc);
    }

    ~DebugSceneRenderer() {
        SAFE_DELETE(m_VertexBuffer);
        SAFE_DELETE(m_PipelineState);
    }

    void Execute(HWRendererAPI* rendererAPI, const RenderList* renderList) {
        rendererAPI->SetGraphicsPipelineState(m_PipelineState);
        uint32_t stride = sizeof(Vector3);
        uint32_t offset = 0;
        rendererAPI->SetVertexBuffers(&m_VertexBuffer, 1, &stride, &offset);

        RenderSceneDebug(rendererAPI, renderList, m_VertexBuffer);
    }

private:
    HWBuffer* m_VertexBuffer;
    HWGraphicsPipelineState* m_PipelineState;
};

DebugSceneRenderer* g_DebugSceneRenderer = nullptr;
Skybox* g_SkyboxRenderer = nullptr;

void PGRenderer::Destroy() {
    PGRendererResources::ClearResources();

    SAFE_DELETE(g_SkyboxRenderer);
    SAFE_DELETE(g_DebugSceneRenderer);

    delete s_ShaderLib;
    delete s_RendererAPI;
}

void CalculateCascadeProjMatrices(Matrix4& cameraInverseProjectionViewMatrix, Matrix4 lightView, const PGRendererConfig& rendererConfig, const Box& sceneBoundingBox,
                                  Matrix4* outMatricesData) {
    PG_PROFILE_FUNCTION();

    // Get frustum points in world space
    Vector4 frustumCorners[] = {
        (cameraInverseProjectionViewMatrix * Vector4(-1, -1, 0, 1)), // left-bottom-near
        (cameraInverseProjectionViewMatrix * Vector4(-1, 1, 0, 1)),  // left-top-near
        (cameraInverseProjectionViewMatrix * Vector4(1, -1, 0, 1)),  // right-bottom-near
        (cameraInverseProjectionViewMatrix * Vector4(1, 1, 0, 1)),   // right-top-near
        (cameraInverseProjectionViewMatrix * Vector4(-1, -1, 1, 1)), // left-bottom-far
        (cameraInverseProjectionViewMatrix * Vector4(-1, 1, 1, 1)),  // left-top-far
        (cameraInverseProjectionViewMatrix * Vector4(1, -1, 1, 1)),  // right-bottom-far
        (cameraInverseProjectionViewMatrix * Vector4(1, 1, 1, 1)),   // right-top-far
    };

    Vector4 unprojectedPoints[] = {
        (frustumCorners[0] / frustumCorners[0].w),
        (frustumCorners[1] / frustumCorners[1].w),
        (frustumCorners[2] / frustumCorners[2].w),
        (frustumCorners[3] / frustumCorners[3].w),
        (frustumCorners[4] / frustumCorners[4].w),
        (frustumCorners[5] / frustumCorners[5].w),
        (frustumCorners[6] / frustumCorners[6].w),
        (frustumCorners[7] / frustumCorners[7].w)
    };

    Vector3 sceneBoundingBoxCorners[] = {
        sceneBoundingBox.min,
        Vector3(sceneBoundingBox.min.x, sceneBoundingBox.max.y, sceneBoundingBox.min.z),
        Vector3(sceneBoundingBox.max.x, sceneBoundingBox.min.y, sceneBoundingBox.min.z),
        Vector3(sceneBoundingBox.max.x, sceneBoundingBox.max.y, sceneBoundingBox.min.z),

        Vector3(sceneBoundingBox.min.x, sceneBoundingBox.max.y, sceneBoundingBox.max.z),
        Vector3(sceneBoundingBox.max.x, sceneBoundingBox.min.y, sceneBoundingBox.max.z),
        Vector3(sceneBoundingBox.min.x, sceneBoundingBox.min.y, sceneBoundingBox.max.z),
        sceneBoundingBox.max
    };

    Vector3 sceneBoundingBoxCornersLightSpace[8];
    for (size_t i = 0; i < ARRAYSIZE(sceneBoundingBoxCorners); i++) {
        Vector4 v = lightView * Vector4(sceneBoundingBoxCorners[i], 1.0f);

        sceneBoundingBoxCornersLightSpace[i] = (v.xyz() / v.w);
    }

    float minScenePointLightSpace = sceneBoundingBoxCornersLightSpace[0].x;
    float maxScenePointLightSpace = sceneBoundingBoxCornersLightSpace[0].x;
    for (size_t i = 1; i < ARRAYSIZE(sceneBoundingBoxCornersLightSpace); i++) {
        Vector3 corner = sceneBoundingBoxCornersLightSpace[i];

        if (corner.x < minScenePointLightSpace) {
            minScenePointLightSpace = corner.x;
        } else if (corner.x > maxScenePointLightSpace) {
            maxScenePointLightSpace = corner.x;
        }

        if (corner.y < minScenePointLightSpace) {
            minScenePointLightSpace = corner.y;
        }
        else if (corner.y > maxScenePointLightSpace) {
            maxScenePointLightSpace = corner.y;
        }

        if (corner.z < minScenePointLightSpace) {
            minScenePointLightSpace = corner.z;
        }
        else if (corner.z > maxScenePointLightSpace) {
            maxScenePointLightSpace = corner.z;
        }
    }

    // Little padding for avoiding clipping
    minScenePointLightSpace -= 2.0f;
    maxScenePointLightSpace += 2.0f;

    const uint8_t cascadeCount = rendererConfig.shadowCascadeCount;

    float* splits = (float*) alloca(sizeof(float) * (cascadeCount + 1));
    memset(splits, 0, sizeof(float) * (cascadeCount + 1));
    float split = 0.01f;
    for (size_t i = 1; i < cascadeCount; ++i) {
        *(splits + i) = split * std::powf(3.0f, i);
    }
    *(splits + cascadeCount) = 1.0f;

    Matrix4* cascadeProjMatrices = (Matrix4*) alloca(sizeof(Matrix4) * cascadeCount);
    memset(cascadeProjMatrices, 0, sizeof(Matrix4) * cascadeCount);
    for (uint32_t cascadeIndex = 0; cascadeIndex < cascadeCount; ++cascadeIndex) {
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
            radius = std::max(radius, Lenght(points[i].xyz() - center));
        }

        // We make cascade projection matrix as a cube so that we can snap cascade to texel size
        Vector3 minCorner = center + Vector3(-radius, -radius, -radius);
        Vector3 maxCorner = center + Vector3(radius, radius, radius);

        float texelSize = 1.0f / rendererConfig.shadowMapSize;
        Vector3 extend = maxCorner - minCorner;
        Vector3 worldUnitsForTexelSize = extend * texelSize;
        minCorner = Floor(minCorner / worldUnitsForTexelSize) * worldUnitsForTexelSize;
        maxCorner = Floor(maxCorner / worldUnitsForTexelSize) * worldUnitsForTexelSize;

        *(cascadeProjMatrices + cascadeIndex) = OrthoMatrixOffCenterLH(minCorner.x, maxCorner.x, maxCorner.y, minCorner.y, 
            minScenePointLightSpace, maxScenePointLightSpace);
    }

    memcpy(outMatricesData, cascadeProjMatrices, cascadeCount * sizeof(Matrix4));

}

bool PGRenderer::Initialize(PGWindow* window) {
    s_RendererAPI = new DX11RendererAPI(window);
    s_ShaderLib = new PGShaderLib(s_RendererAPI);
    s_ShaderLib->LoadDefaultShaders();

    PGRendererResources::CreateDefaultInputLayout(s_RendererAPI);

    PGRendererResources::CreateDefaultBuffers(s_RendererAPI, s_RendererConfig);
    HWBuffer* vsConstantBuffers[8] = {0};
    HWBuffer* psConstantBuffers[8] = {0};

    vsConstantBuffers[PER_DRAW_CBUFFER_SLOT] = PGRendererResources::s_PerDrawGlobalConstantBuffer;
    vsConstantBuffers[PER_VIEW_CBUFFER_SLOT] = PGRendererResources::s_PerViewGlobalConstantBuffer;
    vsConstantBuffers[PER_FRAME_CBUFFER_SLOT] = PGRendererResources::s_PerFrameGlobalConstantBuffer;
    vsConstantBuffers[RENDERER_VARIABLES_CBUFFER_SLOT] = PGRendererResources::s_RendererVarsConstantBuffer;

    psConstantBuffers[PER_MATERIAL_CBUFFER_SLOT] = PGRendererResources::s_PerMaterialGlobalConstantBuffer;
    psConstantBuffers[PER_VIEW_CBUFFER_SLOT] = PGRendererResources::s_PerViewGlobalConstantBuffer;
    psConstantBuffers[PER_FRAME_CBUFFER_SLOT] = PGRendererResources::s_PerFrameGlobalConstantBuffer;
    psConstantBuffers[POST_PROCESS_CBUFFER_SLOT] = PGRendererResources::s_PostProcessConstantBuffer;
    psConstantBuffers[RENDERER_VARIABLES_CBUFFER_SLOT] = PGRendererResources::s_RendererVarsConstantBuffer;
    s_RendererAPI->SetConstantBuffersVS(0, vsConstantBuffers, ARRAYSIZE(vsConstantBuffers));
    s_RendererAPI->SetConstantBuffersPS(0, psConstantBuffers, ARRAYSIZE(psConstantBuffers));

    // Bind default sampler states
    PGRendererResources::CreateDefaultSamplerStates(s_RendererAPI);
    s_RendererAPI->SetSamplerStatesPS(0, PGRendererResources::s_DefaultSamplers.data(), PGRendererResources::s_DefaultSamplers.max_size());

    // init shadow mapping
    PGRendererResources::CreateShadowMapResources(s_RendererAPI, s_RendererConfig);
    s_ShadowGenStage.Initialize(s_RendererAPI);
    s_ShadowGenStage.SetShadowMapTarget(s_RendererAPI, (HWTexture2D*) PGRendererResources::s_ShadowMapCascadesTexture->resource, s_RendererConfig);
    s_SceneRenderPass.SetShaderResource(SHADOW_MAP_TEXTURE2D_SLOT, PGRendererResources::s_ShadowMapCascadesTexture->srv, ShaderStage::PIXEL);

    PGShader* tonemappingShader = s_ShaderLib->GetDefaultShader("HDRPostProcess");
    s_ToneMappingPass.SetShader(s_RendererAPI, tonemappingShader);

    g_SkyboxRenderer = new Skybox(s_RendererAPI, s_ShaderLib);
    g_DebugSceneRenderer = new DebugSceneRenderer(s_RendererAPI, s_ShaderLib, s_RendererConfig);

    return true;
}

void PGRenderer::ResizeResources(size_t newWidth, size_t newHeight) {
    s_RendererAPI->ResizeBackBuffer(newWidth, newHeight);

    s_RendererConfig.width = newWidth;
    s_RendererConfig.height = newHeight;

    PGRendererResources::CreateSizeDependentResources(s_RendererAPI, s_RendererConfig);

    // Render passes
    HWViewport defaultViewport = s_RendererAPI->GetDefaultViewport();
    s_SceneZPrePass.SetDepthStencilTarget(PGRendererResources::s_DepthStencilTarget->dsv);
    s_SceneZPrePass.SetViewport(defaultViewport);

    s_SceneRenderPass.SetRenderTarget(0, PGRendererResources::s_HDRRenderTarget->rtv);
    s_SceneRenderPass.SetDepthStencilTarget(PGRendererResources::s_DepthStencilTarget->dsv);
    s_SceneRenderPass.SetViewport(defaultViewport);

    // Post process
    s_ToneMappingPass.SetRenderTarget(0, s_RendererAPI->GetBackbufferRenderTargetView());
    s_ToneMappingPass.SetViewport(defaultViewport);
    if (s_RendererConfig.msaaSampleCount > 1) {
        s_ToneMappingPass.SetShaderResource(POST_PROCESS_TEXTURE0_SLOT, PGRendererResources::s_ResolvedHDRRenderTarget->srv, ShaderStage::PIXEL);
    } else {
        s_ToneMappingPass.SetShaderResource(POST_PROCESS_TEXTURE0_SLOT, PGRendererResources::s_HDRRenderTarget->srv, ShaderStage::PIXEL);
    }

}

void PGRenderer::BeginFrame() {
}

void PGRenderer::RenderFrame() {
    s_ShaderLib->ReloadShadersIfNeeded();
    PGRendererResources::UpdateShaders(s_RendererAPI);

    //TODO: We are adding all the scene objects into the render list. Needs culling to eleminate some of the scene objects
    s_RenderList.Clear();
    s_RenderList.AddSceneObjects(s_ActiveSceneData->sceneObjects.data(), s_ActiveSceneData->sceneObjects.size(), s_ActiveSceneData->camera);
    s_RenderList.ValidatePipelineStates(s_ShaderLib, s_RendererAPI);

    PGCamera* mainCamera = s_ActiveSceneData->camera;
    Matrix4 cameraInverseProjViewMatrix = mainCamera->GetInverseProjectionViewMatrix();
    PGRenderView mainRenderView;
    mainRenderView.renderList = &s_RenderList;
    mainRenderView.cameraPos = mainCamera->GetPosition();
    mainRenderView.viewMatrix = mainCamera->GetViewMatrix();
    mainRenderView.projMatrix = mainCamera->GetProjectionMatrix();
    mainRenderView.projViewMatrix = mainCamera->GetProjectionViewMatrix();
    mainRenderView.inverseProjViewMatrix = cameraInverseProjViewMatrix;

    PGLight* directionalLight = s_ActiveSceneData->directionalLight;
    Vector3 directionalLightDirection = Normalize(-directionalLight->position);
    PerFrameGlobalConstantBuffer perFrameGlobalConstantBuffer = {};
    perFrameGlobalConstantBuffer.g_DirectionLightDirection = Vector4(directionalLightDirection, 1.0f);
    perFrameGlobalConstantBuffer.g_DirectionLightColor = Vector4(directionalLight->color, directionalLight->intensity);

    Matrix4 lightView = LookAtLH(Vector3(0.0f, 0.0f, 0.0f), directionalLightDirection, Vector3(0.0f, 0.0f, 1.0f));
    perFrameGlobalConstantBuffer.g_DirectionLightViewMatrix = lightView;
    Matrix4 shadowProjMatrices[MAX_SHADOW_CASCADE_COUNT];
    CalculateCascadeProjMatrices(cameraInverseProjViewMatrix, lightView, s_RendererConfig, s_ActiveSceneData->boundingBox,
                                 shadowProjMatrices);
    memcpy(perFrameGlobalConstantBuffer.g_DirectionLightProjMatrices, shadowProjMatrices, sizeof(Matrix4) * MAX_SHADOW_CASCADE_COUNT);

    PGRenderView shadowCascadeRenderViews[MAX_SHADOW_CASCADE_COUNT];
    for (size_t cascadeIndex = 0; cascadeIndex < MAX_SHADOW_CASCADE_COUNT; ++cascadeIndex) {
        PGRenderView& renderView = shadowCascadeRenderViews[cascadeIndex];
        Matrix4& cascadeProjMatrix = shadowProjMatrices[cascadeIndex];

        renderView.renderList = &s_RenderList;
        renderView.cameraPos = directionalLight->position;
        renderView.viewMatrix = lightView;
        renderView.projMatrix = cascadeProjMatrix;
        renderView.projViewMatrix = cascadeProjMatrix * lightView;
    }

    void* data = s_RendererAPI->Map(PGRendererResources::s_PerFrameGlobalConstantBuffer);
    memcpy(data, &perFrameGlobalConstantBuffer, sizeof(PerFrameGlobalConstantBuffer));
    s_RendererAPI->Unmap(PGRendererResources::s_PerFrameGlobalConstantBuffer);

    s_RenderList.SortByDepth();
    {
        PG_PROFILE_SCOPE("Z Prepass");
        s_SceneZPrePass.Execute(s_RendererAPI, mainRenderView, SceneRenderPassType::DEPTH_PASS);
    }

    s_ShadowGenStage.Execute(s_RendererAPI, shadowCascadeRenderViews, s_RendererConfig);

    s_RenderList.SortByKey();
    {
        PG_PROFILE_SCOPE("Forward Pass");
        s_SceneRenderPass.Execute(s_RendererAPI, mainRenderView, SceneRenderPassType::FORWARD, false);
    }

    // Render skybox
    if (s_ActiveSceneData->skyboxTexture) {
        g_SkyboxRenderer->RenderSkybox(s_RendererAPI, s_ActiveSceneData->skyboxTexture);
    }

    if (s_RendererConfig.debugDrawBoundingBoxes) {
        PG_PROFILE_SCOPE("Debug Rendering");
        g_DebugSceneRenderer->Execute(s_RendererAPI, &s_RenderList);
    }

    if (s_RendererConfig.msaaSampleCount > 1) {
        s_RendererAPI->MSAAResolve((HWTexture2D*)PGRendererResources::s_ResolvedHDRRenderTarget->resource, (HWTexture2D*)PGRendererResources::s_HDRRenderTarget->resource);
    }

    // PostProcess
    {
        PG_PROFILE_SCOPE("Tone Mapping");
        s_ToneMappingPass.Execute(s_RendererAPI);
    }


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

void PGRenderer::EndScene() {
    // TODO: sorting, bathcing, etc
}
