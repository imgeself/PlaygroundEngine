#pragma once

#include "BaseRenderPass.h"

class FullscreenPass : public RenderPass {
public:
    virtual ~FullscreenPass() = default;

    void Execute(HWRendererAPI* rendererAPI, bool clearRenderTargets, const char* eventName) {
        rendererAPI->BeginEvent(eventName);
        rendererAPI->SetRenderTargets(m_RenderTargets, MAX_RENDER_TARGET_COUNT, nullptr);
        if (clearRenderTargets) {
            for (size_t i = 0; i < MAX_RENDER_TARGET_COUNT; ++i) {
                HWRenderTargetView* renderTarget = m_RenderTargets[i];
                if (renderTarget) {
                    float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
                    rendererAPI->ClearRenderTarget(m_RenderTargets[0], color);
                }
            }
        }

        rendererAPI->SetViewport(&m_Viewport);
        rendererAPI->SetGraphicsPipelineState(m_PipelineState);

        // Vertex shader only used for generating fullscreen quad, we don't need to bind any resources for vertex shader
        rendererAPI->SetShaderResourcesPS(0, m_ShaderResourcesPS, MAX_SHADER_RESOURCE_COUNT);
        rendererAPI->Draw(3, 0);
        rendererAPI->EndEvent();
    }

    void SetShader(HWRendererAPI* rendererAPI, PGShader* shader) {
        SAFE_DELETE(m_PipelineState);

        HWDepthStencilDesc depthStencilDesc;
        depthStencilDesc.depthEnable = false;
        depthStencilDesc.depthWriteMask = DEPTH_WRITE_MASK_ZERO;

        HWGraphicsPipelineStateDesc pipelineDesc;
        pipelineDesc.vertexShader = shader->GetVertexBytecode();
        pipelineDesc.pixelShader = shader->GetPixelBytecode();
        pipelineDesc.depthStencilDesc = depthStencilDesc;
        pipelineDesc.primitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        m_PipelineState = rendererAPI->CreateGraphicsPipelineState(pipelineDesc);
    }

private:
    HWGraphicsPipelineState* m_PipelineState;
};

