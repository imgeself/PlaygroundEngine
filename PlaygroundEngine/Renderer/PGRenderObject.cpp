#include "PGRenderObject.h"

PGRenderObject::PGRenderObject(const MeshRef& mesh, HWRendererAPI* rendererAPI)
    : mesh(mesh) {
    size_t vertexBufferSize = sizeof(Vertex) * mesh->vertices.size();
    vertexBuffer = rendererAPI->CreateVertexBuffer(mesh->vertices.data(), vertexBufferSize, HWResourceFlags::USAGE_IMMUTABLE);

    size_t indicesCount = mesh->indices.size();
    indexBuffer= rendererAPI->CreateIndexBuffer(mesh->indices.data(), indicesCount, HWResourceFlags::USAGE_IMMUTABLE);

    shader = mesh->material->shader;
    HWVertexShader* hwVertexShader = shader->GetHWVertexShader();

    //TODO: Do we want fixed input elements for all shaders?
    std::vector<VertexInputElement> inputElements = {
        { "POSITION", 0, VertexDataFormat_FLOAT3, 0, PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, VertexDataFormat_FLOAT3, 0, PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, VertexDataFormat_FLOAT2, 0, PER_VERTEX_DATA, 0 }
    };

    inputLayout = rendererAPI->CreateVertexInputLayout(inputElements, hwVertexShader);
}

PGRenderObject::~PGRenderObject() {
    delete vertexBuffer;
    delete inputLayout;
    delete indexBuffer;
}

void PGRenderObject::UpdatePerDrawConstantBuffer(HWRendererAPI* rendererAPI) {
    // Update transform data
    PerDrawGlobalConstantBuffer perDrawGlobalConstantBuffer = {};
    perDrawGlobalConstantBuffer.g_ModelMatrix = mesh->transform.GetTransformMatrix();
    memcpy(&perDrawGlobalConstantBuffer.g_Material, mesh->material, sizeof(DrawMaterial));

    void* data = rendererAPI->Map(perDrawConstantBuffer);
    memcpy(data, &perDrawGlobalConstantBuffer, sizeof(PerDrawGlobalConstantBuffer));
    rendererAPI->Unmap(perDrawConstantBuffer);
}

