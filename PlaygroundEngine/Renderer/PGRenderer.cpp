#include "PGRenderer.h"

#include "DX11/DX11RendererAPI.h"

#include <memory>

IRendererAPI* PGRenderer::m_RendererAPI = nullptr;
std::unordered_set<Mesh*> PGRenderer::m_RenderObjects = std::unordered_set<Mesh*>();
PGScene* PGRenderer::m_ActiveSceneData;

PGRenderer::~PGRenderer() {
    if (m_RendererAPI) {
        delete m_RendererAPI;
    }
}

bool PGRenderer::Initialize(PGWindow* window) {
    m_RendererAPI = new DX11RendererAPI(window);

    return true;
}

void PGRenderer::BeginFrame() {
    const float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    m_RendererAPI->ClearScreen(color);
}

void PGRenderer::EndFrame() {
    m_RendererAPI->Present();
}

void PGRenderer::BeginScene(PGScene* sceneData) {
    m_ActiveSceneData = sceneData;
}

void PGRenderer::AddRenderObject(Mesh* renderMesh) {
    m_RenderObjects.insert(renderMesh);
}

void PGRenderer::EndScene() {
    //TODO: This function should be create gpu commands and pass them to render thread.
    for (Mesh* renderObject : m_RenderObjects) {
        size_t vertexBufferStride = sizeof(Vertex);
        size_t vertexBufferSize = sizeof(Vertex) * renderObject->vertices.size();
        std::shared_ptr<IVertexBuffer>vertexBuffer(m_RendererAPI->CreateVertexBuffer(renderObject->vertices.data(), vertexBufferSize, vertexBufferStride));

        uint32_t indicesCount = renderObject->indices.size();
        std::shared_ptr<IIndexBuffer> indexBuffer(m_RendererAPI->CreateIndexBuffer(renderObject->indices.data(), indicesCount));

        IShaderProgram* shader = renderObject->material.shader;

        //TODO: Do we want fixed input elements for all shaders?
        std::vector<VertexInputElement> inputElements = {
            { "Position", VertexDataFormat_FLOAT3, 0, 0 },
            { "Normal", VertexDataFormat_FLOAT3, 0, 12 }
        };

        std::shared_ptr<IVertexInputLayout> inputLayout(m_RendererAPI->CreateVertexInputLayout(inputElements, shader));


        // Bindings
        m_RendererAPI->SetVertexBuffer(vertexBuffer.get(), vertexBufferStride);
        m_RendererAPI->SetIndexBuffer(indexBuffer.get());
        m_RendererAPI->SetInputLayout(inputLayout.get());
        m_RendererAPI->SetShaderProgram(shader);

        // TODO: Uniform data should be constructed by parsing shader files.
        struct EngineConstantBuffer {
            Matrix4 modelMatrix;
            Matrix4 viewMatrix;
            Matrix4 projMatrix;
            Vector4 lightPos;
            Vector4 cameraPos;
        };

        EngineConstantBuffer constantBuffer = {};
        constantBuffer.modelMatrix = renderObject->transform.GetTransformMatrix();
        constantBuffer.viewMatrix = m_ActiveSceneData->camera->GetViewMatrix();
        constantBuffer.projMatrix = m_ActiveSceneData->camera->GetProjectionMatrix();
        constantBuffer.lightPos = Vector4(m_ActiveSceneData->light->position, 1.0f);
        constantBuffer.cameraPos = Vector4(m_ActiveSceneData->camera->GetPosition(), 1.0f);

        std::shared_ptr<IConstantBuffer> vsConstantBuffer(m_RendererAPI->CreateConstantBuffer(&constantBuffer, sizeof(EngineConstantBuffer)));
        m_RendererAPI->SetConstanBufferVS(vsConstantBuffer.get());
        m_RendererAPI->DrawIndexed(indexBuffer.get());
    }

}

