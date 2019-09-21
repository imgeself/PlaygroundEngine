#pragma once

#include "../Core.h"
#include "../MeshUtils.h"
#include "../Scene.h"
#include "../Platform/PGWindow.h"
#include "PGRendererAPI.h"

#include <unordered_set>

class PG_API PGRenderer {
public:
    ~PGRenderer();

    static bool Initialize(PGWindow* window);
    static void BeginFrame();
    static void EndFrame();

    static void BeginScene(PGScene* sceneData);
    static void AddRenderObject(Mesh* renderMesh);
    static void EndScene();

    static IRendererAPI* GetRendererAPI() {
        return m_RendererAPI;
    }

private:
    PGRenderer();

    static IRendererAPI* m_RendererAPI;
    static std::unordered_set<Mesh*> m_RenderObjects;
    static PGScene* m_ActiveSceneData;
};

