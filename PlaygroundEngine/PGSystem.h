#pragma once

#include "Events/PGSystemEventDispatcher.h"
#include "Platform/PGWindow.h"
#include "Renderer/PGRendererAPI.h"

#include <memory>

class PGSystem : public ISystemEventListener {
public:
    PGSystem();
    ~PGSystem();

    bool InitializeSystem();
    void RunMainLoop();

    virtual void OnSystemEvent(SystemEvent event) override;

    static std::shared_ptr<PGSystemEventDispatcher> GetSystemEventDispatcher() { return m_systemEventDispatcher; };

private:
    PGWindow* m_Window;
    IRendererAPI* m_Renderer;

    static std::shared_ptr<PGSystemEventDispatcher> m_systemEventDispatcher;
};

