#include "PGRenderer.h"

#include "DX11/DX11RendererAPI.h"

#include <memory>

HWRendererAPI* PGRenderer::s_RendererAPI = nullptr;
PGShaderLib* PGRenderer::s_ShaderLib = nullptr;
std::vector<PGRenderObject*> PGRenderer::s_RenderObjects = std::vector<PGRenderObject*>();
PGScene* PGRenderer::s_ActiveSceneData = nullptr;

HWConstantBuffer* PGRenderer::s_PerFrameGlobalConstantBuffer = nullptr;


void PGRenderer::Uninitialize() {
    delete s_PerFrameGlobalConstantBuffer;
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

    PerFrameGlobalConstantBuffer perFrameGlobalConstantBuffer = {};
    s_PerFrameGlobalConstantBuffer = s_RendererAPI->CreateConstantBuffer(&perFrameGlobalConstantBuffer, sizeof(PerFrameGlobalConstantBuffer));
    return true;
}

void PGRenderer::BeginFrame() {
    const float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    s_RendererAPI->ClearScreen(color);
}

void PGRenderer::RenderFrame() {
    //TODO: This function should be create gpu commands and pass them to render thread.

    PerFrameGlobalConstantBuffer perFrameGlobalConstantBuffer = {};
    perFrameGlobalConstantBuffer.viewMatrix = s_ActiveSceneData->camera->GetViewMatrix();
    perFrameGlobalConstantBuffer.projMatrix = s_ActiveSceneData->camera->GetProjectionMatrix();
    perFrameGlobalConstantBuffer.lightPos = Vector4(s_ActiveSceneData->light->position, 1.0f);
    perFrameGlobalConstantBuffer.cameraPos = Vector4(s_ActiveSceneData->camera->GetPosition(), 1.0f);

    void* data = s_RendererAPI->Map(s_PerFrameGlobalConstantBuffer);
    memcpy(data, &perFrameGlobalConstantBuffer, sizeof(PerFrameGlobalConstantBuffer));
    s_RendererAPI->Unmap(s_PerFrameGlobalConstantBuffer);

    s_ShaderLib->ReloadShadersIfNeeded();
    for (PGRenderObject* renderObject : s_RenderObjects) {
        HWConstantBuffer* perDrawConstantBuffer = renderObject->UpdatePerDrawConstantBuffer(s_RendererAPI);

        // TODO: We might need some sort of ConstantManager class 
        renderObject->GetShader()->SetSystemConstantBuffer(perDrawConstantBuffer, SystemConstantBufferSlot_PerDraw);
        renderObject->GetShader()->SetSystemConstantBuffer(s_PerFrameGlobalConstantBuffer, SystemConstantBufferSlot_PerFrame);
        renderObject->Render(s_RendererAPI);
    }
}

void PGRenderer::EndFrame() {
    s_RendererAPI->Present();
}

void PGRenderer::BeginScene(PGScene* sceneData) {
    s_ActiveSceneData = sceneData;
}

void PGRenderer::AddMesh(Mesh* renderMesh) {
    PGRenderObject* object = new PGRenderObject(renderMesh, s_RendererAPI);
    s_RenderObjects.push_back(object);
}

void PGRenderer::EndScene() {
    // TODO: sorting, bathcing, etc
}

