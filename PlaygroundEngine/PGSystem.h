#pragma once

#include "Core.h"
#include "Events/PGSystemEventDispatcher.h"
#include "Platform/PGWindow.h"
#include "Platform/PGLibrary.h"
#include "Renderer/PGRenderer.h"
#include "Renderer/PGShaderLib.h"
#include "Imgui/imgui_impl.h"
#include "PGApplication.h"
#include "MeshUtils.h"
#include "PGResouceManager.h"

#include <memory>

class PG_API PGSystem : public ISystemEventListener {
public:
    PGSystem();
    ~PGSystem();

    bool InitializeSystem(SystemInitArguments* initArguments);
    void RunMainLoop();

    virtual void OnSystemEvent(SystemEvent event, uint64_t param1, uint64_t param2) override;

    static std::shared_ptr<PGSystemEventDispatcher> GetSystemEventDispatcher() { return s_systemEventDispatcher; };

    MeshRef GetDefaultMeshInstance(const std::string& name) { 
        MeshRef ref = m_DefaultMeshMap[name];
        return std::make_shared<Mesh>(ref->name, ref->vertices, ref->indices);
    }

    inline PGShaderLib* GetShaderLib() {
        return PGRenderer::GetShaderLib();
    }

private:
    PGWindow* m_Window;
    DLibrary* m_GameLibrary;
    IApplication* m_GameApplication;

    std::unordered_map<std::string, MeshRef> m_DefaultMeshMap;

    static std::shared_ptr<PGSystemEventDispatcher> s_systemEventDispatcher;
};

