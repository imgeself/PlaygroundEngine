#pragma once

#include "PGShaderLib.h"
#include "SceneRenderPass.h"

const uint32_t CASCADE_COUNT = 4;

class ShadowGenStage {
public:
    void Initialize(HWRendererAPI* rendererAPI, PGShaderLib* shaderLib, size_t shadowMapSize);
    void Execute(HWRendererAPI* rendererAPI, const std::vector<PGRenderObject*>& shadowCasters);

    inline void SetShadowMapDSV(HWDepthStencilView* dsv) { m_DepthStencilView = dsv; }

private:
    class ShadowGenPass : public SceneRenderPass {
        friend class ShadowGenStage;
    public:
        virtual ~ShadowGenPass() = default;

        virtual void Execute(HWRendererAPI* rendererAPI) {
            PG_PROFILE_FUNCTION();
            for (PGRenderObject* renderObject : m_RenderObjects) {
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
    };

    HWDepthStencilView* m_DepthStencilView;
    HWConstantBuffer* m_PerShadowGenConstantBuffer;

    ShadowGenStage::ShadowGenPass m_ShadowGenPass;
};

