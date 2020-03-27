#include "Skybox.h"
#include "../Assets/Shaders/ShaderDefinitions.h"
#include "../Math/math_vector.h"
#include "../PGProfiler.h"

const Vector3 vertices[] = {
    { -1, 1, 1 },
    { -1, 1, -1 },
    { -1, -1, -1 },
    { -1, -1, 1 },
    { -1, 1, -1 },
    { 1, 1, -1 },
    { 1, -1, -1 },
    { -1, -1, -1 },
    { 1, 1, -1 },
    { 1, 1, 1 },
    { 1, -1, 1 },
    { 1, -1, -1 },
    { 1, 1, 1 },
    { -1, 1, 1 },
    { -1, -1, 1 },
    { 1, -1, 1 },
    { -1, -1, 1 },
    { -1, -1, -1 },
    { 1, -1, -1 },
    { 1, -1, 1 },
    { 1, 1, 1 },
    { 1, 1, -1 },
    { -1, 1, -1 },
    { -1, 1, 1 }
};

const uint16_t indices[] = {
    0, 3, 1,
    1, 3, 2,
    4, 7, 5,
    5, 7, 6,
    8, 11, 9,
    9, 11, 10,
    12, 15, 13,
    13, 15, 14,
    16, 19, 17,
    17, 19, 18,
    20, 23, 21,
    21, 23, 22
};

Skybox::Skybox(HWRendererAPI* rendererAPI, PGShaderLib* shaderLib) {
    SubresourceData bufferSubresourceData = {};
    bufferSubresourceData.data = vertices;
    m_VertexBuffer = rendererAPI->CreateBuffer(&bufferSubresourceData, sizeof(vertices), HWResourceFlags::USAGE_IMMUTABLE | HWResourceFlags::BIND_VERTEX_BUFFER);

    bufferSubresourceData.data = indices;
    m_IndexBuffer = rendererAPI->CreateBuffer(&bufferSubresourceData, sizeof(indices), HWResourceFlags::USAGE_IMMUTABLE | HWResourceFlags::BIND_INDEX_BUFFER);

    PGShader* shader = shaderLib->GetDefaultShader("Skybox");
    HWVertexInputElement inputElements[] = {
        { "POSITION", 0, VertexDataFormat_FLOAT3, 0, PER_VERTEX_DATA, 0 },
    };
    HWInputLayoutDesc inputLayoutDesc = {};
    inputLayoutDesc.elements = inputElements;
    inputLayoutDesc.elementCount = ARRAYSIZE(inputElements);

    HWRasterizerDesc rasterizerDesc = {};
    rasterizerDesc.cullMode = CULL_NONE; // We need to disable back-face culling because we are rendering a cube from inside

    HWDepthStencilDesc depthStencilDesc = {};
    depthStencilDesc.depthFunc = COMPARISON_LESS_EQUAL; // Skybox z value is always 1.

    HWGraphicsPipelineStateDesc pipelineDesc;
    pipelineDesc.rasterizerDesc = rasterizerDesc;
    pipelineDesc.depthStencilDesc = depthStencilDesc;
    pipelineDesc.inputLayoutDesc = inputLayoutDesc;
    pipelineDesc.primitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    pipelineDesc.vertexShader = shader->GetVertexBytecode();
    pipelineDesc.pixelShader = shader->GetPixelBytecode();

    m_PipelineState = rendererAPI->CreateGraphicsPipelineState(pipelineDesc);

}

Skybox::~Skybox() {
    delete m_VertexBuffer;
    delete m_IndexBuffer;
    delete m_PipelineState;
}

void Skybox::RenderSkybox(HWRendererAPI* rendererAPI, PGTexture* skyboxCubemap) {
    PG_PROFILE_FUNCTION();
    rendererAPI->BeginEvent("SKYBOX");
    uint32_t offset = 0;
    uint32_t stride = sizeof(Vector3);
    rendererAPI->SetVertexBuffers(&m_VertexBuffer, 1, &stride, &offset);
    rendererAPI->SetIndexBuffer(m_IndexBuffer, sizeof(indices[0]), 0);
    rendererAPI->SetGraphicsPipelineState(m_PipelineState);

    HWShaderResourceView* skyboxView = skyboxCubemap->GetHWResourceView();
    rendererAPI->SetShaderResourcesPS(SKYBOX_TEXTURECUBE_SLOT, &skyboxView, 1);

    rendererAPI->DrawIndexed(ARRAYSIZE(indices), 0, 0);
    rendererAPI->EndEvent();
}
