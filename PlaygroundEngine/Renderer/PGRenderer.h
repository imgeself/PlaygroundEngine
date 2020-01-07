#pragma once

#include "../Core.h"
#include "../MeshUtils.h"
#include "../Scene.h"
#include "../Platform/PGWindow.h"

#include "../Assets/Shaders/ShaderDefinitions.h"
#include "HWRendererAPI.h"
#include "PGShaderLib.h"
#include "PGRenderObject.h"
#include "SceneRenderPass.h"
#include "PostProcessPass.h"

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
    static void AddMesh(const MeshRef& renderMesh);
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

    static void CreateDefaultSamplerStates();

    static HWRendererAPI* s_RendererAPI;
    static PGShaderLib* s_ShaderLib;
    static std::vector<PGRenderObject*> s_RenderObjects;
    static PGScene* s_ActiveSceneData;

    static HWConstantBuffer* s_PerFrameGlobalConstantBuffer;
    static HWConstantBuffer* s_PerDrawGlobalConstantBuffer;
    static HWConstantBuffer* s_PostProcessConstantBuffer;
    static HWConstantBuffer* s_RendererVarsConstantBuffer;

    static std::array<HWSamplerState*, 5> s_DefaultSamplers;

    static ShadowMapPass s_ShadowMapPass;
    static SceneRenderPass s_SceneRenderPass;
    static TonemapPass s_PostProcessPass;
};

