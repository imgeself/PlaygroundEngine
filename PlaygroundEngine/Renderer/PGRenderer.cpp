#include "PGRenderer.h"

#include "DX11/DX11RendererAPI.h"

#include <memory>

HWRendererAPI* PGRenderer::s_RendererAPI = nullptr;
PGShaderLib* PGRenderer::s_ShaderLib = nullptr;
std::unordered_set<Mesh*> PGRenderer::s_RenderObjects = std::unordered_set<Mesh*>();
PGScene* PGRenderer::s_ActiveSceneData;

void PGRenderer::Uninitialize() {
    delete s_RendererAPI;
    delete s_ShaderLib;
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
    s_RendererAPI->Present();
}

void PGRenderer::BeginScene(PGScene* sceneData) {
    s_ActiveSceneData = sceneData;
}

void PGRenderer::AddRenderObject(Mesh* renderMesh) {
    s_RenderObjects.insert(renderMesh);
}

void PGRenderer::EndScene() {
    //TODO: This function should be create gpu commands and pass them to render thread.
    s_ShaderLib->ReloadShadersIfNeeded();
    for (Mesh* renderObject : s_RenderObjects) {
        size_t vertexBufferStride = sizeof(Vertex);
        size_t vertexBufferSize = sizeof(Vertex) * renderObject->vertices.size();
        std::shared_ptr<HWVertexBuffer>vertexBuffer(s_RendererAPI->CreateVertexBuffer(renderObject->vertices.data(), vertexBufferSize, vertexBufferStride));

        size_t indicesCount = renderObject->indices.size();
        std::shared_ptr<HWIndexBuffer> indexBuffer(s_RendererAPI->CreateIndexBuffer(renderObject->indices.data(), indicesCount));

        PGShader* shader = renderObject->material.shader;
        HWShaderProgram* hwShader = shader->GetHWShader();

        //TODO: Do we want fixed input elements for all shaders?
        std::vector<VertexInputElement> inputElements = {
            { "Position", VertexDataFormat_FLOAT3, 0, 0 },
            { "Normal", VertexDataFormat_FLOAT3, 0, 12 }
        };

        std::shared_ptr<HWVertexInputLayout> inputLayout(s_RendererAPI->CreateVertexInputLayout(inputElements, hwShader));


        // Bindings
        s_RendererAPI->SetVertexBuffer(vertexBuffer.get(), vertexBufferStride);
        s_RendererAPI->SetIndexBuffer(indexBuffer.get());
        s_RendererAPI->SetInputLayout(inputLayout.get());
        s_RendererAPI->SetShaderProgram(hwShader);

        /*
        PerFrameData constantBuffer = {};
        constantBuffer.modelMatrix = renderObject->transform.GetTransformMatrix();
        constantBuffer.viewMatrix = s_ActiveSceneData->camera->GetViewMatrix();
        constantBuffer.projMatrix = s_ActiveSceneData->camera->GetProjectionMatrix();
        constantBuffer.lightPos = Vector4(s_ActiveSceneData->light->position, 1.0f);
        constantBuffer.cameraPos = Vector4(s_ActiveSceneData->camera->GetPosition(), 1.0f);

        //IConstantBuffer* vsConstantBuffer = s_RendererAPI->CreateConstantBuffer(&constantBuffer, sizeof(PerFrameData));
        */

        shader->SetHWConstantBufers(s_RendererAPI);

        s_RendererAPI->DrawIndexed(indexBuffer.get());
    }

}

