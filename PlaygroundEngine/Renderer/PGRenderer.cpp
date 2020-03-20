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

        HWPipelineStateDesc pipelineStateDesc;
        pipelineStateDesc.rasterizerDesc = rasterizerDesc;
        pipelineStateDesc.inputLayoutDesc = PGRendererResources::s_DefaultInputLayoutDescs[InputLayoutType::POS];
        pipelineStateDesc.primitiveTopology = PRIMITIVE_TOPOLOGY_LINELIST;

        pipelineStateDesc.vertexShader = debugShader->GetVertexBytecode();
        pipelineStateDesc.pixelShader = debugShader->GetPixelBytecode();

        m_PipelineState = rendererAPI->CreatePipelineState(pipelineStateDesc);
    }

    ~DebugSceneRenderer() {
        SAFE_DELETE(m_VertexBuffer);
        SAFE_DELETE(m_PipelineState);
    }

    void Execute(HWRendererAPI* rendererAPI, const RenderList& renderList) {
        rendererAPI->SetPipelineState(m_PipelineState);
        uint32_t stride = sizeof(Vector3);
        uint32_t offset = 0;
        rendererAPI->SetVertexBuffers(&m_VertexBuffer, 1, &stride, &offset);

        RenderSceneDebug(rendererAPI, renderList, m_VertexBuffer);
    }

private:
    HWBuffer* m_VertexBuffer;
    HWPipelineState* m_PipelineState;
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

void CalculateCascadeProjMatrices(PGCamera* camera, Matrix4 lightView, const PGRendererConfig& rendererConfig,
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
            radius = std::max(radius, Lenght(points[i].xyz() - center));
        }

        // We make cascade projection matrix as a cube so that we can snap cascade to texel size
        Vector3 minCorner = center + Vector3(-radius, -radius, -radius);
        Vector3 maxCorner = center + Vector3(radius, radius, radius);

        // Extrude min/max z values for avoid shadow camera being too close to scene and causing clipping
        // TODO: Instead of this method we need to clamp max/min z values to scene's bounding box's top and bottom lines
        minCorner.z = std::min(minCorner.z, -50.0f);
        maxCorner.z = std::min(maxCorner.z, 50.0f);

        float texelSize = 1.0f / rendererConfig.shadowMapSize;
        Vector3 extend = maxCorner - minCorner;
        Vector3 worldUnitsForTexelSize = extend * texelSize;
        minCorner = Floor(minCorner / worldUnitsForTexelSize) * worldUnitsForTexelSize;
        maxCorner = Floor(maxCorner / worldUnitsForTexelSize) * worldUnitsForTexelSize;


        cascadeProjMatrices[cascadeIndex] = OrthoMatrixOffCenterLH(minCorner.x, maxCorner.x, maxCorner.y, minCorner.y, minCorner.z, maxCorner.z);
    }

    memcpy(outMatricesData, cascadeProjMatrices, ARRAYSIZE(cascadeProjMatrices) * sizeof(Matrix4));

}

bool PGRenderer::Initialize(PGWindow* window) {
    s_RendererAPI = new DX11RendererAPI(window);
    s_ShaderLib = new PGShaderLib(s_RendererAPI);
    s_ShaderLib->LoadDefaultShaders();

    PGRendererResources::CreateDefaultInputLayout(s_RendererAPI);

    PGRendererResources::CreateDefaultBuffers(s_RendererAPI, s_RendererConfig);
    HWBuffer* constantBuffers[8] = {0};
    constantBuffers[PER_DRAW_CBUFFER_SLOT] = PGRendererResources::s_PerDrawGlobalConstantBuffer;
    constantBuffers[PER_FRAME_CBUFFER_SLOT] = PGRendererResources::s_PerFrameGlobalConstantBuffer;
    constantBuffers[RENDERER_VARIABLES_CBUFFER_SLOT] = PGRendererResources::s_RendererVarsConstantBuffer;
    constantBuffers[POST_PROCESS_CBUFFER_SLOT] = PGRendererResources::s_PostProcessConstantBuffer;
    s_RendererAPI->SetConstantBuffersVS(0, constantBuffers, ARRAYSIZE(constantBuffers));
    s_RendererAPI->SetConstantBuffersPS(0, constantBuffers, ARRAYSIZE(constantBuffers));

    // Bind default sampler states
    PGRendererResources::CreateDefaultSamplerStates(s_RendererAPI);
    s_RendererAPI->SetSamplerStatesPS(0, PGRendererResources::s_DefaultSamplers.data(), PGRendererResources::s_DefaultSamplers.max_size());

    // init shadow mapping
    PGRendererResources::CreateShadowMapResources(s_RendererAPI, s_RendererConfig);
    s_ShadowGenStage.Initialize(s_RendererAPI, PGRendererResources::s_ShadowMapCascadesTexture, s_RendererConfig.shadowMapSize);
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
    s_SceneRenderPass.SetRenderTarget(0, PGRendererResources::s_HDRRenderTarget->rtv);
    s_SceneRenderPass.SetDepthStencilTarget(PGRendererResources::s_DepthStencilTarget->dsv);
    HWViewport defaultViewport = s_RendererAPI->GetDefaultViewport();
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

    //TODO: This function should be create gpu commands and pass them to the render thread.
    PerFrameGlobalConstantBuffer perFrameGlobalConstantBuffer = {};
    perFrameGlobalConstantBuffer.g_ViewMatrix = s_ActiveSceneData->camera->GetViewMatrix();
    perFrameGlobalConstantBuffer.g_ProjMatrix = s_ActiveSceneData->camera->GetProjectionMatrix();
    perFrameGlobalConstantBuffer.g_CameraPos = Vector4(s_ActiveSceneData->camera->GetPosition(), 1.0f);
    perFrameGlobalConstantBuffer.g_InverseViewProjMatrix = s_ActiveSceneData->camera->GetInverseProjectionViewMatrix();

    perFrameGlobalConstantBuffer.g_LightPos = Vector4(s_ActiveSceneData->light->position, 1.0f);
    Matrix4 lightView = LookAtLH(Vector3(0.0f, 0.0f, 0.0f), Normalize(-s_ActiveSceneData->light->position), Vector3(0.0f, 0.0f, 1.0f));
    perFrameGlobalConstantBuffer.g_LightViewMatrix = lightView;
    CalculateCascadeProjMatrices(s_ActiveSceneData->camera, lightView, s_RendererConfig,
                                 perFrameGlobalConstantBuffer.g_LightProjMatrices);

    void* data = s_RendererAPI->Map(PGRendererResources::s_PerFrameGlobalConstantBuffer);
    memcpy(data, &perFrameGlobalConstantBuffer, sizeof(PerFrameGlobalConstantBuffer));
    s_RendererAPI->Unmap(PGRendererResources::s_PerFrameGlobalConstantBuffer);

    s_RenderList.SortByDepth();
    s_ShadowGenStage.Execute(s_RendererAPI, s_RenderList, s_ShaderLib);

    s_RenderList.SortByKey();
    s_SceneRenderPass.Execute(s_RendererAPI, s_RenderList, SceneRenderPassType::FORWARD);

    // Render skybox
    if (s_ActiveSceneData->skyboxTexture) {
        g_SkyboxRenderer->RenderSkybox(s_RendererAPI, s_ActiveSceneData->skyboxTexture);
    }

    if (s_RendererConfig.debugRendering) {
        g_DebugSceneRenderer->Execute(s_RendererAPI, s_RenderList);
    }

    if (s_RendererConfig.msaaSampleCount > 1) {
        s_RendererAPI->MSAAResolve((HWTexture2D*)PGRendererResources::s_ResolvedHDRRenderTarget->resource, (HWTexture2D*)PGRendererResources::s_HDRRenderTarget->resource);
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

void PGRenderer::EndScene() {
    // TODO: sorting, bathcing, etc
}
