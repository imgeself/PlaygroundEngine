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

bool show_demo_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void Application::OnUIRender() {
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f
            = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)& clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
}

void Application::OnExit() {
    printf("Exit \n");

}

