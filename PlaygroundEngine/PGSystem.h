#pragma once

#include "Core.h"
#include "Events/PGSystemEventDispatcher.h"
#include "Platform/PGWindow.h"
#include "Platform/PGLibrary.h"
#include "Renderer/PGRenderer.h"
#include "PGApplication.h"
#include "Imgui/imgui_impl.h"
#include "MeshUtils.h"

#include <memory>

class PGSystem : public ISystemEventListener {
public:
    PGSystem();
    ~PGSystem();

    bool InitializeSystem(SystemInitArguments* initArguments);
    void RunMainLoop();

    virtual void OnSystemEvent(SystemEvent event) override;

    static std::shared_ptr<PGSystemEventDispatcher> GetSystemEventDispatcher() { return s_systemEventDispatcher; };
    Mesh* GetDefaultMeshInstance(const std::string& name, const Material& material, const Transform& transform) { 
        MeshRef ref = m_DefaultMeshMap[name];
        return new Mesh(ref->name, ref->vertices, ref->indices, material, transform);
    }

private:
    PGWindow* m_Window;
    DLibrary* m_GameLibrary;
    IApplication* m_GameApplication;

    std::unordered_map<std::string, MeshRef> m_DefaultMeshMap;

    static std::shared_ptr<PGSystemEventDispatcher> s_systemEventDispatcher;
};

