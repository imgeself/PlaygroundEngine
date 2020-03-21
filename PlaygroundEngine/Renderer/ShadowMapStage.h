#pragma once

#include "PGShaderLib.h"
#include "SceneRenderPass.h"

class ShadowGenStage {
public:
    void Initialize(HWRendererAPI* rendererAPI, GPUResource* shadowMapTarget, size_t shadowMapSize);
    void Execute(HWRendererAPI* rendererAPI, const RenderList& shadowCasterList, PGShaderLib* shaderLib, const PGRendererConfig& rendererConfig, bool clear = true);

private:
    HWBuffer* m_PerShadowGenConstantBuffer;

    HWDepthStencilView* m_ShadowMapTarget;
    HWViewport m_ShadowMapViewport;
};

