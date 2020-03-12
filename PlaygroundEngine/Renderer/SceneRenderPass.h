#pragma once

#include "HWRendererAPI.h"
#include "PGRenderList.h"

#include "BaseRenderPass.h"
#include "../PGProfiler.h"

class SceneRenderPass : public RenderPass {
public:
    virtual ~SceneRenderPass() = default;

    virtual void Execute(HWRendererAPI* rendererAPI, const RenderList& renderList, SceneRenderPassType scenePassType, bool clear = true) {
        if (clear) {
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
        }
        rendererAPI->SetViewport(&m_Viewport);
        rendererAPI->SetShaderResourcesVS(0, m_ShaderResourcesVS, MAX_SHADER_RESOURCE_COUNT);
        rendererAPI->SetShaderResourcesPS(0, m_ShaderResourcesPS, MAX_SHADER_RESOURCE_COUNT);

        RenderScene(rendererAPI, renderList, scenePassType);
    }
};
