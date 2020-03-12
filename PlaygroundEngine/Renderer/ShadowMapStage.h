#pragma once

#include "PGShaderLib.h"
#include "SceneRenderPass.h"

const uint32_t CASCADE_COUNT = 4;

class ShadowGenStage {
public:
    void Initialize(HWRendererAPI* rendererAPI, GPUResource* shadowMapTarget, size_t shadowMapSize);
    void Execute(HWRendererAPI* rendererAPI, const RenderList& shadowCasterList, PGShaderLib* shaderLib, bool clear = true);

private:
    HWBuffer* m_PerShadowGenConstantBuffer;

    HWDepthStencilView* m_ShadowMapTarget;
    HWViewport m_ShadowMapViewport;
};

