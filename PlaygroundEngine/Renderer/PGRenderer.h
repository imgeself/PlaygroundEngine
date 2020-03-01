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
    static void Uninitialize();
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

private:
    PGRenderer();

    static HWRendererAPI* s_RendererAPI;
    static PGShaderLib* s_ShaderLib;
    static PGScene* s_ActiveSceneData;

    static PGRendererConfig s_RendererConfig;
    static RenderList s_RenderList;

    static ShadowGenStage s_ShadowGenStage;
    static SceneRenderPass s_SceneRenderPass;
    static FullscreenPass s_ToneMappingPass;
};

