#pragma once

#include "BaseRenderPass.h"

class FullscreenPass : public RenderPass {
public:
    virtual ~FullscreenPass() = default;

    virtual void Execute(HWRendererAPI* rendererAPI) override {
        rendererAPI->SetRenderTargets(m_RenderTargets, MAX_RENDER_TARGET_COUNT, nullptr);
        for (size_t i = 0; i < MAX_RENDER_TARGET_COUNT; ++i) {
            HWRenderTargetView* renderTarget = m_RenderTargets[i];
            if (renderTarget) {
                float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
                rendererAPI->ClearRenderTarget(m_RenderTargets[0], color);
            }
        }
        rendererAPI->SetViewport(&m_Viewport);

        rendererAPI->SetVertexShader(m_Shader->GetHWVertexShader());
        rendererAPI->SetPixelShader(m_Shader->GetHWPixelShader());

        // Vertex shader only used for generating fullscreen quad, we don't need to bind any resources for vertex shader
        rendererAPI->SetShaderResourcesPS(0, m_ShaderResourcesPS, MAX_SHADER_RESOURCE_COUNT);
        rendererAPI->Draw(3, 0);
    }
};

