#pragma once

#include "Core.h"
#include "Events/PGSystemEventDispatcher.h"
#include "Platform/PGWindow.h"
#include "Platform/PGLibrary.h"
#include "Renderer/PGRendererAPI.h"
#include "PGGameApplication.h"

#include <memory>

class PGSystem : public ISystemEventListener {
public:
    PGSystem();
    ~PGSystem();

    bool InitializeSystem(SystemInitArguments* initArguments);
    void RunMainLoop();

    virtual void OnSystemEvent(SystemEvent event) override;

    static std::shared_ptr<PGSystemEventDispatcher> GetSystemEventDispatcher() { return m_systemEventDispatcher; };

private:
    PGWindow* m_Window;
    IRendererAPI* m_Renderer;
    DLibrary* m_GameLibrary;
    IApplication* m_GameApplication;

    static std::shared_ptr<PGSystemEventDispatcher> m_systemEventDispatcher;
};

