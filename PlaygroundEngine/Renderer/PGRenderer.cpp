#include "PGRenderer.h"

#include "DX11/DX11RendererAPI.h"

#include <memory>

HWRendererAPI* PGRenderer::s_RendererAPI = nullptr;
PGShaderLib* PGRenderer::s_ShaderLib = nullptr;
std::vector<PGRenderObject*> PGRenderer::s_RenderObjects = std::vector<PGRenderObject*>();
PGScene* PGRenderer::s_ActiveSceneData;

void PGRenderer::Uninitialize() {
    delete s_RendererAPI;
    delete s_ShaderLib;
    for (PGRenderObject* renderObject : s_RenderObjects) {
        delete renderObject;
    }
}

bool PGRenderer::Initialize(PGWindow* window) {
    s_RendererAPI = new DX11RendererAPI(window);
    s_ShaderLib = new PGShaderLib(s_RendererAPI);
    s_ShaderLib->LoadDefaultShaders();
    return true;
}

void PGRenderer::BeginFrame() {
    const float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    s_RendererAPI->ClearScreen(color);
}

void PGRenderer::EndFrame() {
    //TODO: This function should be create gpu commands and pass them to render thread.
    s_ShaderLib->ReloadShadersIfNeeded();
    for (PGRenderObject* renderObject : s_RenderObjects) {
        renderObject->Render(s_RendererAPI);
    }

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

