#include "PGSystem.h"

bool PGSystem::InitializeSystem() {

    const char* windowName = "PlaygroundEngine";
    m_Window = new PGWindow(windowName, 1280, 720);
    return true;
}

void PGSystem::RunMainLoop() {
    m_Window->Show();

    bool running = true;
    while (running) {
        if (!m_Window->ProcessMessages()) {
            // Close requested

            delete m_Window;
            running = false;
        }
    }
}

