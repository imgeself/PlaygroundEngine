#include "ShadowMapStage.h"
#include "../PGProfiler.h"

void ShadowGenStage::Initialize(HWRendererAPI* rendererAPI, PGShaderLib* shaderLib, size_t shadowMapSize) {
    HWViewport shadowViewport;
    shadowViewport.topLeftX = 0.0f;
    shadowViewport.topLeftY = 0.0f;
    shadowViewport.width = (float) shadowMapSize;
    shadowViewport.height = (float) shadowMapSize;

    PGShader* shadowGenShader = shaderLib->GetDefaultShader("ShadowGen");
    
    // Init pass
    m_ShadowGenPass.SetShader(shadowGenShader);
    m_ShadowGenPass.SetViewport(shadowViewport);

    PerShadowGenConstantBuffer initData = {};
    m_PerShadowGenConstantBuffer = rendererAPI->CreateConstantBuffer(&initData, sizeof(PerShadowGenConstantBuffer), 
                                                                     HWResourceFlags::USAGE_DYNAMIC | HWResourceFlags::CPU_ACCESS_WRITE, "ShadowGenCB");
    rendererAPI->SetConstanBuffersVS(PER_SHADOWGEN_CBUFFER_SLOT, &m_PerShadowGenConstantBuffer, 1);
}

void ShadowGenStage::Execute(HWRendererAPI* rendererAPI, const std::vector<PGRenderObject*>& shadowCasters) {
    PG_PROFILE_FUNCTION();
    m_ShadowGenPass.SetRenderObjects(shadowCasters);
    rendererAPI->SetRenderTargets(nullptr, 0, m_DepthStencilView);
    rendererAPI->ClearDepthStencilView(m_DepthStencilView, false, 1.0f, 0);
    rendererAPI->SetViewport(&m_ShadowGenPass.m_Viewport);
    rendererAPI->SetVertexShader(m_ShadowGenPass.m_Shader->GetHWVertexShader());
    rendererAPI->SetPixelShader(nullptr); // We don't need a pixel shader in shadow generation stage
    for (uint32_t cascadeIndex = 0; cascadeIndex < CASCADE_COUNT; ++cascadeIndex) {
        void* gpuBuffer = rendererAPI->Map(m_PerShadowGenConstantBuffer);
        PerShadowGenConstantBuffer data = {};
        data.g_ShadowGenCascadeIndex = cascadeIndex;
        memcpy(gpuBuffer, &data, sizeof(PerShadowGenConstantBuffer));
        rendererAPI->Unmap(m_PerShadowGenConstantBuffer);

        m_ShadowGenPass.Execute(rendererAPI);
    }
}



