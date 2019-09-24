#include "Application.h"

#include <stdio.h>
#include <time.h>
#include <Math/math_util.h>
#include <Platform/PGTime.h>

Application::Application(PGSystem* system) 
    : m_System(system) {
}

Application::~Application() {
    printf("Destructor \n");
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
    mainCamera->SetFrustum(1280, 720, 0.001, 1000.0f, PI / 4.0f);
    mainCamera->SetView(Vector3(0.0f, 0.0f, -10.0f), Vector3(0.0f, 0.0f, 0.0f));

    PGShaderLib* shaderLib = m_System->GetShaderLib();
    m_CubeShader = shaderLib->LoadShaderFromDisk("Shaders/PhongShader.hlsl");

    Material cubeMaterial = { m_CubeShader.get() };
    Transform cubeTransform;
    m_CubeMesh = m_System->GetDefaultMeshInstance("Cube", cubeMaterial, cubeTransform);

    m_LightCubeShader = shaderLib->LoadShaderFromDisk("Shaders/LightCube.hlsl");
    Material lightCubeMaterial = { m_LightCubeShader.get() };
    Transform lightCubeTransform;
    Vector3 lightPosition(2.0f, 2.0f, -2.0f);
    lightCubeTransform.Translate(lightPosition);
    lightCubeTransform.Scale(Vector3(0.3f, 0.3f, 0.3f));
    m_LightCubeMesh = m_System->GetDefaultMeshInstance("Cube", lightCubeMaterial, lightCubeTransform);

    PGLight* mainLight = new PGLight;
    mainLight->position = lightPosition;

    m_Scene.camera = mainCamera;
    m_Scene.light = mainLight;

    PGRenderer::BeginScene(&m_Scene);

    PGRenderer::AddRenderObject(m_CubeMesh);
    PGRenderer::AddRenderObject(m_LightCubeMesh);
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

    float seed = 2.0f * deltaTime;
    m_CubeMesh->transform.RotateXAxis(seed);
    m_CubeMesh->transform.RotateYAxis(seed);

    m_LightCubeMesh->transform.SetPosition(lightPos);

    PGRenderer::EndScene();

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

