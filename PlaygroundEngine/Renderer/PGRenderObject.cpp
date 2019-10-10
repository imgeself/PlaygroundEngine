#include "PGRenderObject.h"

PGRenderObject::PGRenderObject(Mesh* mesh, HWRendererAPI* rendererAPI)
    : m_Mesh(mesh) {
    size_t vertexBufferStride = sizeof(Vertex);
    size_t vertexBufferSize = sizeof(Vertex) * mesh->vertices.size();
    m_VertexBuffer = rendererAPI->CreateVertexBuffer(mesh->vertices.data(), vertexBufferSize, vertexBufferStride);

    size_t indicesCount = mesh->indices.size();
    m_IndexBuffer= rendererAPI->CreateIndexBuffer(mesh->indices.data(), indicesCount);

    m_Shader = mesh->material.shader;
    HWShaderProgram* hwShader = m_Shader->GetHWShader();

    //TODO: Do we want fixed input elements for all shaders?
    std::vector<VertexInputElement> inputElements = {
        { "Position", VertexDataFormat_FLOAT3, 0, 0 },
        { "Normal", VertexDataFormat_FLOAT3, 0, 12 }
    };

    m_InputLayout = rendererAPI->CreateVertexInputLayout(inputElements, hwShader);

    // Create per view constant buffer
    PerDrawGlobalConstantBuffer perDrawGlobalConstantBuffer = {};
    perDrawGlobalConstantBuffer.modelMatrix = mesh->transform.GetTransformMatrix();
    m_PerDrawConstantBuffer = rendererAPI->CreateConstantBuffer(&perDrawGlobalConstantBuffer, sizeof(PerDrawGlobalConstantBuffer));
}

PGRenderObject::~PGRenderObject() {
    delete m_VertexBuffer;
    delete m_InputLayout;
    delete m_IndexBuffer;
    delete m_PerDrawConstantBuffer;
}

HWConstantBuffer* PGRenderObject::UpdatePerDrawConstantBuffer(HWRendererAPI* rendererAPI) {
    // Update transform data
    PerDrawGlobalConstantBuffer perDrawGlobalConstantBuffer = {};
    perDrawGlobalConstantBuffer.modelMatrix = m_Mesh->transform.GetTransformMatrix();

    void* data = rendererAPI->Map(m_PerDrawConstantBuffer);
    memcpy(data, &perDrawGlobalConstantBuffer, sizeof(PerDrawGlobalConstantBuffer));
    rendererAPI->Unmap(m_PerDrawConstantBuffer);
    return m_PerDrawConstantBuffer;
}


void PGRenderObject::Render(HWRendererAPI* rendererAPI) {
    size_t vertexBufferStride = sizeof(Vertex);
    rendererAPI->SetVertexBuffer(m_VertexBuffer, vertexBufferStride);
    rendererAPI->SetIndexBuffer(m_IndexBuffer);
    rendererAPI->SetInputLayout(m_InputLayout);
    rendererAPI->SetShaderProgram(m_Shader->GetHWShader());
    m_Shader->SetHWConstantBufers(rendererAPI);
    rendererAPI->DrawIndexed(m_IndexBuffer);
}

