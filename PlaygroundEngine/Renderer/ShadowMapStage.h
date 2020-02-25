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

                uint32_t vertexBufferStride = sizeof(Vertex);
                uint32_t offset = 0;
                rendererAPI->SetVertexBuffers(&renderObject->vertexBuffer, 1, &vertexBufferStride, &offset);
                rendererAPI->SetIndexBuffer(renderObject->indexBuffer);
                rendererAPI->SetInputLayout(renderObject->inputLayout);
                rendererAPI->DrawIndexed(renderObject->indexBuffer);
            }
        }
    };

    HWDepthStencilView* m_DepthStencilView;
    HWConstantBuffer* m_PerShadowGenConstantBuffer;

    ShadowGenStage::ShadowGenPass m_ShadowGenPass;
};

