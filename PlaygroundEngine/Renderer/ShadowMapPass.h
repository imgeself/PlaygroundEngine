#pragma once

#include "HWRendererAPI.h"
#include "PGShaderLib.h"
#include "PGRenderObject.h"

const uint32_t CASCADE_COUNT = 4;

class ShadowMapPass {
public:
    ShadowMapPass();
    ~ShadowMapPass();

    void Initialize(HWRendererAPI* rendererAPI, PGShaderLib* shaderLib, size_t shadowMapSize);
    void Execute(HWRendererAPI* rendererAPI, const std::vector<PGRenderObject*>& renderObjects);

    inline HWTexture2D* GetShadowMapTexture() { return m_ShadowMapTexture; };
    inline HWShaderResourceView* GetShadowMapResourceView() { return m_ShadowMapRsourceView; };

private:
    HWTexture2D* m_ShadowMapTexture;
    HWDepthStencilView* m_DepthStencilViews[CASCADE_COUNT];
    HWViewport m_Viewport;
    ShaderRef m_ShadowGenShader;

    HWConstantBuffer* m_PerShadowGenConstantBuffer;

    HWShaderResourceView* m_ShadowMapRsourceView;
};

