#include "Application.h"

#include <stdio.h>

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

    // Bindings
    UINT stride = sizeof(float) * 3;
    rendererAPI->SetVertexBuffer(vertexBuffer.get(), stride);
    rendererAPI->SetIndexBuffer(m_IndexBuffer.get());
    rendererAPI->SetInputLayout(inputLayout.get());
    rendererAPI->SetShaderProgram(shaderProgram.get());
}

void Application::OnUpdate() {

}

void Application::OnRender() {
    m_System->GetRendererApi()->DrawIndexed(m_IndexBuffer.get());
}

void Application::OnExit() {
    printf("Exit \n");

}

