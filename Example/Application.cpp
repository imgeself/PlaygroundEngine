#include "Application.h"
#include "LogWindow.h"
#include "PerformanceWindow.h"

#include <stdio.h>
#include <time.h>
#include <Math/math_util.h>
#include <Platform/PGTime.h>

#include <MeshUtils.h>

#include <PGLog.h>

Application::Application(PGSystem* system) 
    : m_System(system) {
    system->GetSystemEventDispatcher()->RegisterListener(this);
}

Application::~Application() {
    m_System->GetSystemEventDispatcher()->RemoveListener(this);
}

void Application::OnInit() {

    m_MainCamera.SetFrustum(1280, 720, 0.01f, 100.0f, PI / 4.0f);

    PGShaderLib* shaderLib = m_System->GetShaderLib();
    m_PBRShader = shaderLib->GetDefaultShader("PBRForward");

    Vector3 lightPosition(20.0f, 54.0f, -20.0f);
    PGLight* mainLight = new PGLight;
    mainLight->position = lightPosition;

    m_Scene.camera = &m_MainCamera;
    m_Scene.light = mainLight;

    PGTexture* skybox = (PGTexture*) PGResourceManager::CreateResource("./assets/envmap/environment.dds");
    PGTexture* irradiance = (PGTexture*) PGResourceManager::CreateResource("./assets/envmap/irradiance.dds");
    PGTexture* radiance = (PGTexture*) PGResourceManager::CreateResource("./assets/envmap/radiance.dds");
    PGTexture* brdf = (PGTexture*)PGResourceManager::CreateResource("./assets/envmap/brdfLUT.dds");

    m_Scene.skybox = new Skybox(skybox);

    PGRenderer::BeginScene(&m_Scene);

    m_DefaultMaterial = new Material;
    memset(m_DefaultMaterial, 0, sizeof(Material));
    m_DefaultMaterial->diffuseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    m_DefaultMaterial->ambientColor = Vector4(0.03f, 0.03f, 0.03f, 1.0f);
    m_DefaultMaterial->opacity = 1.0f;
    m_DefaultMaterial->roughness = 0.0f;
    m_DefaultMaterial->metallic = 0.0f;
    m_DefaultMaterial->shader = m_PBRShader;
    m_DefaultMaterial->radianceMap = radiance;
    m_DefaultMaterial->irradianceMap = irradiance;
    m_DefaultMaterial->envBrdf = brdf;

    LoadMeshFromGLTFFile(PGRenderer::GetRendererAPI(), &m_Scene, m_DefaultMaterial, "./assets/DamagedHelmet/DamagedHelmet.gltf");
    //LoadMeshFromGLTFFile(PGRenderer::GetRendererAPI(), &m_Scene, m_DefaultMaterial, "./assets/Sponza/Sponza.gltf");
    Transform cameraTransform;
    cameraTransform.Translate(Vector3(0.0f,-0.5f, -4.0f));
    m_MainCamera.TransformCamera(&cameraTransform);

    PGRenderer::EndScene();
}

void Application::OnSystemEvent(SystemEvent event, uint64_t param1, uint64_t param2) {
    if (event == SystemEvent::RESIZE) {
        m_MainCamera.SetFrustum((uint32_t) param1, (uint32_t) param2, 0.01f, 100.0f, PI / 4.0f);
    }
}


void Application::OnUpdate(float deltaTime) {
    if (ImGui::IsAnyItemActive()) {
        return;
    }

    Transform cameraTransform;
    Vector3 cameraMove(0.0f, 0.0f, 0.0f);

    static Vector2 lastMouse;
    static float pitch = 0.0f;
    static float yaw = 0.0f;
    if (PGInput::IsMouseButtonPressed(PGMOUSE_LBUTTON)) {
        Vector2 mousePos = PGInput::GetMousePos();
        if (lastMouse == Vector2(-1.0f, -1.0f)) {
            // First click
            lastMouse = mousePos;
        }
        else {
            Vector2 mouseDelta = mousePos - lastMouse;
            lastMouse = mousePos;

            pitch += mouseDelta.y / 400;
            yaw += mouseDelta.x / 300;
        }
    } else {
        lastMouse.x = -1.0f;
        lastMouse.y = -1.0f;
    }

    cameraTransform.RotateXAxis(pitch);
    cameraTransform.RotateYAxis(yaw);

    if (PGInput::IsKeyPressed(PGKEY_W)) {
        cameraMove.z += 5.0f * deltaTime;

    } else if (PGInput::IsKeyPressed(PGKEY_S)) {
        cameraMove.z -= 5.0f * deltaTime;
    }

    if (PGInput::IsKeyPressed(PGKEY_A)) {
        cameraMove.x -= 5.0f * deltaTime;
    } else if (PGInput::IsKeyPressed(PGKEY_D)) {
        cameraMove.x += 5.0f * deltaTime;
    }

    if (PGInput::IsKeyPressed(PGKEY_Q)) {
        cameraMove.y -= 5.0f * deltaTime;
    }
    else if (PGInput::IsKeyPressed(PGKEY_E)) {
        cameraMove.y += 5.0f * deltaTime;
    }

    cameraMove = (cameraTransform.rotationMatrix * Vector4(cameraMove, 0.0f)).xyz();

    Vector3 cameraPos = m_Scene.camera->GetPosition();
    cameraTransform.Translate(cameraPos + cameraMove);
    m_Scene.camera->TransformCamera(&cameraTransform);

    Vector3 lightPos = m_Scene.light->position;
    if (PGInput::IsKeyPressed(PGKEY_I)) {
        lightPos.z += 5.0f * deltaTime;
    }
    else if (PGInput::IsKeyPressed(PGKEY_K)) {
        lightPos.z -= 5.0f * deltaTime;
    }

    if (PGInput::IsKeyPressed(PGKEY_J)) {
        lightPos.x -= 5.0f * deltaTime;
    }
    else if (PGInput::IsKeyPressed(PGKEY_L)) {
        lightPos.x += 5.0f * deltaTime;
    }
    m_Scene.light->position = lightPos;

}

void Application::OnUIRender() {
    static bool* p_open = new bool(true);
    bool opt_fullscreen = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", p_open, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    ImGui::End();

    DrawPerformanceWindow();

    if (ImGui::Begin("Material")) {
        ImGui::SliderFloat("Roughness", &m_DefaultMaterial->roughness, 0.0f, 1.0f);
        ImGui::SliderFloat("Metallic", &m_DefaultMaterial->metallic, 0.0f, 1.0f);
        ImGui::ColorPicker3("Albedo", &m_DefaultMaterial->diffuseColor.x);
    }
    ImGui::End();

    DrawLogWindow();
}

void Application::OnExit() {
}

