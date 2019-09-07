#include "PGSystem.h"
#include "Renderer/PGRendererAPI.h"
#include "Renderer/DX11/DX11RendererAPI.h"

// Static members
std::shared_ptr<PGSystemEventDispatcher> PGSystem::m_systemEventDispatcher = std::make_shared<PGSystemEventDispatcher>();

PGSystem::PGSystem() {
    m_systemEventDispatcher->RegisterListener(this);
}

PGSystem::~PGSystem() {
    m_systemEventDispatcher->RemoveListener(this);
}

bool PGSystem::InitializeSystem(SystemInitArguments* initArguments) {

    m_GameLibrary = new DLibrary(initArguments->gameLibraryPath);
    bool isLoaded = m_GameLibrary->Load();
    PG_ASSERT(isLoaded, "Couldn't load game library");

    GameApplicationProc GetGameApplication = m_GameLibrary->GetFunctionAddress<GameApplicationProc>("GetGameApplication");
    PG_ASSERT(GetGameApplication, "Couldn't get the game application proc");
    m_GameApplication = GetGameApplication();
    PG_ASSERT(m_GameApplication, "Couldn't get the game application");

    const char* windowName = "PlaygroundEngine";
    m_Window = new PGWindow(windowName, 1280, 720);

    m_Renderer = new DX11RendererAPI(m_Window);
    
    m_systemEventDispatcher->DispatchSystemEvent(SystemEvent::INITIALIZE);
    m_GameApplication->OnInit();
    
    return true;
}

void PGSystem::RunMainLoop() {
    m_Window->Show();

    while (true) {
        if (!m_Window->ProcessMessages()) {
            // Close requested
            m_systemEventDispatcher->DispatchSystemEvent(SystemEvent::CLOSE);
            m_GameApplication->OnExit();
            break;
        }

        if (PGInput::IsKeyPressed(PGKEY_W)) {
            printf("W is pressed\n");
        }
        m_GameApplication->OnUpdate();

        const float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
        m_Renderer->ClearScreen(color);

        m_GameApplication->OnRender();

        m_Renderer->Render();
        m_Renderer->EndFrame();
    }
}

void PGSystem::OnSystemEvent(SystemEvent event) {
    printf("Event occured %d\n", event);
}
