#include "Skybox.h"
#include "PGRenderer.h"


Skybox::Skybox(PGTexture* skyboxCubemap) : m_SkyboxCubemap(skyboxCubemap) {
    HWRendererAPI* rendererAPI = PGRenderer::GetRendererAPI();
    PGShaderLib* shaderLib = PGRenderer::GetShaderLib();

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
    uint32_t indices[] = {
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

    m_VertexBuffer = rendererAPI->CreateVertexBuffer((void*)vertices, sizeof(vertices), sizeof(Vector3));
    m_IndexBuffer = rendererAPI->CreateIndexBuffer(indices, ARRAYSIZE(indices));

    m_Shader = shaderLib->GetDefaultShader("Skybox");

    std::vector<VertexInputElement> inputElements = {
        { "POSITION", VertexDataFormat_FLOAT3, 0, 0 },
    };

    m_VertexInputLayout = rendererAPI->CreateVertexInputLayout(inputElements, m_Shader->GetHWShader());
}

Skybox::~Skybox() {
    delete m_VertexBuffer;
    delete m_IndexBuffer;
    delete m_VertexInputLayout;
}

void Skybox::RenderSkybox() {
    HWRendererAPI* rendererAPI = PGRenderer::GetRendererAPI();

    rendererAPI->SetVertexBuffer(m_VertexBuffer, sizeof(Vector3));
    rendererAPI->SetIndexBuffer(m_IndexBuffer);
    rendererAPI->SetInputLayout(m_VertexInputLayout);
    rendererAPI->SetShaderProgram(m_Shader->GetHWShader());

    HWShaderResourceView* skyboxView = m_SkyboxCubemap->GetHWResourceView();
    rendererAPI->SetShaderResourcesPS(SKYBOX_TEXTURECUBE_SLOT, &skyboxView, 1);

    rendererAPI->DrawIndexed(m_IndexBuffer);
}
