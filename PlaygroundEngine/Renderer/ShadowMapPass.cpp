#include "ShadowMapPass.h"

ShadowMapPass::ShadowMapPass() {

}

void ShadowMapPass::Initialize(HWRendererAPI* rendererAPI, PGShaderLib* shaderLib, size_t shadowMapWidth, size_t shadowMapHeight) {
    Texture2DInitParams initParams = {};
    initParams.width = shadowMapWidth;
    initParams.height = shadowMapHeight;
    initParams.format = DXGI_FORMAT_R16_TYPELESS;
    initParams.sampleCount = 1;
    initParams.data = nullptr;
    initParams.flags = TextureResourceFlags::BIND_DEPTH_STENCIL | TextureResourceFlags::BIND_SHADER_RESOURCE;

    m_ShadowMapTexture = rendererAPI->CreateTexture2D(&initParams);
    m_DepthStencilView = rendererAPI->CreateDepthStencilView(m_ShadowMapTexture);
    m_ShadowMapRsourceView = rendererAPI->CreateShaderResourceView(m_ShadowMapTexture);
    delete m_ShadowMapTexture;

    m_Viewport.topLeftX = 0;
    m_Viewport.topLeftY = 0;
    m_Viewport.width = shadowMapWidth;
    m_Viewport.height = shadowMapHeight;

    m_ShadowGenShader = shaderLib->GetDefaultShader("ShadowGen");
}

ShadowMapPass::~ShadowMapPass() {
    delete m_ShadowMapRsourceView;
    delete m_DepthStencilView;
}

void ShadowMapPass::Execute(HWRendererAPI* rendererAPI, const std::vector<PGRenderObject*>& renderObjects) {
    rendererAPI->SetRenderTargets(nullptr, 0, m_DepthStencilView);
    rendererAPI->ClearDepthStencilView(m_DepthStencilView, false, 1.0f, 0);
    rendererAPI->SetViewport(&m_Viewport);

    HWShaderProgram* hwShader = m_ShadowGenShader->GetHWShader();
    rendererAPI->SetShaderProgram(hwShader);

    for (PGRenderObject* renderObject : renderObjects) {
        renderObject->UpdatePerDrawConstantBuffer(rendererAPI);

        size_t vertexBufferStride = sizeof(Vertex);
        rendererAPI->SetVertexBuffer(renderObject->vertexBuffer, vertexBufferStride);
        rendererAPI->SetIndexBuffer(renderObject->indexBuffer);
        rendererAPI->SetInputLayout(renderObject->inputLayout);
        if (renderObject->indexBuffer) {
            rendererAPI->DrawIndexed(renderObject->indexBuffer);
        } else {
            rendererAPI->Draw(renderObject->vertexBuffer);
        }
    }
}



