#pragma once

#include "HWRendererAPI.h"
#include "PGShaderLib.h"

#define MAX_RENDER_TARGET_COUNT 4

class TonemapPass {
public:
    TonemapPass();

    void SetRenderTarget(size_t slot, HWRenderTargetView* renderTarget);
    void SetViewport(HWViewport& viewport);
    void SetHDRBufferResourceView(HWShaderResourceView* hdrBuffer);

    void Execute(HWRendererAPI* rendererAPI, PGShaderLib* shaderLib);
private:
    HWRenderTargetView* m_RenderTargets[MAX_RENDER_TARGET_COUNT] = { 0 };
    size_t m_RenderTargetCount;
    HWViewport m_Viewport;

    HWShaderResourceView* m_HDRBufferResourceView = nullptr;
};

