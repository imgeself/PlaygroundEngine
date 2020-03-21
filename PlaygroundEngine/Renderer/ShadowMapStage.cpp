#include "ShadowMapStage.h"
#include "../PGProfiler.h"

void ShadowGenStage::Initialize(HWRendererAPI* rendererAPI, GPUResource* shadowMapTarget, size_t shadowMapSize) {
    m_ShadowMapViewport.topLeftX = 0.0f;
    m_ShadowMapViewport.topLeftY = 0.0f;
    m_ShadowMapViewport.width = (float) shadowMapSize;
    m_ShadowMapViewport.height = (float) shadowMapSize;

    m_ShadowMapTarget = shadowMapTarget->dsv;

    uint32_t flags = HWResourceFlags::USAGE_DYNAMIC | HWResourceFlags::CPU_ACCESS_WRITE | HWResourceFlags::BIND_CONSTANT_BUFFER;
    m_PerShadowGenConstantBuffer = rendererAPI->CreateBuffer(nullptr, sizeof(PerShadowGenConstantBuffer), flags, "ShadowGenCB");
    rendererAPI->SetConstantBuffersVS(PER_SHADOWGEN_CBUFFER_SLOT, &m_PerShadowGenConstantBuffer, 1);
}

void ShadowGenStage::Execute(HWRendererAPI* rendererAPI, const RenderList& shadowCasterList, PGShaderLib* shaderLib, const PGRendererConfig& rendererConfig, bool clear) {
    PG_PROFILE_FUNCTION();
    if (clear) {
        rendererAPI->SetRenderTargets(nullptr, 0, m_ShadowMapTarget);
        rendererAPI->ClearDepthStencilView(m_ShadowMapTarget, false, 1.0f, 0);
        rendererAPI->SetViewport(&m_ShadowMapViewport);
    }

    const uint8_t cascadeCount = rendererConfig.shadowCascadeCount;
    for (uint8_t cascadeIndex = 0; cascadeIndex < cascadeCount; ++cascadeIndex) {
        void* gpuBuffer = rendererAPI->Map(m_PerShadowGenConstantBuffer);
        PerShadowGenConstantBuffer data = {};
        data.g_ShadowGenCascadeIndex = cascadeIndex;
        memcpy(gpuBuffer, &data, sizeof(PerShadowGenConstantBuffer));
        rendererAPI->Unmap(m_PerShadowGenConstantBuffer);

        RenderScene(rendererAPI, shadowCasterList, SceneRenderPassType::DEPTH_PASS);
    }
}



