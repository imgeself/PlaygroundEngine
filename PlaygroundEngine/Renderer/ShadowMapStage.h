#pragma once

#include "PGShaderLib.h"
#include "SceneRenderPass.h"

class ShadowGenStage {
public:
    void Initialize(HWRendererAPI* rendererAPI);
    void Execute(HWRendererAPI* rendererAPI, const RenderList& shadowCasterList, PGShaderLib* shaderLib, const PGRendererConfig& rendererConfig, bool clear = true);

    inline void SetShadowMapTarget(GPUResource* shadowMapTarget, size_t shadowMapSize) {
        m_ShadowMapViewport.topLeftX = 0.0f;
        m_ShadowMapViewport.topLeftY = 0.0f;
        m_ShadowMapViewport.width = (float)shadowMapSize;
        m_ShadowMapViewport.height = (float)shadowMapSize;

        m_ShadowMapTarget = shadowMapTarget->dsv;
    }

private:
    HWBuffer* m_PerShadowGenConstantBuffer;

    HWDepthStencilView* m_ShadowMapTarget;
    HWViewport m_ShadowMapViewport;
};

