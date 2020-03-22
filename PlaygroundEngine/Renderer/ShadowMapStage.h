#pragma once

#include "PGShaderLib.h"
#include "SceneRenderPass.h"

class ShadowGenStage {
public:
    void Initialize(HWRendererAPI* rendererAPI);
    void Execute(HWRendererAPI* rendererAPI, PGRenderView renderViews[MAX_SHADOW_CASCADE_COUNT], const PGRendererConfig& rendererConfig, bool clear = true);

    void SetShadowMapTarget(HWRendererAPI* rendererAPI, HWTexture2D* shadowMapTexture, const PGRendererConfig& rendererConfig);

private:
    HWDepthStencilView* m_ShadowMapTargets[MAX_SHADOW_CASCADE_COUNT] = {0};

    HWViewport m_ShadowMapViewport;
};

