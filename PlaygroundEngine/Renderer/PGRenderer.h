#pragma once

#include "../Core.h"
#include "../MeshUtils.h"
#include "../Scene.h"
#include "../Platform/PGWindow.h"

#include "../Assets/Shaders/ShaderDefinitions.h"
#include "HWRendererAPI.h"
#include "PGShaderLib.h"
#include "PGRenderList.h"
#include "SceneRenderPass.h"
#include "FullscreenPass.h"
#include "ShadowMapStage.h"

#include <vector>
#include <array>

class PG_API PGRenderer {
public:
    static bool Initialize(PGWindow* window);
    static void Destroy();
    static void BeginFrame();
    static void RenderFrame();
    static void EndFrame();

    static void BeginScene(PGScene* sceneData);
    static void EndScene();

    static void ResizeResources(size_t newWidth, size_t newHeight);

    static HWRendererAPI* GetRendererAPI() {
        return s_RendererAPI;
    }

    static PGShaderLib* GetShaderLib() {
        return s_ShaderLib;
    }

    static void DrawBoundingBoxes(bool enable) {
        s_RendererConfig.debugDrawBoundingBoxes = enable;
    }

    static void DrawCascadeColors(bool enable) {
        s_RendererConfig.debugDrawCascadeColor = enable;

        RendererVariablesConstantBuffer rendererVariablesConstantBuffer = {};
        rendererVariablesConstantBuffer.g_ShadowCascadeCount = s_RendererConfig.shadowCascadeCount;
        rendererVariablesConstantBuffer.g_ShadowMapSize = s_RendererConfig.shadowMapSize;
        rendererVariablesConstantBuffer.g_VisualizeCascades = s_RendererConfig.debugDrawCascadeColor;

        void* data = s_RendererAPI->Map(PGRendererResources::s_RendererVarsConstantBuffer);
        memcpy(data, &rendererVariablesConstantBuffer, sizeof(RendererVariablesConstantBuffer));
        s_RendererAPI->Unmap(PGRendererResources::s_RendererVarsConstantBuffer);
    }

    static void SetShadowMapResolution(uint32_t shadowMapResolution) {
        s_RendererConfig.shadowMapSize = shadowMapResolution;
        PGRendererResources::CreateShadowMapResources(s_RendererAPI, s_RendererConfig);
        s_ShadowGenStage.SetShadowMapTarget(s_RendererAPI, (HWTexture2D*) PGRendererResources::s_ShadowMapCascadesTexture->resource, s_RendererConfig);
        s_SceneRenderPass.SetShaderResource(SHADOW_MAP_TEXTURE2D_SLOT, PGRendererResources::s_ShadowMapCascadesTexture->srv, ShaderStage::PIXEL);

        RendererVariablesConstantBuffer rendererVariablesConstantBuffer = {};
        rendererVariablesConstantBuffer.g_ShadowCascadeCount = s_RendererConfig.shadowCascadeCount;
        rendererVariablesConstantBuffer.g_ShadowMapSize = s_RendererConfig.shadowMapSize;
        rendererVariablesConstantBuffer.g_VisualizeCascades = s_RendererConfig.debugDrawCascadeColor;

        void* data = s_RendererAPI->Map(PGRendererResources::s_RendererVarsConstantBuffer);
        memcpy(data, &rendererVariablesConstantBuffer, sizeof(RendererVariablesConstantBuffer));
        s_RendererAPI->Unmap(PGRendererResources::s_RendererVarsConstantBuffer);
    }

    static void SetMSAASampleCount(uint8_t sampleCount) {
        s_RendererConfig.msaaSampleCount = sampleCount;
        PGRendererResources::CreateSizeDependentResources(s_RendererAPI, s_RendererConfig);

        // Render passes
        s_SceneZPrePass.SetDepthStencilTarget(PGRendererResources::s_DepthStencilTarget->dsv);
        s_SceneRenderPass.SetRenderTarget(0, PGRendererResources::s_HDRRenderTarget->rtv);
        s_SceneRenderPass.SetDepthStencilTarget(PGRendererResources::s_DepthStencilTarget->dsv);

        // Post process
        s_ToneMappingPass.SetRenderTarget(0, s_RendererAPI->GetBackbufferRenderTargetView());
        if (s_RendererConfig.msaaSampleCount > 1) {
            s_ToneMappingPass.SetShaderResource(POST_PROCESS_TEXTURE0_SLOT, PGRendererResources::s_ResolvedHDRRenderTarget->srv, ShaderStage::PIXEL);
        } else {
            s_ToneMappingPass.SetShaderResource(POST_PROCESS_TEXTURE0_SLOT, PGRendererResources::s_HDRRenderTarget->srv, ShaderStage::PIXEL);
        }
    }

    static void SetAnisotropyLevel(uint8_t anistropyLevel) {
        SAFE_DELETE(PGRendererResources::s_DefaultSamplers[OBJECT_SAMPLER_STATE_SLOT]);

        SamplerStateInitParams objectSamplerStateInitParams = {};
        objectSamplerStateInitParams.filterMode = TextureFilterMode_ANISOTROPIC;
        objectSamplerStateInitParams.addressModeU = TextureAddressMode_WRAP;
        objectSamplerStateInitParams.addressModeV = TextureAddressMode_WRAP;
        objectSamplerStateInitParams.addressModeW = TextureAddressMode_WRAP;
        objectSamplerStateInitParams.maxAnisotropy = anistropyLevel;
        PGRendererResources::s_DefaultSamplers[OBJECT_SAMPLER_STATE_SLOT] = s_RendererAPI->CreateSamplerState(&objectSamplerStateInitParams);

        s_RendererAPI->SetSamplerStatesPS(0, PGRendererResources::s_DefaultSamplers.data(), PGRendererResources::s_DefaultSamplers.max_size());
    }

private:
    PGRenderer();

    static HWRendererAPI* s_RendererAPI;
    static PGShaderLib* s_ShaderLib;
    static PGScene* s_ActiveSceneData;

    static PGRendererConfig s_RendererConfig;

    static std::array<RenderList, RL_COUNT> s_RenderLists;

    static ShadowGenStage s_ShadowGenStage;
    static SceneRenderPass s_SceneZPrePass;
    static SceneRenderPass s_SceneRenderPass;
    static FullscreenPass s_ToneMappingPass;
};

