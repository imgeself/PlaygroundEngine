#pragma once

#include "imgui.h"
// We will use imgui_dx11 implementation for now.
// TODO: We should implement with our own graphics API.
#include "imgui_impl_dx11.h"
#include "../Platform/PGWindow.h"
#include "../Platform/PGInput.h"
#include "../Renderer/PGRendererAPI.h"

class IMGUI_IMPL_API ImguiModule {
public:
    ~ImguiModule();

    static void Initialize(PGWindow* window, IRendererAPI* rendererAPI);
    static void Begin(float deltaTime);
    static void Render();

private:
    static PGWindow* m_Window;

    ImguiModule();
};

