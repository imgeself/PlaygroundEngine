#include "PGSystem.h"
#include "Renderer/DX11/DX11RendererAPI.h"
#include "PGGameApplication.h"
#include "Platform/PGTime.h"
#include "PGProfiler.h"

// Static members
std::shared_ptr<PGSystemEventDispatcher> PGSystem::s_systemEventDispatcher = std::make_shared<PGSystemEventDispatcher>();

PGSystem::PGSystem() {
    s_systemEventDispatcher->RegisterListener(this);
}

PGSystem::~PGSystem() {
    s_systemEventDispatcher->RemoveListener(this);

    delete m_GameApplication;
    delete m_GameLibrary;
    delete m_Window;
}

bool PGSystem::InitializeSystem(SystemInitArguments* initArguments) {

    m_GameLibrary = new DLibrary(initArguments->gameLibraryPath);
    bool isLoaded = m_GameLibrary->Load();
    PG_ASSERT(isLoaded, "Couldn't load game library");

    GameApplicationProc GetGameApplication = m_GameLibrary->GetFunctionAddress<GameApplicationProc>("GetGameApplication");
    PG_ASSERT(GetGameApplication, "Couldn't get the game application proc");
    m_GameApplication = GetGameApplication(this);
    PG_ASSERT(m_GameApplication, "Couldn't get the game application");

    const char* windowName = "PlaygroundEngine";
    m_Window = new PGWindow(windowName, 1280, 720, s_systemEventDispatcher.get());

    PGRenderer::Initialize(m_Window);

    s_systemEventDispatcher->DispatchSystemEvent(SystemEvent::INITIALIZE);
    m_GameApplication->OnInit();

    ImguiModule::Initialize(m_Window, PGRenderer::GetRendererAPI());
    return true;
}

void PGSystem::RunMainLoop() {
    m_Window->Show();

    while (true) {
        PG_PROFILE_SCOPE("Frame");
        if (!m_Window->ProcessMessages()) {
            // Close requested
            s_systemEventDispatcher->DispatchSystemEvent(SystemEvent::CLOSE);
            m_GameApplication->OnExit();
            PGResourceManager::ReleaseResourcePool();
            PGRenderer::Uninitialize();
            break;
        }

        static uint64_t lastTime;
        uint64_t time = PGTime::GetTimeMacroseconds();
        uint64_t timePast = time - lastTime;
        float deltaTime = (float) (timePast / 1000000.0);
        lastTime = time;

        PGRenderer::BeginFrame();
        {
            PG_PROFILE_SCOPE("Update");
            m_GameApplication->OnUpdate(deltaTime);
        }

        {
            PG_PROFILE_SCOPE("Render");
            PGRenderer::RenderFrame();
        }

        ImguiModule::Begin(deltaTime);
        m_GameApplication->OnUIRender();
        ImguiModule::Render();

        PGRenderer::EndFrame();
    }
}

void PGSystem::OnSystemEvent(SystemEvent event, uint64_t param1, uint64_t param2) {
    if (event == SystemEvent::RESIZE) {
        PGRenderer::ResizeResources(param1, param2);
    }
}
