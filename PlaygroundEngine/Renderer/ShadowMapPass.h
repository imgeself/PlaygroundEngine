#pragma once

#include "HWRendererAPI.h"
#include "PGShaderLib.h"
#include "PGRenderObject.h"

class ShadowMapPass {
public:
    ShadowMapPass();
    ~ShadowMapPass();

    void Initialize(HWRendererAPI* rendererAPI, PGShaderLib* shaderLib, size_t shadowMapWidth, size_t shadowMapHeight);
    void Execute(HWRendererAPI* rendererAPI, const std::vector<PGRenderObject*>& renderObjects);

    inline HWTexture2D* GetShadowMapTexture() { return m_ShadowMapTexture; };
    inline HWShaderResourceView* GetShadowMapResourceView() { return m_ShadowMapRsourceView; };

private:
    HWTexture2D* m_ShadowMapTexture;
    HWDepthStencilView* m_DepthStencilView;
    HWViewport m_Viewport;
    ShaderRef m_ShadowGenShader;

    HWShaderResourceView* m_ShadowMapRsourceView;
};

