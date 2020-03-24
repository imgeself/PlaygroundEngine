#pragma once

#include "../Core.h"
#include "HWRendererAPI.h"
#include "PGTexture.h"
#include "PGShaderLib.h"

class Skybox {
public:
    Skybox(HWRendererAPI* rendererAPI, PGShaderLib* shaderLib);
    ~Skybox();

    void RenderSkybox(HWRendererAPI* rendererAPI, PGTexture* skyboxCube);

private:
    HWBuffer* m_VertexBuffer;
    HWBuffer* m_IndexBuffer;
    HWGraphicsPipelineState* m_PipelineState;
};
