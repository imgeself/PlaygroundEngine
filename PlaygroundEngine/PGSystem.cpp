#include "PGSystem.h"
#include "Renderer/PGRendererAPI.h"
#include "Renderer/DX11/DX11RendererAPI.h"
#include "PGGameApplication.h"
#include "Platform/PGTime.h"

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
    m_Window = new PGWindow(windowName, 1280, 720);

    PGRenderer::Initialize(m_Window);

    m_DefaultMeshMap = LoadDefaultMeshes();
    
    s_systemEventDispatcher->DispatchSystemEvent(SystemEvent::INITIALIZE);
    m_GameApplication->OnInit();

    ImguiModule::Initialize(m_Window, PGRenderer::GetRendererAPI());
    return true;
}

void PGSystem::RunMainLoop() {
    m_Window->Show();

    while (true) {
        if (!m_Window->ProcessMessages()) {
            // Close requested
            s_systemEventDispatcher->DispatchSystemEvent(SystemEvent::CLOSE);
            m_GameApplication->OnExit();
            break;
        }

        static uint64_t lastTime;
        uint64_t time = PGTime::GetTimeMacroseconds();
        uint64_t timePast = time - lastTime;
        float deltaTime = (float) (timePast / 1000000.0);
        lastTime = time;

        PGRenderer::BeginFrame();

        m_GameApplication->OnUpdate(deltaTime);

        ImguiModule::Begin(deltaTime);
        m_GameApplication->OnUIRender();
        ImguiModule::Render();

        PGRenderer::EndFrame();
    }
}

void PGSystem::OnSystemEvent(SystemEvent event) {
    printf("Event occured %d\n", event);
}
