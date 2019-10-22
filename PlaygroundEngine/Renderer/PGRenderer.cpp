#include "PGRenderer.h"

#include "DX11/DX11RendererAPI.h"

#include <memory>

HWRendererAPI* PGRenderer::s_RendererAPI = nullptr;
PGShaderLib* PGRenderer::s_ShaderLib = nullptr;
std::vector<PGRenderObject*> PGRenderer::s_RenderObjects = std::vector<PGRenderObject*>();
PGScene* PGRenderer::s_ActiveSceneData = nullptr;

HWConstantBuffer* PGRenderer::s_PerFrameGlobalConstantBuffer = nullptr;
HWConstantBuffer* PGRenderer::s_PerDrawGlobalConstantBuffer = nullptr;

std::array<HWSamplerState*, 4> PGRenderer::s_DefaultSamplers = std::array<HWSamplerState*, 4>();

ShadowMapPass PGRenderer::s_ShadowMapPass = ShadowMapPass();
SceneRenderPass PGRenderer::s_SceneRenderPass = SceneRenderPass();

enum RenderPass {
    SHADOW,
    FORWARD
};

void PGRenderer::Uninitialize() {
    delete s_PerFrameGlobalConstantBuffer;
    delete s_PerDrawGlobalConstantBuffer;
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

bool PGRenderer::Initialize(PGWindow* window) {
    s_RendererAPI = new DX11RendererAPI(window);
    s_ShaderLib = new PGShaderLib(s_RendererAPI);
    s_ShaderLib->LoadDefaultShaders();

    // Default resources init
    PerFrameGlobalConstantBuffer perFrameGlobalConstantBuffer = {};
    s_PerFrameGlobalConstantBuffer = s_RendererAPI->CreateConstantBuffer(&perFrameGlobalConstantBuffer, sizeof(PerFrameGlobalConstantBuffer));

    PerDrawGlobalConstantBuffer perDrawGlobalConstantBuffer = {};
    s_PerDrawGlobalConstantBuffer = s_RendererAPI->CreateConstantBuffer(&perDrawGlobalConstantBuffer, sizeof(PerDrawGlobalConstantBuffer));

    HWConstantBuffer* constantBuffers[2] = { s_PerDrawGlobalConstantBuffer, s_PerFrameGlobalConstantBuffer };
    s_RendererAPI->SetConstanBuffersVS(0, constantBuffers, 2);
    s_RendererAPI->SetConstanBuffersPS(0, &s_PerFrameGlobalConstantBuffer, 1);

    // Default sampler states
    SamplerStateInitParams samplerStateInitParams = {};
    samplerStateInitParams.filterMode = TextureFilterMode_POINT;
    samplerStateInitParams.AddressModeU = TextureAddressMode_BORDER;
    samplerStateInitParams.AddressModeV = TextureAddressMode_BORDER;
    samplerStateInitParams.AddressModeW = TextureAddressMode_BORDER;
    float borderColor[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
    memcpy(samplerStateInitParams.borderColor, borderColor, sizeof(float) * 4);

    s_DefaultSamplers[POINT_BORDER_SAMPLER_STATE_SLOT] = s_RendererAPI->CreateSamplerState(&samplerStateInitParams);

    // Bind default sampler states
    s_RendererAPI->SetSamplerStatesPS(0, s_DefaultSamplers.data(), s_DefaultSamplers.max_size());



    s_SceneRenderPass.SetRenderTarget(0, s_RendererAPI->GetBackbufferRenderTargetView());
    s_SceneRenderPass.SetDepthStencilTarget(s_RendererAPI->GetBackbufferDepthStencilView());
    HWViewport defaultViewport = s_RendererAPI->GetDefaultViewport();
    s_SceneRenderPass.SetViewport(defaultViewport);

    s_ShadowMapPass.Initialize(s_RendererAPI, s_ShaderLib, 1024, 1024);
    s_SceneRenderPass.SetShadowMapPass(&s_ShadowMapPass);

    return true;
}

void PGRenderer::BeginFrame() {
    const float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    s_RendererAPI->ClearScreen(color);
}

void PGRenderer::RenderFrame() {
    s_ShaderLib->ReloadShadersIfNeeded();

    //TODO: This function should be create gpu commands and pass them to render thread.
    PerFrameGlobalConstantBuffer perFrameGlobalConstantBuffer = {};
    perFrameGlobalConstantBuffer.viewMatrix = s_ActiveSceneData->camera->GetViewMatrix();
    perFrameGlobalConstantBuffer.projMatrix = s_ActiveSceneData->camera->GetProjectionMatrix();
    perFrameGlobalConstantBuffer.cameraPos = Vector4(s_ActiveSceneData->camera->GetPosition(), 1.0f);

    int shadowMapHeight = 1024;
    perFrameGlobalConstantBuffer.lightPos = Vector4(s_ActiveSceneData->light->position, 1.0f);
    perFrameGlobalConstantBuffer.lightProjMatrix = OrthoMatrixLH(20.0f, 20.0f, 0.001f, 100.0f);
    perFrameGlobalConstantBuffer.lightViewMatrix = LookAtLH(s_ActiveSceneData->light->position, Vector3(0.0f, 0.0f, 0.0f));

    void* data = s_RendererAPI->Map(s_PerFrameGlobalConstantBuffer);
    memcpy(data, &perFrameGlobalConstantBuffer, sizeof(PerFrameGlobalConstantBuffer));
    s_RendererAPI->Unmap(s_PerFrameGlobalConstantBuffer);

    
    s_SceneRenderPass.Execute(s_RendererAPI);
}

void PGRenderer::EndFrame() {
    s_RendererAPI->Present();
}

void PGRenderer::BeginScene(PGScene* sceneData) {
    s_ActiveSceneData = sceneData;
}

void PGRenderer::AddMesh(Mesh* renderMesh) {
    PGRenderObject* object = new PGRenderObject(renderMesh, s_RendererAPI);
    object->perDrawConstantBuffer = s_PerDrawGlobalConstantBuffer;
    s_RenderObjects.push_back(object);
}

void PGRenderer::EndScene() {
    // TODO: sorting, bathcing, etc
    s_SceneRenderPass.AddRenderObjects(s_RenderObjects);
}

