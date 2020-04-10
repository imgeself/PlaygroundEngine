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
    PGDirectionalLight* sunLight = new PGDirectionalLight;
    sunLight->direction = -lightPosition;
    sunLight->color = Vector3(1.0f, 0.8f, 0.6f);
    sunLight->intensity = 4.0f;

    m_Scene.camera = &m_MainCamera;
    m_Scene.directionalLight = sunLight;

    PGTexture* skybox = (PGTexture*) PGResourceManager::CreateResource("./assets/beach/beach.dds");
    PGTexture* irradiance = (PGTexture*) PGResourceManager::CreateResource("./assets/beach/beachirradiance.dds");
    PGTexture* radiance = (PGTexture*) PGResourceManager::CreateResource("./assets/beach/beachradiance.dds");
    PGTexture* brdf = (PGTexture*)PGResourceManager::CreateResource("./assets/beach/brdfLUT.dds");

    m_Scene.skyboxTexture = skybox;

    PGRenderer::BeginScene(&m_Scene);

    m_DefaultMaterial = new Material;
    memset(m_DefaultMaterial, 0, sizeof(Material));
    m_DefaultMaterial->diffuseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    m_DefaultMaterial->ambientColor = Vector4(0.03f, 0.03f, 0.03f, 1.0f);
    m_DefaultMaterial->roughness = 1.0f;
    m_DefaultMaterial->metallic = 0.0f;
    m_DefaultMaterial->shader = m_PBRShader;
    m_DefaultMaterial->radianceMap = radiance;
    m_DefaultMaterial->irradianceMap = irradiance;
    m_DefaultMaterial->envBrdf = brdf;

    m_DefaultMaterial->doubleSided = true;
    m_DefaultMaterial->alphaMode = AlphaMode_BLEND_OVER_OP;


    //LoadSceneFromGLTFFile(PGRenderer::GetRendererAPI(), &m_Scene, m_DefaultMaterial, "./assets/DamagedHelmet/DamagedHelmet.gltf", false);
    LoadSceneFromGLTFFile(PGRenderer::GetRendererAPI(), &m_Scene, m_DefaultMaterial, "./assets/Sponza/Sponza.gltf", true);
    //LoadSceneFromGLTFFile(PGRenderer::GetRendererAPI(), &m_Scene, m_DefaultMaterial, "./assets/AntiqueCamera.glb", true);

    Transform cameraTransform;
    cameraTransform.Translate(Vector3(0.0f,-0.5f, -4.0f));
    m_MainCamera.TransformCamera(&cameraTransform);

    PGPointLight pointLight = {};
    pointLight.position = Vector3(-3.0, 1.0f, 1.0f);
    pointLight.color = Vector3(0.0f, 0.8f, 1.0f);
    pointLight.intensity = 5.0f;

    PGPointLight pointLight2 = {};
    pointLight2.position = Vector3(3.0, 1.0f, 1.0f);
    pointLight2.color = Vector3(1.0f, 0.0f, 1.0f);
    pointLight2.intensity = 5.0f;

    PGPointLight pointLight3 = {};
    pointLight3.position = Vector3(-3.0, 5.0f, 3.0f);
    pointLight3.color = Vector3(0.0f, 0.8f, 0.0f);
    pointLight3.intensity = 4.0f;

    PGPointLight pointLight4 = {};
    pointLight4.position = Vector3(-5.0, 5.0f, -2.0f);
    pointLight4.color = Vector3(1.0f, 0.0f, 0.0f);
    pointLight4.intensity = 3.0f;

    m_Scene.pointLights.push_back(pointLight);
    m_Scene.pointLights.push_back(pointLight2);
    m_Scene.pointLights.push_back(pointLight3);
    m_Scene.pointLights.push_back(pointLight4);

    PGSpotLight spotLight = {};
    spotLight.position = Vector3(0.0f, 1.0f, 0.5f);
    spotLight.minConeAngleCos = std::cosf(PI / 6.0f);
    spotLight.direction = Vector3(-1.0f, -0.5f, 0.0f);
    spotLight.maxConeAngleCos = std::cosf(PI / 4.0f);
    spotLight.color = Vector3(1.0f, 1.0f, 0.3f);
    spotLight.intensity = 2.0f;

    PGSpotLight spotLight2 = {};
    spotLight2.position = Vector3(0.0f, 1.0f, -0.5f);
    spotLight2.minConeAngleCos = std::cosf(PI / 6.0f);
    spotLight2.direction = Vector3(-1.0f, -0.5f, 0.0f);
    spotLight2.maxConeAngleCos = std::cosf(PI / 4.0f);
    spotLight2.color = Vector3(1.0f, 1.0f, 0.3f);
    spotLight2.intensity = 2.0f;

    m_Scene.spotLights.push_back(spotLight);
    m_Scene.spotLights.push_back(spotLight2);



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

    Vector3 lightDirection = m_Scene.directionalLight->direction;
    if (PGInput::IsKeyPressed(PGKEY_I)) {
        lightDirection.z += 15.0f * deltaTime;
    }
    else if (PGInput::IsKeyPressed(PGKEY_K)) {
        lightDirection.z -= 15.0f * deltaTime;
    }

    if (PGInput::IsKeyPressed(PGKEY_J)) {
        lightDirection.x -= 15.0f * deltaTime;
    }
    else if (PGInput::IsKeyPressed(PGKEY_L)) {
        lightDirection.x += 15.0f * deltaTime;
    }

    if (PGInput::IsKeyPressed(PGKEY_U)) {
        lightDirection.y -= 15.0f * deltaTime;
    }
    else if (PGInput::IsKeyPressed(PGKEY_O)) {
        lightDirection.y += 15.0f * deltaTime;
    }
    m_Scene.directionalLight->direction = lightDirection;

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

    if (ImGui::Begin("Renderer Settings")) {
        const char* msaaSampleItemTexts[] = { "MSAA OFF", "MSAA 2x", "MSAA 4x", "MSAA 8x" };
        const uint32_t msaaSampleItemValues[] = { 1, 2, 4, 8 };

        static int msaaSampleCountItem = 0;
        if (ImGui::Combo("MSAA", &msaaSampleCountItem, msaaSampleItemTexts, IM_ARRAYSIZE(msaaSampleItemTexts))) {
            PGRenderer::SetMSAASampleCount(msaaSampleItemValues[msaaSampleCountItem]);
        }

        const char* shadowResolutionItemTexts[] = { "256", "512", "1024", "2048", "4096" };
        const uint32_t shadowResolutionItemValues[] = { 256, 512, 1024, 2048, 4096 };

        static int shadowResolutionItem = 2;
        if (ImGui::Combo("Shadow Resolution", &shadowResolutionItem, shadowResolutionItemTexts, IM_ARRAYSIZE(shadowResolutionItemTexts))) {
            PGRenderer::SetShadowMapResolution(shadowResolutionItemValues[shadowResolutionItem]);
        }

        const char* anisotropyItemTexts[] = { "1x", "2x", "4x", "8x", "16x" };
        const uint32_t anisotropyItemValues[] = { 1, 2, 4, 8, 16 };

        static int anisotropyCountItem = 4;
        if (ImGui::Combo("Anisotropy Level", &anisotropyCountItem, anisotropyItemTexts, IM_ARRAYSIZE(anisotropyItemTexts))) {
            PGRenderer::SetAnisotropyLevel(anisotropyItemValues[anisotropyCountItem]);
        }

        static bool renderBoundingBoxes = false;
        if (ImGui::Checkbox("Render Bounding Boxes", &renderBoundingBoxes)) {
            PGRenderer::DrawBoundingBoxes(renderBoundingBoxes);
        }

        static bool renderCascadeColors = false;
        if (ImGui::Checkbox("Visualize Cascades", &renderCascadeColors)) {
            PGRenderer::DrawCascadeColors(renderCascadeColors);
        }
    }
    ImGui::End();

    DrawLogWindow();
}

void Application::OnExit() {
}

