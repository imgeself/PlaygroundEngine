#include "imgui_impl.h"

#include "../Renderer/DX11/DX11RendererAPI.h"

PGWindow* ImguiModule::m_Window = nullptr;

void ImguiModule::Initialize(PGWindow* window, IRendererAPI* rendererAPI) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = "imgui_impl";
    io.ImeWindowHandle = window->GetWindowHandle();

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_Tab] = PGKEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = PGKEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = PGKEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = PGKEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = PGKEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = PGKEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = PGKEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = PGKEY_HOME;
    io.KeyMap[ImGuiKey_End] = PGKEY_END;
    io.KeyMap[ImGuiKey_Insert] = PGKEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = PGKEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = PGKEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = PGKEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = PGKEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = PGKEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = PGKEY_ENTER;
    io.KeyMap[ImGuiKey_A] = PGKEY_A;
    io.KeyMap[ImGuiKey_C] = PGKEY_C;
    io.KeyMap[ImGuiKey_V] = PGKEY_V;
    io.KeyMap[ImGuiKey_X] = PGKEY_X;
    io.KeyMap[ImGuiKey_Y] = PGKEY_Y;
    io.KeyMap[ImGuiKey_Z] = PGKEY_Z;

    ImGui::CreateContext();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    DX11RendererAPI* renderer = (DX11RendererAPI*) rendererAPI;
    ImGui_ImplDX11_Init(renderer->GetDX11Device(), renderer->GetDX11DeviceContext());

    m_Window = window;
}

ImguiModule::~ImguiModule() {
    ImGui_ImplDX11_Shutdown();
    ImGui::DestroyContext();
}

void ImguiModule::Begin(float deltaTime) {
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    // Setup display size (every frame to accommodate for window resizing)
    Vector2 clientSize = m_Window->GetClientSize();
    io.DisplaySize = ImVec2(clientSize.x, clientSize.y);

    io.DeltaTime = deltaTime;

    // Read keyboard modifiers inputs
    io.KeyCtrl = PGInput::IsKeyPressed(PGKEY_LEFT_CONTROL) || PGInput::IsKeyPressed(PGKEY_RIGHT_CONTROL);
    io.KeyShift = PGInput::IsKeyPressed(PGKEY_LEFT_SHIFT) || PGInput::IsKeyPressed(PGKEY_RIGHT_SHIFT);
    io.KeyAlt = PGInput::IsKeyPressed(PGKEY_LEFT_ALT) || PGInput::IsKeyPressed(PGKEY_RIGHT_ALT);
    io.KeySuper = PGInput::IsKeyPressed(PGKEY_LEFT_SYSTEM) || PGInput::IsKeyPressed(PGKEY_RIGHT_SYSTEM);

    for (uint32_t i = 0; i < 256; ++i) {
        io.KeysDown[i] = PGInput::IsKeyPressed(i);
    }
    for (uint32_t i = 0; i < 3; ++i) {
        io.MouseDown[i] = PGInput::IsMouseButtonPressed(i);
    }

    // Update OS mouse position
    Vector2 mousePos = PGInput::GetMousePos();
    io.MousePos = ImVec2(mousePos.x, mousePos.y);

    ImGui::NewFrame();
}

void ImguiModule::Render() {
    // Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

