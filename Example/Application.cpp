#include "Application.h"
#include "LogWindow.h"

#include <stdio.h>
#include <time.h>
#include <Math/math_util.h>
#include <Platform/PGTime.h>

#include <PGLog.h>

Application::Application(PGSystem* system) 
    : m_System(system) {
}

Application::~Application() {
}

void Application::OnInit() {
    PGCamera* mainCamera = new PGCamera;
    mainCamera->SetFrustum(1280, 720, 0.01f, 100.0f, PI / 4.0f);
    //mainCamera->SetView(Vector3(0.0f, 10.0f, -10.0f), Vector3(0.0f, 0.0f, 1.0f));

    PGShaderLib* shaderLib = m_System->GetShaderLib();
    m_PBRShader = shaderLib->GetDefaultShader("PBRForward");

    Vector3 lightPosition(20.0f, 24.0f, -20.0f);
    PGLight* mainLight = new PGLight;
    mainLight->position = lightPosition;

    m_Scene.camera = mainCamera;
    m_Scene.light = mainLight;

    PGTexture* skybox = (PGTexture*) PGResourceManager::CreateResource("./assets/envmap/environment.dds");
    PGTexture* irradiance = (PGTexture*) PGResourceManager::CreateResource("./assets/envmap/irradiance.dds");
    PGTexture* radiance = (PGTexture*) PGResourceManager::CreateResource("./assets/envmap/radiance.dds");
    PGTexture* brdf = (PGTexture*)PGResourceManager::CreateResource("./assets/envmap/brdfLUT.dds");

    m_Scene.skybox = new Skybox(skybox);

    PGTexture* albedoTexture = (PGTexture*) PGResourceManager::CreateResource("./assets/monkey/albedo.png");
    PGTexture* roughnessTexture = (PGTexture*)PGResourceManager::CreateResource("./assets/monkey/roughness.png");
    PGTexture* metallicTexture = (PGTexture*)PGResourceManager::CreateResource("./assets/monkey/metallic.png");
    PGTexture* aoTexture = (PGTexture*)PGResourceManager::CreateResource("./assets/monkey/ao.png");

    PGRenderer::BeginScene(&m_Scene);

    Material* monkeyMaterial = new Material;
    memset(monkeyMaterial, 0, sizeof(Material));
    monkeyMaterial->ambientColor = Vector4(0.03f, 0.03f, 0.03f, 1.0f);
    monkeyMaterial->opacity = 1.0f;
    monkeyMaterial->indexOfRefraction = 1.0f;
    monkeyMaterial->shader = m_PBRShader;

    monkeyMaterial->albedoTexture = albedoTexture;
    monkeyMaterial->roughnessTexture = roughnessTexture;
    monkeyMaterial->metallicTexture = metallicTexture;
    monkeyMaterial->aoTexture = aoTexture;
    monkeyMaterial->radianceMap = radiance;
    monkeyMaterial->irradianceMap = irradiance;
    monkeyMaterial->envBrdf = brdf;

    monkeyMaterial->hasAlbedoTexture = 1;
    monkeyMaterial->hasRoughnessTexture = 1;
    monkeyMaterial->hasMetallicTexture = 1;
    monkeyMaterial->hasAOTexture = 1;


    m_DefaultMaterial = new Material;
    memset(m_DefaultMaterial, 0, sizeof(Material));
    m_DefaultMaterial->diffuseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    m_DefaultMaterial->ambientColor = Vector4(0.03f, 0.03f, 0.03f, 1.0f);
    m_DefaultMaterial->opacity = 1.0f;
    m_DefaultMaterial->indexOfRefraction = 1.0f;
    m_DefaultMaterial->roughness = 0.0f;
    m_DefaultMaterial->metallic = 0.0f;
    m_DefaultMaterial->shader = m_PBRShader;
    m_DefaultMaterial->radianceMap = radiance;
    m_DefaultMaterial->irradianceMap = irradiance;
    m_DefaultMaterial->envBrdf = brdf;

    Transform planeTransform;
    planeTransform.Scale(Vector3(100.0f, 1.0f, 100.0f));
    MeshRef planeMesh = m_System->GetDefaultMeshInstance("Plane");
    planeMesh->material = m_DefaultMaterial;
    planeMesh->transform = planeTransform;
    //PGRenderer::AddMesh(planeMesh);

    
    MeshRef sphereMesh = LoadMeshFromOBJFile("./assets/uvsphere.obj");
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) {
            // Transform
            Transform sphereTransform = {};
            sphereTransform.Translate(Vector3(-8.0f + i * 2.5f, 10.0f - j * 2.5f, 6.0f));
            // Material
            Material* sphereMaterial = new Material;
            memcpy(sphereMaterial, m_DefaultMaterial, sizeof(Material));
            sphereMaterial->roughness = i / 6.0f;
            sphereMaterial->metallic = j / 6.0f;
            MeshRef sphereMeshInstance = std::make_shared<Mesh>();
            *sphereMeshInstance = *sphereMesh;
            sphereMeshInstance->material = sphereMaterial;
            sphereMeshInstance->transform = sphereTransform;
            PGRenderer::AddMesh(sphereMeshInstance);
        }
    }

    MeshRef monkeyMesh = LoadMeshFromOBJFile("./assets/monkey/monkey.obj");
    monkeyMesh->material = monkeyMaterial;
    Transform monkeyTransform;
    monkeyTransform.Translate(Vector3(0.0f, 3.0f, 0.0f));
    monkeyMesh->transform = monkeyTransform;
    PGRenderer::AddMesh(monkeyMesh);

    /*
    uint32_t randomSeed = 38689 * 643 / 6 + 4;
    MeshRef cubeMesh = LoadMeshFromOBJFile("./assets/cube.obj");
    for (int i = 0; i < 5; ++i) {
        Transform cubeTransform;
        float random = RandomBilateral(&randomSeed) * 5;
        cubeTransform.Translate(Vector3(-8.0f + i * 4, 1.0f, 2.0f+random));
        MeshRef cubeMeshInstance = std::make_shared<Mesh>();
        *cubeMeshInstance = *cubeMesh;
        cubeMeshInstance->material = m_DefaultMaterial;
        cubeMeshInstance->transform = cubeTransform;
        PGRenderer::AddMesh(cubeMeshInstance);
    }

    

    MeshRef sphereMesh = LoadMeshFromOBJFile("./assets/uvsphere.obj");
    sphereMesh->material = m_DefaultMaterial;
    Transform sphereTransform;
    sphereTransform.Translate(Vector3(3.0f, 3.0f, 1.0f));
    sphereMesh->transform = sphereTransform;

    PGRenderer::AddMesh(sphereMesh);
    */
    PGRenderer::EndScene();
}



std::vector<float> frameTimes;

void Application::OnUpdate(float deltaTime) {
    if (frameTimes.size() < 32) {
        frameTimes.push_back(deltaTime);
    } else {
        std::rotate(frameTimes.begin(), frameTimes.begin() + 1, frameTimes.end());
        frameTimes[frameTimes.size() - 1] = deltaTime;
    }

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

    if (ImGui::Begin("Performance")) {
        ImGui::PlotLines("", frameTimes.data(), (int)frameTimes.size(), 0, "Frame Time", 0.0f, 0.038f, ImVec2(0, 80));
        ImGui::Text("Frame %.3f ms", frameTimes.back() * 1000.0f);
    }
    ImGui::End();

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

