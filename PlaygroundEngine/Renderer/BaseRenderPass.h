#pragma once

#include "HWRendererAPI.h"
#include "PGShader.h"

#define MAX_RENDER_TARGET_COUNT 4
#define MAX_SHADER_RESOURCE_COUNT 64

enum ShaderStage : uint8_t {
    VERTEX   = BIT(1),
    PIXEL    = BIT(2),
    COMPUTE  = BIT(3)
};

class RenderPass {
public:
    virtual ~RenderPass() = default;

    inline void SetRenderTarget(size_t slot, HWRenderTargetView* renderTarget) {
        m_RenderTargets[slot] = renderTarget;
    }

    inline void SetDepthStencilTarget(HWDepthStencilView* depthStencilTarget) {
        m_DepthStencilView = depthStencilTarget;
    }

    inline void SetViewport(HWViewport& viewport) {
        m_Viewport = viewport;
    }

    inline void SetShaderResource(size_t slot, HWShaderResourceView* shaderResource, uint8_t shaderStage) {
        if (shaderStage & ShaderStage::VERTEX) {
            m_ShaderResourcesVS[slot] = shaderResource;
        }
        if (shaderStage & ShaderStage::PIXEL) {
            m_ShaderResourcesPS[slot] = shaderResource;
        }
    }

protected:
    HWDepthStencilView* m_DepthStencilView = nullptr;
    HWRenderTargetView* m_RenderTargets[MAX_RENDER_TARGET_COUNT] = { 0 };
    HWViewport m_Viewport;

    HWShaderResourceView* m_ShaderResourcesVS[MAX_SHADER_RESOURCE_COUNT] = { 0 };
    HWShaderResourceView* m_ShaderResourcesPS[MAX_SHADER_RESOURCE_COUNT] = { 0 };

};

