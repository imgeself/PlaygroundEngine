#include "ShadowMapStage.h"
#include "../PGProfiler.h"

void ShadowGenStage::Initialize(HWRendererAPI* rendererAPI) {

}

void ShadowGenStage::Execute(HWRendererAPI* rendererAPI, PGRenderView renderViews[MAX_SHADOW_CASCADE_COUNT], const PGRendererConfig& rendererConfig, bool clear) {
    PG_PROFILE_FUNCTION();

    const uint8_t cascadeCount = rendererConfig.shadowCascadeCount;
    rendererAPI->SetViewport(&m_ShadowMapViewport);
    for (uint8_t cascadeIndex = 0; cascadeIndex < cascadeCount; ++cascadeIndex) {
        PG_PROFILE_SCOPE("ShadowGen Pass");
        rendererAPI->SetRenderTargets(nullptr, 0, m_ShadowMapTargets[cascadeIndex]);
        if (clear) {
            rendererAPI->ClearDepthStencilView(m_ShadowMapTargets[cascadeIndex], false, 1.0f, 0);
        }

        PGRenderView& renderView = renderViews[cascadeIndex];
        renderView.UpdatePerViewConstantBuffer(rendererAPI);
        RenderScene(rendererAPI, renderView.renderList, SceneRenderPassType::DEPTH_PASS);
    }
}

void ShadowGenStage::SetShadowMapTarget(HWRendererAPI* rendererAPI, HWTexture2D* shadowMapTexture, const PGRendererConfig& rendererConfig) {
    m_ShadowMapViewport.topLeftX = 0.0f;
    m_ShadowMapViewport.topLeftY = 0.0f;
    m_ShadowMapViewport.width = (float)rendererConfig.shadowMapSize;
    m_ShadowMapViewport.height = (float)rendererConfig.shadowMapSize;

    for (uint8_t cascadeIndex = 0; cascadeIndex < MAX_SHADOW_CASCADE_COUNT; ++cascadeIndex) {
        SAFE_DELETE(m_ShadowMapTargets[cascadeIndex]);
    }
    for (uint8_t cascadeIndex = 0; cascadeIndex < rendererConfig.shadowCascadeCount; ++cascadeIndex) {
        m_ShadowMapTargets[cascadeIndex] = rendererAPI->CreateDepthStencilView(shadowMapTexture, cascadeIndex, 1);
    }
}

