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


    float colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
    };

    PGCamera* mainCamera = new PGCamera;
    mainCamera->SetFrustum(1280, 720, 0.001f, 100.0f, PI / 4.0f);
    mainCamera->SetView(Vector3(0.0f, 1.0f, -10.0f), Vector3(0.0f, 0.0f, 0.0f));

    PGShaderLib* shaderLib = m_System->GetShaderLib();
    m_CubeShader = shaderLib->LoadShaderFromDisk("Shaders/PhongShader.hlsl");

    m_LightCubeShader = shaderLib->LoadShaderFromDisk("Shaders/LightCube.hlsl");
    Material lightCubeMaterial = { m_LightCubeShader };
    Transform lightCubeTransform;
    Vector3 lightPosition(2.0f, 24.0f, -5.0f);
    lightCubeTransform.Translate(lightPosition);
    lightCubeTransform.Scale(Vector3(0.3f, 0.3f, 0.3f));
    MeshRef lightCubeMesh = m_System->GetDefaultMeshInstance("Cube");
    lightCubeMesh->material = lightCubeMaterial;
    lightCubeMesh->transform = lightCubeTransform;

    PGLight* mainLight = new PGLight;
    mainLight->position = lightPosition;

    m_Scene.camera = mainCamera;
    m_Scene.light = mainLight;

    PGRenderer::BeginScene(&m_Scene);

    Material cubeMaterial = { m_CubeShader };
    uint32_t randomSeed = 38689 * 643 / 6 + 4;
    for (int i = 0; i < 5; ++i) {
        Transform cubeTransform;
        float random = RandomBilateral(&randomSeed) * 5;
        cubeTransform.Translate(Vector3(-8.0f + i * 4, 1.0f, 2.0f+random));
        MeshRef cubeMesh = m_System->GetDefaultMeshInstance("Cube");
        cubeMesh->material = cubeMaterial;
        cubeMesh->transform = cubeTransform;
        PGRenderer::AddMesh(cubeMesh);
    }

    MeshRef monkeyMesh = LoadMeshFromOBJFile("./monkey.obj");
    monkeyMesh->material = cubeMaterial;
    Transform monkeyTransform;
    monkeyTransform.Translate(Vector3(0.0f, 3.0f, 0.0f));
    //monkeyTransform.RotateYAxis(PI);
    monkeyMesh->transform = monkeyTransform;

    MeshRef sphereMesh = LoadMeshFromOBJFile("./uvsphere.obj");
    sphereMesh->material = cubeMaterial;
    Transform sphereTransform;
    sphereTransform.Translate(Vector3(3.0f, 3.0f, 1.0f));
    sphereMesh->transform = sphereTransform;

    Transform planeTransform;
    planeTransform.Scale(Vector3(100.0f, 1.0f, 100.0f));
    MeshRef planeMesh = m_System->GetDefaultMeshInstance("Plane");
    planeMesh->material = cubeMaterial;
    planeMesh->transform = planeTransform;
    PGRenderer::AddMesh(planeMesh);
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
}

void Application::OnExit() {
    printf("Exit \n");

}

