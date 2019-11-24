#include "Application.h"

#include <stdio.h>
#include <time.h>
#include <Math/math_util.h>
#include <Platform/PGTime.h>

Application::Application(PGSystem* system) 
    : m_System(system) {
}

Application::~Application() {
}

void Application::OnInit() {
    printf("init \n");

    PGCamera* mainCamera = new PGCamera;
    mainCamera->SetFrustum(1280, 720, 0.01f, 100.0f, PI / 4.0f);
    mainCamera->SetView(Vector3(0.0f, 1.0f, -10.0f), Vector3(0.0f, 0.0f, 1.0f));

    PGShaderLib* shaderLib = m_System->GetShaderLib();
    m_PBRShader = shaderLib->GetDefaultShader("PBRForward");

    Vector3 lightPosition(2.0f, 24.0f, -5.0f);
    PGLight* mainLight = new PGLight;
    mainLight->position = lightPosition;

    m_Scene.camera = mainCamera;
    m_Scene.light = mainLight;

    PGTexture* albedoTexture = (PGTexture*) PGResourceManager::CreateResource("./assets/monkey/albedo.png");
    PGTexture* roughnessTexture = (PGTexture*)PGResourceManager::CreateResource("./assets/monkey/roughness.png");
    PGTexture* metallicTexture = (PGTexture*)PGResourceManager::CreateResource("./assets/monkey/metallic.png");
    PGTexture* aoTexture = (PGTexture*)PGResourceManager::CreateResource("./assets/monkey/ao.png");

    PGRenderer::BeginScene(&m_Scene);

    Material* monkeyMaterial = new Material;
    memset(monkeyMaterial, 0, sizeof(Material));
    monkeyMaterial->emissiveColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    monkeyMaterial->ambientColor = Vector4(0.03f, 0.03f, 0.03f, 1.0f);
    monkeyMaterial->opacity = 1.0f;
    monkeyMaterial->indexOfRefraction = 1.0f;
    monkeyMaterial->shader = m_PBRShader;

    monkeyMaterial->albedoTexture = albedoTexture;
    monkeyMaterial->roughnessTexture = roughnessTexture;
    monkeyMaterial->metallicTexture = metallicTexture;
    monkeyMaterial->aoTexture = aoTexture;

    monkeyMaterial->hasAlbedoTexture = 1;
    monkeyMaterial->hasRoughnessTexture = 1;
    monkeyMaterial->hasMetallicTexture = 1;
    monkeyMaterial->hasAOTexture = 1;

    m_DefaultMaterial = new Material;
    memset(m_DefaultMaterial, 0, sizeof(Material));
    m_DefaultMaterial->diffuseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    m_DefaultMaterial->emissiveColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    m_DefaultMaterial->ambientColor = Vector4(0.03f, 0.03f, 0.03f, 1.0f);
    m_DefaultMaterial->opacity = 1.0f;
    m_DefaultMaterial->indexOfRefraction = 1.0f;
    m_DefaultMaterial->roughness = 0.3f;
    m_DefaultMaterial->metallic = 0.2f;
    m_DefaultMaterial->shader = m_PBRShader;

    Transform planeTransform;
    planeTransform.Scale(Vector3(100.0f, 1.0f, 100.0f));
    MeshRef planeMesh = m_System->GetDefaultMeshInstance("Plane");
    planeMesh->material = m_DefaultMaterial;
    planeMesh->transform = planeTransform;
    PGRenderer::AddMesh(planeMesh);

    
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

    MeshRef monkeyMesh = LoadMeshFromOBJFile("./assets/monkey/monkey.obj");
    monkeyMesh->material = monkeyMaterial;
    Transform monkeyTransform;
    monkeyTransform.Translate(Vector3(0.0f, 3.0f, 0.0f));
    //monkeyTransform.RotateYAxis(PI);
    monkeyMesh->transform = monkeyTransform;

    MeshRef sphereMesh = LoadMeshFromOBJFile("./assets/uvsphere.obj");
    sphereMesh->material = m_DefaultMaterial;
    Transform sphereTransform;
    sphereTransform.Translate(Vector3(3.0f, 3.0f, 1.0f));
    sphereMesh->transform = sphereTransform;

    PGRenderer::AddMesh(monkeyMesh);
    PGRenderer::AddMesh(sphereMesh);
    //PGRenderer::AddMesh(m_LightCubeMesh);
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


    Vector3 cameraPos = m_Scene.camera->GetPosition();
    if (PGInput::IsKeyPressed(PGKEY_W)) {
        cameraPos.y += 5.0f * deltaTime;
    } else if (PGInput::IsKeyPressed(PGKEY_S)) {
        cameraPos.y -= 5.0f * deltaTime;
    }

    if (PGInput::IsKeyPressed(PGKEY_A)) {
        cameraPos.x -= 5.0f * deltaTime;
    }
    else if (PGInput::IsKeyPressed(PGKEY_D)) {
        cameraPos.x += 5.0f * deltaTime;
    }
    m_Scene.camera->SetView(cameraPos, Vector3(0.0f, 0.0f, 0.0f));

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
    ImGui::Begin("Performance");
    ImGui::PlotLines("", frameTimes.data(), frameTimes.size(), 0, "Frame Time", 0.0f, 0.038f, ImVec2(0, 80));
    ImGui::Text("Frame %.3f ms", frameTimes.back() * 1000.0f);
    ImGui::End();

    ImGui::Begin("Material");
    ImGui::SliderFloat("Roughness", &m_DefaultMaterial->roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("Metallic", &m_DefaultMaterial->metallic, 0.0f, 1.0f);
    ImGui::End();
}

void Application::OnExit() {
    printf("Exit \n");

}

