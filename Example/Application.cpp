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

    const float vertexData[] = {
     -1.0f, -1.0f,  -1.0f, // 0
     -1.0f,  1.0f,  -1.0f, // 1
      1.0f,  1.0f,  -1.0f, // 2
      1.0f, -1.0f,  -1.0f, // 3
     -1.0f, -1.0f,  1.0f, // 4
     -1.0f,  1.0f,  1.0f, // 5
      1.0f,  1.0f,  1.0f, // 6
      1.0f, -1.0f,  1.0f, // 7
    };

    const float vertexDataArray[] = {
     -1.0f, -1.0f,  -1.0f, 0.0f, 0.0f, -1.0f,
     -1.0f,  1.0f,  -1.0f, 0.0f, 0.0f, -1.0f,
      1.0f,  1.0f,  -1.0f, 0.0f, 0.0f, -1.0f,

     -1.0f, -1.0f,  -1.0f, 0.0f, 0.0f, -1.0f,
      1.0f,  1.0f,  -1.0f, 0.0f, 0.0f, -1.0f,
      1.0f, -1.0f,  -1.0f, 0.0f, 0.0f, -1.0f,

      -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // 4
      1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f, // 6
     -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f, // 5

     -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,// 4
      1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,// 7
      1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f,// 6 

      -1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, // 4
     -1.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,// 5
     -1.0f,  1.0f,  -1.0f, -1.0f, 0.0f, 0.0f,// 1

      -1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f,// 4
      -1.0f,  1.0f,  -1.0f, -1.0f, 0.0f, 0.0f,// 1
      -1.0f, -1.0f,  -1.0f, -1.0f, 0.0f, 0.0f,// 0

      1.0f, -1.0f,  -1.0f, 1.0f, 0.0f, 0.0f,// 3
      1.0f,  1.0f,  -1.0f, 1.0f, 0.0f, 0.0f,// 2
      1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f,// 6

      1.0f, -1.0f,  -1.0f, 1.0f, 0.0f, 0.0f,// 3
      1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f,// 6
      1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f,// 7

     -1.0f,  1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,// 1
     -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f,// 5
      1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f,// 6

      -1.0f,  1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,// 1
      1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f,// 6
      1.0f,  1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,// 2

      -1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f,// 4
     -1.0f, -1.0f,  -1.0f, 0.0f, -1.0f, 0.0f,// 0
      1.0f, -1.0f,  -1.0f, 0.0f, -1.0f, 0.0f,// 3

      -1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f,// 4
      1.0f, -1.0f,  -1.0f, 0.0f, -1.0f, 0.0f,// 3
      1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f// 7

    };

    float colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
    };

    uint32_t indices[] = {
        0, 1, 2,
        0, 2, 3,
        4, 6, 5,
        4, 7, 6,
        4, 5, 1,
        4, 1, 0,
        3, 2, 6,
        3, 6, 7,
        1, 5, 6,
        1, 6, 2,
        4, 0, 3,
        4, 3, 7
    };

    IRendererAPI* rendererAPI = m_System->GetRendererApi();
    uint32_t stride = sizeof(float) * 6;
    m_VertexBuffer = std::shared_ptr<IVertexBuffer>(rendererAPI->CreateVertexBuffer((void*)vertexDataArray, sizeof(vertexDataArray), stride));

    uint32_t indicesCount = sizeof(indices) / sizeof(uint32_t);
    //m_IndexBuffer = std::shared_ptr<IIndexBuffer>(rendererAPI->CreateIndexBuffer(indices, indicesCount));

    const char* vertexShaderFileName = "../bin/shaders/VertexShader.cso";
    const char* pixelShaderFileName = "../bin/shaders/PixelShader.cso";
    std::shared_ptr<IShaderProgram> shaderProgram(rendererAPI->CreateShaderProgram(vertexShaderFileName, pixelShaderFileName));

    std::vector<VertexInputElement> inputElements = {
        { "Position", VertexDataFormat_FLOAT3, 0, 0 },
        { "Normal", VertexDataFormat_FLOAT3, 0, 12 }
    };

    std::shared_ptr<IVertexInputLayout> inputLayout(rendererAPI->CreateVertexInputLayout(inputElements, shaderProgram.get()));

    std::shared_ptr<IConstantBuffer> psConstantBuffer(rendererAPI->CreateConstantBuffer(colors, sizeof(colors)));

    // Bindings
    rendererAPI->SetVertexBuffer(m_VertexBuffer.get(), stride);
    //rendererAPI->SetIndexBuffer(m_IndexBuffer.get());
    rendererAPI->SetInputLayout(inputLayout.get());
    rendererAPI->SetShaderProgram(shaderProgram.get());
    rendererAPI->SetConstanBufferPS(psConstantBuffer.get());

    m_Camera.SetFrustum(1280, 720, 0.001, 1000.0f, PI / 4.0f);
    m_Camera.SetView(Vector3(0.0f, 0.0f, -10.0f), Vector3(0.0f, 0.0f, 0.0f));
}

std::vector<float> frameTimes;
void Application::OnUpdate(float deltaTime) {
    if (frameTimes.size() < 32) {
        frameTimes.push_back(deltaTime);
    } else {
        std::rotate(frameTimes.begin(), frameTimes.begin() + 1, frameTimes.end());
        frameTimes[frameTimes.size() - 1] = deltaTime;
    }

    IRendererAPI* rendererAPI = m_System->GetRendererApi();

    static Vector3 translate(0.0f, 0.0f, 0.0f);
    static Matrix4 transMatrix = TranslateMatrix(translate);
    static Matrix4 inverseTransMatrix = TranslateMatrix(-translate);
    static Vector4 lightPos(2.0f, 2.0f, -2.0f, 1.0f);
    static struct ConstantBuffer {
        Matrix4 transform = transMatrix;
        Matrix4 viewMatrix;
        Matrix4 projMatrix;
        Vector4 lightPos;
    } cBuff;

    Vector3 cameraPos = m_Camera.GetPosition();
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
    m_Camera.SetView(cameraPos, Vector3(0.0f, 0.0f, 0.0f));

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

    float seed = 0.0f;// *deltaTime;
    Matrix4 xAxisRotate = RotateMatrixXAxis(seed);
    Matrix4 yAxisRotate = RotateMatrixYAxis(seed);
    Matrix4 rotateMatrix = yAxisRotate * xAxisRotate;

    cBuff.transform = transMatrix * rotateMatrix;// *inverseTransMatrix* cBuff.transform;
    cBuff.viewMatrix = m_Camera.GetViewMatrix();
    cBuff.projMatrix = m_Camera.GetProjectionMatrix();
    cBuff.lightPos = lightPos;

    std::shared_ptr<IConstantBuffer> vsConstantBuffer(rendererAPI->CreateConstantBuffer(&cBuff, sizeof(ConstantBuffer)));
    rendererAPI->SetConstanBufferVS(vsConstantBuffer.get());
}

void Application::OnRender() {
    IRendererAPI* rendererAPI = m_System->GetRendererApi();
    //rendererAPI->DrawIndexed(m_IndexBuffer.get());
    rendererAPI->Draw(m_VertexBuffer.get());
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

