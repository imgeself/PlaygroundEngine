#include "ShadowMapStage.h"
#include "../PGProfiler.h"

void ShadowGenStage::Initialize(HWRendererAPI* rendererAPI, GPUResource* shadowMapTarget, size_t shadowMapSize) {
    m_ShadowMapViewport.topLeftX = 0.0f;
    m_ShadowMapViewport.topLeftY = 0.0f;
    m_ShadowMapViewport.width = (float) shadowMapSize;
    m_ShadowMapViewport.height = (float) shadowMapSize;

    m_ShadowMapTarget = shadowMapTarget->dsv;

    PerShadowGenConstantBuffer initData = {};
    m_PerShadowGenConstantBuffer = rendererAPI->CreateConstantBuffer(&initData, sizeof(PerShadowGenConstantBuffer), 
                                                                     HWResourceFlags::USAGE_DYNAMIC | HWResourceFlags::CPU_ACCESS_WRITE, "ShadowGenCB");
    rendererAPI->SetConstanBuffersVS(PER_SHADOWGEN_CBUFFER_SLOT, &m_PerShadowGenConstantBuffer, 1);
}

void ShadowGenStage::Execute(HWRendererAPI* rendererAPI, const RenderList& shadowCasterList, PGShaderLib* shaderLib) {
    PG_PROFILE_FUNCTION();
    rendererAPI->SetRenderTargets(nullptr, 0, m_ShadowMapTarget);
    rendererAPI->ClearDepthStencilView(m_ShadowMapTarget, false, 1.0f, 0);
    rendererAPI->SetViewport(&m_ShadowMapViewport);

    PGShader* shadowGenShader = shaderLib->GetDefaultShader("ShadowGen");
    rendererAPI->SetVertexShader(shadowGenShader->GetHWVertexShader());
    rendererAPI->SetPixelShader(nullptr); // We don't need a pixel shader in shadow generation stage

    for (uint32_t cascadeIndex = 0; cascadeIndex < CASCADE_COUNT; ++cascadeIndex) {
        void* gpuBuffer = rendererAPI->Map(m_PerShadowGenConstantBuffer);
        PerShadowGenConstantBuffer data = {};
        data.g_ShadowGenCascadeIndex = cascadeIndex;
        memcpy(gpuBuffer, &data, sizeof(PerShadowGenConstantBuffer));
        rendererAPI->Unmap(m_PerShadowGenConstantBuffer);

        RenderScene(rendererAPI, shadowCasterList, SceneRenderPassType::SHADOW_GEN);
    }
}



