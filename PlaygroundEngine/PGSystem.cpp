#include "PGSystem.h"

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
    }
}

void PGSystem::OnSystemEvent(SystemEvent event) {
    printf("Event occured %d\n", event);
}


