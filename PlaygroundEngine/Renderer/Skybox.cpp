#include "Skybox.h"
#include "PGRenderer.h"
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

// Winding directions are reversed. Because we are rendering a cube from the inside.
// Back face culling will cull skybox. To be able to render skybox, we have to disable culling, or change culling face to front from back 
// or we can load hardcoded reversed-winding cube. 
// TODO: Disable backface culling instead of hardcoded reverse-winding cube.
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

Skybox::Skybox(PGTexture* skyboxCubemap) : m_SkyboxCubemap(skyboxCubemap) {
    HWRendererAPI* rendererAPI = PGRenderer::GetRendererAPI();
    PGShaderLib* shaderLib = PGRenderer::GetShaderLib();


    SubresourceData bufferSubresourceData = {};
    bufferSubresourceData.data = vertices;
    m_VertexBuffer = rendererAPI->CreateBuffer(&bufferSubresourceData, sizeof(vertices), HWResourceFlags::USAGE_IMMUTABLE | HWResourceFlags::BIND_VERTEX_BUFFER);

    bufferSubresourceData.data = indices;
    m_IndexBuffer = rendererAPI->CreateBuffer(&bufferSubresourceData, sizeof(indices), HWResourceFlags::USAGE_IMMUTABLE | HWResourceFlags::BIND_INDEX_BUFFER);

    m_Shader = shaderLib->GetDefaultShader("Skybox");

    std::vector<VertexInputElement> inputElements = {
        { "POSITION", 0, VertexDataFormat_FLOAT3, 0, PER_VERTEX_DATA, 0 },
    };

    m_VertexInputLayout = rendererAPI->CreateVertexInputLayout(inputElements, m_Shader->GetHWVertexShader());
}

Skybox::~Skybox() {
    delete m_VertexBuffer;
    delete m_IndexBuffer;
    delete m_VertexInputLayout;
}

void Skybox::RenderSkybox() {
    PG_PROFILE_FUNCTION();
    HWRendererAPI* rendererAPI = PGRenderer::GetRendererAPI();

    uint32_t offset = 0;
    uint32_t stride = sizeof(Vector3);
    rendererAPI->SetVertexBuffers(&m_VertexBuffer, 1, &stride, &offset);
    rendererAPI->SetIndexBuffer(m_IndexBuffer, sizeof(indices[0]), 0);
    rendererAPI->SetInputLayout(m_VertexInputLayout);
    rendererAPI->SetVertexShader(m_Shader->GetHWVertexShader());
    rendererAPI->SetPixelShader(m_Shader->GetHWPixelShader());

    HWShaderResourceView* skyboxView = m_SkyboxCubemap->GetHWResourceView();
    rendererAPI->SetShaderResourcesPS(SKYBOX_TEXTURECUBE_SLOT, &skyboxView, 1);

    rendererAPI->DrawIndexed(ARRAYSIZE(indices), 0, 0);
}
