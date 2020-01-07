#include "PostProcessPass.h"
#include "../Assets/Shaders/ShaderDefinitions.h"

TonemapPass::TonemapPass() {

}

void TonemapPass::SetRenderTarget(size_t slot, HWRenderTargetView* renderTarget) {
    PG_ASSERT(slot < MAX_RENDER_TARGET_COUNT, "We only support 4 render target for now");
    m_RenderTargets[slot] = renderTarget;
}

void TonemapPass::SetViewport(HWViewport& viewport) {
    m_Viewport.topLeftX = viewport.topLeftX;
    m_Viewport.topLeftY = viewport.topLeftY;
    m_Viewport.width = viewport.width;
    m_Viewport.height = viewport.height;
}

void TonemapPass::SetHDRBufferResourceView(HWShaderResourceView* hdrBuffer) {
    m_HDRBufferResourceView = hdrBuffer;
}

void TonemapPass::Execute(HWRendererAPI* rendererAPI, PGShaderLib* shaderLib) {
    rendererAPI->SetRenderTargets(m_RenderTargets, MAX_RENDER_TARGET_COUNT, nullptr);
    float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    rendererAPI->ClearRenderTarget(m_RenderTargets[0], color);
    rendererAPI->SetViewport(&m_Viewport);

    PGShader* hdrPostShader = shaderLib->GetDefaultShader("HDRPostProcess");
    rendererAPI->SetShaderProgram(hdrPostShader->GetHWShader());

    rendererAPI->SetShaderResourcesPS(POST_PROCESS_TEXTURE0_SLOT, &m_HDRBufferResourceView, 1);
    rendererAPI->Draw(3, 0);
}

