#pragma once

#include "Core.h"
#include "Events/PGSystemEventDispatcher.h"
#include "Platform/PGWindow.h"
#include "Platform/PGLibrary.h"
#include "Renderer/PGRendererAPI.h"
#include "PGApplication.h"

#include <memory>

class PGSystem : public ISystemEventListener {
public:
    PGSystem();
    ~PGSystem();

    bool InitializeSystem(SystemInitArguments* initArguments);
    void RunMainLoop();

    virtual void OnSystemEvent(SystemEvent event) override;

    static std::shared_ptr<PGSystemEventDispatcher> GetSystemEventDispatcher() { return s_systemEventDispatcher; };
    IRendererAPI* GetRendererApi() { return m_Renderer; };

private:
    PGWindow* m_Window;
    IRendererAPI* m_Renderer;
    DLibrary* m_GameLibrary;
    IApplication* m_GameApplication;

    static std::shared_ptr<PGSystemEventDispatcher> s_systemEventDispatcher;
};

