#pragma once

#include "PGShaderLib.h"
#include "SceneRenderPass.h"

class ShadowGenStage {
public:
    void Initialize(HWRendererAPI* rendererAPI);
    void Execute(HWRendererAPI* rendererAPI, PGRenderView renderViews[MAX_SHADOW_CASCADE_COUNT], const PGRendererConfig& rendererConfig, bool clear = true);
    void ExecutePointLightShadow(HWRendererAPI* rendererAPI, GPUResource* pointLightShadowTexture, PGRenderView renderView, const PGRendererConfig& rendererConfig, uint32_t pointLightShadowIndex, uint32_t cubeMapFaceIndex, bool clear = true);


    void SetShadowMapTarget(HWRendererAPI* rendererAPI, HWTexture2D* shadowMapTexture, const PGRendererConfig& rendererConfig);

private:
    HWDepthStencilView* m_ShadowMapTargets[MAX_SHADOW_CASCADE_COUNT] = {0};

    HWDepthStencilView* m_PointShadowMapTargets[MAX_POINT_LIGHT_COUNT * 6] = {0};
    HWDepthStencilView* m_SpotShadowMapTargets[MAX_SPOT_LIGHT_COUNT] = {0};

    HWViewport m_ShadowMapViewport;
};

