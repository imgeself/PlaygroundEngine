#include "SceneRenderPass.h"
#include "../PGProfiler.h"

void SceneRenderPass::SetRenderObjects(const std::vector<PGRenderObject*>& renderObjects) {
    m_RenderObjects.clear();
    m_RenderObjects.insert(std::end(m_RenderObjects), std::begin(renderObjects), std::end(renderObjects));
}

void SceneRenderPass::Execute(HWRendererAPI* rendererAPI) {
    PG_PROFILE_FUNCTION();
    rendererAPI->SetRenderTargets(m_RenderTargets, MAX_RENDER_TARGET_COUNT, m_DepthStencilView);
    for (size_t i = 0; i < MAX_RENDER_TARGET_COUNT; ++i) {
        HWRenderTargetView* renderTarget = m_RenderTargets[i];
        if (renderTarget) {
            float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
            rendererAPI->ClearRenderTarget(m_RenderTargets[0], color);
        }
    }
    if (m_DepthStencilView) {
        rendererAPI->ClearDepthStencilView(m_DepthStencilView, false, 1.0f, 0);
    }
    rendererAPI->SetViewport(&m_Viewport);
    rendererAPI->SetVertexShader(m_Shader->GetHWVertexShader());
    rendererAPI->SetPixelShader(m_Shader->GetHWPixelShader());

    for (PGRenderObject* renderObject : m_RenderObjects) {
        renderObject->UpdatePerDrawConstantBuffer(rendererAPI);

        PGTexture* albedoTexture = renderObject->mesh->material->albedoTexture;
        PGTexture* roughnessTexture = renderObject->mesh->material->roughnessTexture;
        PGTexture* metallicTexture = renderObject->mesh->material->metallicTexture;
        PGTexture* aoTexture = renderObject->mesh->material->aoTexture;
        PGTexture* radianceMap = renderObject->mesh->material->radianceMap;
        PGTexture* irradianceMap = renderObject->mesh->material->irradianceMap;
        PGTexture* envBrdf = renderObject->mesh->material->envBrdf;
        HWShaderResourceView* textureResources[] = {
            albedoTexture ? albedoTexture->GetHWResourceView() : nullptr,
            roughnessTexture ? roughnessTexture->GetHWResourceView() : nullptr,
            metallicTexture ? metallicTexture->GetHWResourceView() : nullptr,
            aoTexture ? aoTexture->GetHWResourceView() : nullptr,
            radianceMap ? radianceMap->GetHWResourceView() : nullptr,
            irradianceMap ? irradianceMap->GetHWResourceView() : nullptr,
            envBrdf ? envBrdf->GetHWResourceView() : nullptr,
        };
        memcpy(m_ShaderResourcesPS + ALBEDO_TEXTURE2D_SLOT, textureResources, sizeof(textureResources));
        
        rendererAPI->SetShaderResourcesVS(0, m_ShaderResourcesVS, MAX_SHADER_RESOURCE_COUNT);
        rendererAPI->SetShaderResourcesPS(0, m_ShaderResourcesPS, MAX_SHADER_RESOURCE_COUNT);

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

