#include "ShadowMapPass.h"

ShadowMapPass::ShadowMapPass() {

}

void ShadowMapPass::Initialize(HWRendererAPI* rendererAPI, PGShaderLib* shaderLib, size_t shadowMapSize) {
    Texture2DInitParams initParams = {};
    initParams.width = shadowMapSize;
    initParams.height = shadowMapSize;
    initParams.format = DXGI_FORMAT_R16_TYPELESS;
    initParams.sampleCount = 1;
    initParams.arraySize = CASCADE_COUNT;
    initParams.data = nullptr;
    initParams.flags = TextureResourceFlags::BIND_DEPTH_STENCIL | TextureResourceFlags::BIND_SHADER_RESOURCE;

    m_ShadowMapTexture = rendererAPI->CreateTexture2D(&initParams);
    for (uint32_t cascadeIndex = 0; cascadeIndex < CASCADE_COUNT; ++cascadeIndex) {
        m_DepthStencilViews[cascadeIndex] = rendererAPI->CreateDepthStencilView(m_ShadowMapTexture, cascadeIndex, 1);
    }
    m_ShadowMapRsourceView = rendererAPI->CreateShaderResourceView(m_ShadowMapTexture);
    delete m_ShadowMapTexture;

    m_Viewport.topLeftX = 0.0f;
    m_Viewport.topLeftY = 0.0f;
    m_Viewport.width = (float) shadowMapSize;
    m_Viewport.height = (float) shadowMapSize;

    m_ShadowGenShader = shaderLib->GetDefaultShader("ShadowGen");

    PerShadowGenConstantBuffer initData = {};
    m_PerShadowGenConstantBuffer = rendererAPI->CreateConstantBuffer(&initData, sizeof(PerShadowGenConstantBuffer));
    rendererAPI->SetConstanBuffersVS(PER_SHADOWGEN_CBUFFER_SLOT, &m_PerShadowGenConstantBuffer, 1);
    rendererAPI->SetConstanBuffersPS(PER_SHADOWGEN_CBUFFER_SLOT, &m_PerShadowGenConstantBuffer, 1);
}

ShadowMapPass::~ShadowMapPass() {
    delete m_ShadowMapRsourceView;
    for (size_t cascadeIndex = 0; cascadeIndex < CASCADE_COUNT; ++cascadeIndex) {
        delete m_DepthStencilViews[cascadeIndex];
    }
    delete m_PerShadowGenConstantBuffer;
}

void ShadowMapPass::Execute(HWRendererAPI* rendererAPI, const std::vector<PGRenderObject*>& renderObjects) {
    HWShaderProgram* hwShader = m_ShadowGenShader->GetHWShader();
    rendererAPI->SetShaderProgram(hwShader);
    rendererAPI->SetViewport(&m_Viewport);
    
    for (uint32_t cascadeIndex = 0; cascadeIndex < CASCADE_COUNT; ++cascadeIndex) {
        HWDepthStencilView* depthStencilView = m_DepthStencilViews[cascadeIndex];
        rendererAPI->SetRenderTargets(nullptr, 0, depthStencilView);
        rendererAPI->ClearDepthStencilView(depthStencilView, false, 1.0f, 0);

        void* gpuBuffer = rendererAPI->Map(m_PerShadowGenConstantBuffer);
        memcpy(gpuBuffer, &cascadeIndex, sizeof(uint32_t));
        rendererAPI->Unmap(m_PerShadowGenConstantBuffer);

        for (PGRenderObject* renderObject : renderObjects) {
            renderObject->UpdatePerDrawConstantBuffer(rendererAPI);

            size_t vertexBufferStride = sizeof(Vertex);
            rendererAPI->SetVertexBuffer(renderObject->vertexBuffer, vertexBufferStride);
            rendererAPI->SetIndexBuffer(renderObject->indexBuffer);
            rendererAPI->SetInputLayout(renderObject->inputLayout);
            if (renderObject->indexBuffer) {
                rendererAPI->DrawIndexed(renderObject->indexBuffer);
            }
            else {
                rendererAPI->Draw(renderObject->vertexBuffer);
            }
        }
    }
}



