#include "SceneRenderPass.h"

SceneRenderPass::SceneRenderPass() {

}

void SceneRenderPass::SetRenderTarget(size_t slot, HWRenderTargetView* renderTarget) {
    PG_ASSERT(slot < MAX_RENDER_TARGET_COUNT, "We only support 4 render target for now");
    m_RenderTargets[slot] = renderTarget;
}

void SceneRenderPass::SetDepthStencilTarget(HWDepthStencilView* depthStencilTarget) {
    m_DepthStencilView = depthStencilTarget;
}

void SceneRenderPass::SetViewport(HWViewport& viewport) {
    m_Viewport.topLeftX = viewport.topLeftX;
    m_Viewport.topLeftY = viewport.topLeftY;
    m_Viewport.width = viewport.width;
    m_Viewport.height = viewport.height;
}

void SceneRenderPass::AddRenderObject(PGRenderObject* renderObject) {
    m_RenderObjects.push_back(renderObject);
}

void SceneRenderPass::AddRenderObjects(const std::vector<PGRenderObject*>& renderObjects) {
    m_RenderObjects.insert(std::end(m_RenderObjects), std::begin(renderObjects), std::end(renderObjects));
}

void SceneRenderPass::Execute(HWRendererAPI* rendererAPI) {
    if (m_ShadowMapPass) {
        // Generate shadow map
        m_ShadowMapPass->Execute(rendererAPI, m_RenderObjects);
    }

    rendererAPI->SetRenderTargets(m_RenderTargets, MAX_RENDER_TARGET_COUNT, m_DepthStencilView);
    float color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    rendererAPI->ClearRenderTarget(m_RenderTargets[0], color);
    if (m_DepthStencilView) {
        rendererAPI->ClearDepthStencilView(m_DepthStencilView, false, 1.0f, 0);
    }
    rendererAPI->SetViewport(&m_Viewport);

    if (m_ShadowMapPass) {
        HWShaderResourceView* resource = m_ShadowMapPass->GetShadowMapResourceView();
        rendererAPI->SetShaderResourcesPS(SHADOW_MAP_TEXTURE2D_SLOT, &resource, 1);
    }

    for (PGRenderObject* renderObject : m_RenderObjects) {
        renderObject->UpdatePerDrawConstantBuffer(rendererAPI);

        PGTexture* albedoTexture = renderObject->mesh->material->albedoTexture;
        PGTexture* roughnessTexture = renderObject->mesh->material->roughnessTexture;
        PGTexture* metallicTexture = renderObject->mesh->material->metallicTexture;
        PGTexture* aoTexture = renderObject->mesh->material->aoTexture;
        HWShaderResourceView* textureResources[] = {
            albedoTexture ? albedoTexture->GetHWResourceView() : nullptr,
            roughnessTexture ? roughnessTexture->GetHWResourceView() : nullptr,
            metallicTexture ? metallicTexture->GetHWResourceView() : nullptr,
            aoTexture ? aoTexture->GetHWResourceView() : nullptr,
        };
        rendererAPI->SetShaderResourcesPS(1, textureResources, ARRAYSIZE(textureResources));

        size_t vertexBufferStride = sizeof(Vertex);
        rendererAPI->SetVertexBuffer(renderObject->vertexBuffer, vertexBufferStride);
        rendererAPI->SetIndexBuffer(renderObject->indexBuffer);
        rendererAPI->SetInputLayout(renderObject->inputLayout);
        rendererAPI->SetShaderProgram(renderObject->shader->GetHWShader());
        if (renderObject->indexBuffer) {
            rendererAPI->DrawIndexed(renderObject->indexBuffer);
        } else {
            rendererAPI->Draw(renderObject->vertexBuffer);
        }
    }
}

