#include "../PGWindow.h"
#include "../PGInput.h"
#include "../../Imgui/imgui.h"


LRESULT CALLBACK PGWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    PGWindow* window = nullptr;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<PGWindow*>(create->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) window);

        window->m_Handle = hwnd;
    } else {
        window = reinterpret_cast<PGWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window) {
        return window->HandleMessage(uMsg, wParam, lParam);
    } else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
};

PGWindow::PGWindow(const char* name, uint32_t width, uint32_t height, PGSystemEventDispatcher* eventDispatcher) 
    : m_Width(width)
    , m_Height(height) {
    
    HINSTANCE hInstance = GetModuleHandle(0);
    SetProcessDPIAware();

    const char* windowClassName = "WindowClass";
    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc = &PGWindow::WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = windowClassName;

    RegisterClass(&windowClass);

    HWND windowHandle = CreateWindowEx(0,
        windowClassName,
        name,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        NULL,
        NULL,
        hInstance,
        this);

    m_Handle = windowHandle;
    m_SystemEventDispatcher = eventDispatcher;
}

PGWindow::~PGWindow() {
    DestroyWindow(m_Handle);
}

WindowHandle PGWindow::GetWindowHandle() {
    return m_Handle;
}

uint32_t PGWindow::GetWidth() {
    return m_Width;
}

uint32_t PGWindow::GetHeight() {
    return m_Height;
}

Vector2 PGWindow::GetClientSize() {
    RECT rect;
    GetClientRect(m_Handle, &rect);
    return Vector2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
}


void PGWindow::Show() {
    ShowWindow(m_Handle, SW_SHOWDEFAULT);
}

bool PGWindow::ProcessMessages() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_QUIT || msg.message == WM_DESTROY) {
            return false;
        }
    }

    return true;
}

LRESULT PGWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        } return 0;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            PGInput::keyPressedState[wParam] = true;
        } break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            PGInput::keyPressedState[wParam] = false;
        } break;
        case WM_LBUTTONDOWN:
        {
            PGInput::mouseButtonPressedState[PGMOUSE_LBUTTON] = true;
        } break;
        case WM_RBUTTONDOWN:
        {
            PGInput::mouseButtonPressedState[PGMOUSE_RBUTTON] = true;
        } break;
        case WM_MBUTTONDOWN:
        {
            PGInput::mouseButtonPressedState[PGMOUSE_MBUTTON] = true;
        } break;
        case WM_LBUTTONUP:
        {
            PGInput::mouseButtonPressedState[PGMOUSE_LBUTTON] = false;
        } break;
        case WM_RBUTTONUP:
        {
            PGInput::mouseButtonPressedState[PGMOUSE_RBUTTON] = false;
        } break;
        case WM_MBUTTONUP:
        {
            PGInput::mouseButtonPressedState[PGMOUSE_MBUTTON] = false;
        } break;
        case WM_CHAR:
        {
            // NOTE: We may need to store input characters for our input system too.
            ImGuiIO& io = ImGui::GetIO();
            io.AddInputCharacter((unsigned int) wParam);
        } break;
        case WM_SIZE:
        {
            m_Width = LOWORD(lParam);
            m_Height = HIWORD(lParam);

            RECT rect;
            GetClientRect(m_Handle, &rect);
            if (m_SystemEventDispatcher) {
                m_SystemEventDispatcher->DispatchSystemEvent(SystemEvent::RESIZE, (uint64_t)(rect.right - rect.left), (uint64_t)(rect.bottom - rect.top));
            }
        } break;
    }

    return DefWindowProc(m_Handle, uMsg, wParam, lParam);
}


