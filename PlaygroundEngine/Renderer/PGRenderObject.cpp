#include "PGRenderObject.h"

PGRenderObject::PGRenderObject(Mesh* mesh, HWRendererAPI* rendererAPI)
    : mesh(mesh) {
    size_t vertexBufferStride = sizeof(Vertex);
    size_t vertexBufferSize = sizeof(Vertex) * mesh->vertices.size();
    vertexBuffer = rendererAPI->CreateVertexBuffer(mesh->vertices.data(), vertexBufferSize, vertexBufferStride);

    size_t indicesCount = mesh->indices.size();
    indexBuffer= rendererAPI->CreateIndexBuffer(mesh->indices.data(), indicesCount);

    shader = mesh->material.shader;
    HWShaderProgram* hwShader = shader->GetHWShader();

    //TODO: Do we want fixed input elements for all shaders?
    std::vector<VertexInputElement> inputElements = {
        { "Position", VertexDataFormat_FLOAT3, 0, 0 },
        { "Normal", VertexDataFormat_FLOAT3, 0, 12 }
    };

    inputLayout = rendererAPI->CreateVertexInputLayout(inputElements, hwShader);
}

PGRenderObject::~PGRenderObject() {
    delete vertexBuffer;
    delete inputLayout;
    delete indexBuffer;
}

void PGRenderObject::UpdatePerDrawConstantBuffer(HWRendererAPI* rendererAPI) {
    // Update transform data
    PerDrawGlobalConstantBuffer perDrawGlobalConstantBuffer = {};
    perDrawGlobalConstantBuffer.modelMatrix = mesh->transform.GetTransformMatrix();

    void* data = rendererAPI->Map(perDrawConstantBuffer);
    memcpy(data, &perDrawGlobalConstantBuffer, sizeof(PerDrawGlobalConstantBuffer));
    rendererAPI->Unmap(perDrawConstantBuffer);
}

