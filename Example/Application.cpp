#include "Application.h"

#include <stdio.h>
#include <time.h>
#include <Math/math_util.h>

Application::Application(PGSystem* system) 
    : m_System(system) {
}

Application::~Application() {
    printf("Destructor \n");
}

void Application::OnInit() {
    printf("init \n");

    const float vertexData[] = {
     -1.0f, -1.0f,  -1.0f,
     -1.0f,  1.0f,  -1.0f,
      1.0f,  1.0f,  -1.0f,
      1.0f, -1.0f,  -1.0f,
     -1.0f, -1.0f,  1.0f,
     -1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,
      1.0f, -1.0f,  1.0f,
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
    std::shared_ptr<IVertexBuffer> vertexBuffer(rendererAPI->CreateVertexBuffer((void*)vertexData, sizeof(vertexData)));

    uint32_t indicesCount = sizeof(indices) / sizeof(uint32_t);
    m_IndexBuffer = std::shared_ptr<IIndexBuffer>(rendererAPI->CreateIndexBuffer(indices, indicesCount));

    const char* vertexShaderFileName = "../bin/shaders/VertexShader.cso";
    const char* pixelShaderFileName = "../bin/shaders/PixelShader.cso";
    std::shared_ptr<IShaderProgram> shaderProgram(rendererAPI->CreateShaderProgram(vertexShaderFileName, pixelShaderFileName));

    std::vector<VertexInputElement> inputElements = {
        { "POSITION", VertexDataFormat_FLOAT3, 0, 0 }
    };

    std::shared_ptr<IVertexInputLayout> inputLayout(rendererAPI->CreateVertexInputLayout(inputElements, shaderProgram.get()));

    std::shared_ptr<IConstantBuffer> psConstantBuffer(rendererAPI->CreateConstantBuffer(colors, sizeof(colors)));

    // Bindings
    UINT stride = sizeof(float) * 3;
    rendererAPI->SetVertexBuffer(vertexBuffer.get(), stride);
    rendererAPI->SetIndexBuffer(m_IndexBuffer.get());
    rendererAPI->SetInputLayout(inputLayout.get());
    rendererAPI->SetShaderProgram(shaderProgram.get());
    rendererAPI->SetConstanBufferPS(psConstantBuffer.get());
}

void Application::OnUpdate() {

}

void Application::OnRender() {
    IRendererAPI* rendererAPI = m_System->GetRendererApi();

    struct ConstantBuffer {
        Matrix4 transform;
    } cBuff;

    cBuff.transform = IdentityMatrix;

    Vector3 translate(0.0f, 0.0f, 6.0f);
    Matrix4 transMatrix = TranslateMatrix(translate);

    time_t time = clock();
    float seed = time % 125263 / 662.9f;
    Matrix4 xAxisRotate = RotateMatrixXAxis(seed);
    Matrix4 yAxisRotate = RotateMatrixYAxis(seed);
    Matrix4 rotateMatrix = yAxisRotate * xAxisRotate;

    Matrix4 projMatrix = PerspectiveMatrix(1280, 720, 0.01f, 100.f, PI / 4.0f);

    cBuff.transform = projMatrix * transMatrix * rotateMatrix * cBuff.transform;

    std::shared_ptr<IConstantBuffer> vsConstantBuffer(rendererAPI->CreateConstantBuffer(&cBuff, sizeof(ConstantBuffer)));
    rendererAPI->SetConstanBufferVS(vsConstantBuffer.get());

    rendererAPI->DrawIndexed(m_IndexBuffer.get());
}

void Application::OnExit() {
    printf("Exit \n");

}

