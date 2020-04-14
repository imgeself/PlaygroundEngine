#include "ShadowMapStage.h"
#include "../PGProfiler.h"

void ShadowGenStage::Initialize(HWRendererAPI* rendererAPI) {

}

void ShadowGenStage::Execute(HWRendererAPI* rendererAPI, PGRenderView renderViews[MAX_SHADOW_CASCADE_COUNT], const PGRendererConfig& rendererConfig, bool clear) {
    //PG_PROFILE_FUNCTION();
    rendererAPI->BeginEvent("SHADOW GEN");
    const uint8_t cascadeCount = rendererConfig.shadowCascadeCount;
    rendererAPI->SetViewport(&m_ShadowMapViewport);
    for (uint8_t cascadeIndex = 0; cascadeIndex < cascadeCount; ++cascadeIndex) {
        PG_PROFILE_SCOPE("ShadowGen Pass");
        rendererAPI->BeginEvent("SHADOW CASCADE GEN");
        rendererAPI->SetRenderTargets(nullptr, 0, m_ShadowMapTargets[cascadeIndex]);
        if (clear) {
            rendererAPI->ClearDepthStencilView(m_ShadowMapTargets[cascadeIndex], false, 1.0f, 0);
        }

        PGRenderView& renderView = renderViews[cascadeIndex];
        renderView.UpdatePerViewConstantBuffer(rendererAPI);
        RenderScene(rendererAPI, renderView.renderList, SceneRenderPassType::DEPTH_PASS);
        rendererAPI->EndEvent();
    }
    rendererAPI->EndEvent();
}

void ShadowGenStage::ExecutePointLightShadow(HWRendererAPI* rendererAPI, GPUResource* pointLightShadowTexture, 
                                             PGRenderView& renderView, const PGRendererConfig& rendererConfig, 
                                             uint32_t pointLightShadowIndex, uint32_t cubeMapFaceIndex, bool clear) {
    //PG_PROFILE_FUNCTION();
    rendererAPI->BeginEvent("POINT SHADOW GEN");

    HWViewport viewport = {};
    viewport.width = rendererConfig.pointLightShadowMapSize;
    viewport.height = rendererConfig.pointLightShadowMapSize;

    rendererAPI->SetViewport(&viewport);
    uint32_t shadowDSVIndex = pointLightShadowIndex * 6 + cubeMapFaceIndex;
    HWDepthStencilView* shadowMapDSV = m_PointShadowMapTargets[shadowDSVIndex];

    rendererAPI->SetRenderTargets(nullptr, 0, shadowMapDSV);
    if (clear) {
        rendererAPI->ClearDepthStencilView(shadowMapDSV, false, 1.0f, 0);
    }

    renderView.UpdatePerViewConstantBuffer(rendererAPI);
    RenderScene(rendererAPI, renderView.renderList, SceneRenderPassType::DEPTH_PASS);

    rendererAPI->EndEvent();
}

void ShadowGenStage::ExecuteSpotLightShadow(HWRendererAPI* rendererAPI, GPUResource* pointLightShadowTexture,
                                            PGRenderView& renderView, const PGRendererConfig& rendererConfig, 
                                            uint32_t spotLightShadowIndex, bool clear) {
    //PG_PROFILE_FUNCTION();
    rendererAPI->BeginEvent("SPOT SHADOW GEN");

    HWViewport viewport = {};
    viewport.width = rendererConfig.spotLightShadowMapSize;
    viewport.height = rendererConfig.spotLightShadowMapSize;

    rendererAPI->SetViewport(&viewport);
    uint32_t shadowDSVIndex = spotLightShadowIndex;
    HWDepthStencilView* shadowMapDSV = m_SpotShadowMapTargets[shadowDSVIndex];

    rendererAPI->SetRenderTargets(nullptr, 0, shadowMapDSV);
    if (clear) {
        rendererAPI->ClearDepthStencilView(shadowMapDSV, false, 1.0f, 0);
    }

    renderView.UpdatePerViewConstantBuffer(rendererAPI);
    RenderScene(rendererAPI, renderView.renderList, SceneRenderPassType::DEPTH_PASS);

    rendererAPI->EndEvent();
}



void ShadowGenStage::SetShadowMapTarget(HWRendererAPI* rendererAPI, const PGRendererConfig& rendererConfig,
                                        HWTexture2D* directionalShadowMapTexture, HWTexture2D* pointShadowMapTexture, HWTexture2D* spotShadowMapTexture) {
    m_ShadowMapViewport.topLeftX = 0.0f;
    m_ShadowMapViewport.topLeftY = 0.0f;
    m_ShadowMapViewport.width = (float)rendererConfig.shadowMapSize;
    m_ShadowMapViewport.height = (float)rendererConfig.shadowMapSize;

    for (uint8_t cascadeIndex = 0; cascadeIndex < MAX_SHADOW_CASCADE_COUNT; ++cascadeIndex) {
        SAFE_DELETE(m_ShadowMapTargets[cascadeIndex]);
    }
    for (uint8_t cascadeIndex = 0; cascadeIndex < rendererConfig.shadowCascadeCount; ++cascadeIndex) {
        HWResourceViewDesc resourceViewDesc;
        resourceViewDesc.firstArraySlice = cascadeIndex;
        resourceViewDesc.sliceArrayCount = 1;
        resourceViewDesc.firstMip = 0;
        resourceViewDesc.mipCount = 1;
        m_ShadowMapTargets[cascadeIndex] = rendererAPI->CreateDepthStencilView(directionalShadowMapTexture, resourceViewDesc);
    }

    for (size_t pointLightDSVIndex = 0; pointLightDSVIndex < (MAX_POINT_LIGHT_COUNT * 6); ++pointLightDSVIndex) {
        SAFE_DELETE(m_PointShadowMapTargets[pointLightDSVIndex]);

        HWResourceViewDesc resourceViewDesc = {};
        resourceViewDesc.firstArraySlice = pointLightDSVIndex;
        resourceViewDesc.sliceArrayCount = 1;
        resourceViewDesc.firstMip = 0;
        resourceViewDesc.mipCount = 1;
        HWDepthStencilView* DSV = rendererAPI->CreateDepthStencilView(pointShadowMapTexture, resourceViewDesc);
        m_PointShadowMapTargets[pointLightDSVIndex] = DSV;
    }

    for (size_t spotLightDSVIndex = 0; spotLightDSVIndex < MAX_SPOT_LIGHT_COUNT; ++spotLightDSVIndex) {
        SAFE_DELETE(m_SpotShadowMapTargets[spotLightDSVIndex]);

        HWResourceViewDesc resourceViewDesc = {};
        resourceViewDesc.firstArraySlice = spotLightDSVIndex;
        resourceViewDesc.sliceArrayCount = 1;
        resourceViewDesc.firstMip = 0;
        resourceViewDesc.mipCount = 1;
        HWDepthStencilView* DSV = rendererAPI->CreateDepthStencilView(spotShadowMapTexture, resourceViewDesc);
        m_SpotShadowMapTargets[spotLightDSVIndex] = DSV;
    }
}

