#pragma once

#include "../Core.h"
#include "HWRendererAPI.h"
#include "PGTexture.h"
#include "PGShader.h"

class PG_API Skybox {
public:
    Skybox(PGTexture* skyboxCube);
    ~Skybox();

    void RenderSkybox();

private:
    HWVertexBuffer* m_VertexBuffer;
    HWIndexBuffer* m_IndexBuffer;
    HWVertexInputLayout* m_VertexInputLayout;
    PGShader* m_Shader;
    PGTexture* m_SkyboxCubemap;
};
