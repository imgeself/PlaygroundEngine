#pragma once

#include "../Core.h"
#include "../MeshUtils.h"
#include "../Scene.h"
#include "../Platform/PGWindow.h"
#include "HWRendererAPI.h"
#include "PGShaderLib.h"
#include "../Assets/Shaders/ShaderDefinitions.h"
#include "PGRenderObject.h"

#include <vector>

class PG_API PGRenderer {
public:
    static bool Initialize(PGWindow* window);
    static void Uninitialize();
    static void BeginFrame();
    static void RenderFrame();
    static void EndFrame();

    static void BeginScene(PGScene* sceneData);
    static void AddMesh(Mesh* renderMesh);
    static void EndScene();

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
    static std::vector<PGRenderObject*> s_RenderObjects;
    static PGScene* s_ActiveSceneData;

    static HWConstantBuffer* s_PerFrameGlobalConstantBuffer;
};

