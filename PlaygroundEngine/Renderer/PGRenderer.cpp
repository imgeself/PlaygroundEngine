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

        HWShaderProgram* shader = renderObject->material.shader;

        //TODO: Do we want fixed input elements for all shaders?
        std::vector<VertexInputElement> inputElements = {
            { "Position", VertexDataFormat_FLOAT3, 0, 0 },
            { "Normal", VertexDataFormat_FLOAT3, 0, 12 }
        };

        std::shared_ptr<HWVertexInputLayout> inputLayout(s_RendererAPI->CreateVertexInputLayout(inputElements, shader));


        // Bindings
        s_RendererAPI->SetVertexBuffer(vertexBuffer.get(), vertexBufferStride);
        s_RendererAPI->SetIndexBuffer(indexBuffer.get());
        s_RendererAPI->SetInputLayout(inputLayout.get());
        s_RendererAPI->SetShaderProgram(shader);

        /*
        PerFrameData constantBuffer = {};
        constantBuffer.modelMatrix = renderObject->transform.GetTransformMatrix();
        constantBuffer.viewMatrix = s_ActiveSceneData->camera->GetViewMatrix();
        constantBuffer.projMatrix = s_ActiveSceneData->camera->GetProjectionMatrix();
        constantBuffer.lightPos = Vector4(s_ActiveSceneData->light->position, 1.0f);
        constantBuffer.cameraPos = Vector4(s_ActiveSceneData->camera->GetPosition(), 1.0f);

        //IConstantBuffer* vsConstantBuffer = s_RendererAPI->CreateConstantBuffer(&constantBuffer, sizeof(PerFrameData));
        */

        ConstantBufferList vertexConstantBuffers = shader->GetVertexShaderConstantBuffers();
        ConstantBufferList pixelConstantBuffers = shader->GetPixelShaderConstantBuffers();

        if (vertexConstantBuffers.size() > 0) {
            HWConstantBuffer** vsConstantBuffers = (HWConstantBuffer**) alloca(sizeof(HWConstantBuffer*) * vertexConstantBuffers.size());
            for (size_t i = 0; i < vertexConstantBuffers.size(); ++i) {
                ConstantBufferResource* constantBuffer = vertexConstantBuffers[i];
                HWConstantBuffer* vsConstantBuffer = s_RendererAPI->CreateConstantBuffer(constantBuffer->GetData(), constantBuffer->GetSize());
                *(vsConstantBuffers + i) = vsConstantBuffer;
            }
            s_RendererAPI->SetConstanBuffersVS(vsConstantBuffers, vertexConstantBuffers.size());
            for (size_t i = 0; i < vertexConstantBuffers.size(); ++i) {
                delete* (vsConstantBuffers + i);
            }
        }

        if (pixelConstantBuffers.size() > 0) {
            HWConstantBuffer** psConstantBuffers = (HWConstantBuffer**) alloca(sizeof(HWConstantBuffer*) * pixelConstantBuffers.size());
            for (size_t i = 0; i < pixelConstantBuffers.size(); ++i) {
                ConstantBufferResource* constantBuffer = pixelConstantBuffers[i];
                HWConstantBuffer* vsConstantBuffer = s_RendererAPI->CreateConstantBuffer(constantBuffer->GetData(), constantBuffer->GetSize());
                *(psConstantBuffers + i) = vsConstantBuffer;
            }
            s_RendererAPI->SetConstanBuffersPS(psConstantBuffers, pixelConstantBuffers.size());
            for (size_t i = 0; i < pixelConstantBuffers.size(); ++i) {
                delete *(psConstantBuffers + i);
            }
        }

        s_RendererAPI->DrawIndexed(indexBuffer.get());
    }

}

