#pragma once

#include "HWRendererAPI.h"
#include "PGRenderObject.h"
#include "ShadowMapPass.h"

#define MAX_RENDER_TARGET_COUNT 4

class SceneRenderPass {
public:
    SceneRenderPass();

    void SetRenderTarget(size_t slot, HWRenderTargetView* renderTarget);
    void SetDepthStencilTarget(HWDepthStencilView* depthStencilTarget);
    void SetViewport(HWViewport& viewport);

    void AddRenderObject(PGRenderObject* renderObject);
    void AddRenderObjects(const std::vector<PGRenderObject*>& renderObjects);

    void Execute(HWRendererAPI* rendererAPI);

    inline void SetShadowMapPass(ShadowMapPass* shadowMapPass) { 
        m_ShadowMapPass = shadowMapPass;
    }

private:
    HWDepthStencilView* m_DepthStencilView = nullptr;
    HWRenderTargetView* m_RenderTargets[MAX_RENDER_TARGET_COUNT] = {0};
    size_t m_RenderTargetCount;
    HWViewport m_Viewport;

    ShadowMapPass* m_ShadowMapPass;

    std::vector<PGRenderObject*> m_RenderObjects;
};
