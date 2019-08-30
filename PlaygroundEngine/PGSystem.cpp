#include "PGSystem.h"
#include "Renderer/PGRenderer.h"
#include "Renderer/DX11/DX11Renderer.h"

// Static members
std::shared_ptr<PGSystemEventDispatcher> PGSystem::m_systemEventDispatcher = std::make_shared<PGSystemEventDispatcher>();

PGSystem::PGSystem() {
    m_systemEventDispatcher->RegisterListener(this);
}

PGSystem::~PGSystem() {
    m_systemEventDispatcher->RemoveListener(this);
}

bool PGSystem::InitializeSystem() {

    const char* windowName = "PlaygroundEngine";
    m_Window = new PGWindow(windowName, 1280, 720);

    m_Renderer = new DX11Renderer(m_Window);
    
    m_systemEventDispatcher->DispatchSystemEvent(SystemEvent::INITIALIZE);
    
    return true;
}

void PGSystem::RunMainLoop() {
    m_Window->Show();

    bool running = true;
    while (running) {
        if (!m_Window->ProcessMessages()) {
            // Close requested
            m_systemEventDispatcher->DispatchSystemEvent(SystemEvent::CLOSE);

            delete m_Window;
            running = false;
        }

        if (PGInput::IsKeyPressed(PGKEY_W)) {
            printf("W is pressed\n");
        }

        const float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
        m_Renderer->ClearScreen(color);

        m_Renderer->EndFrame();
    }
}

void PGSystem::OnSystemEvent(SystemEvent event) {
    printf("Event occured %d\n", event);
}
