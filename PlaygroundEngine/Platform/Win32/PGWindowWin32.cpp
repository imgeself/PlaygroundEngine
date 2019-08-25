#include "../PGWindow.h"

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
        window->HandleMessage(uMsg, wParam, lParam);
    } else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
};

PGWindow::PGWindow(const char* name, uint32_t width, uint32_t height) 
    : m_Width(width)
    , m_Height(height) {
    HINSTANCE hInstance = GetModuleHandle(0);

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

void PGWindow::Show() {
    ShowWindow(m_Handle, SW_SHOWDEFAULT);
}

// Procces all window messages
// Returns false if quit requested otherwise returns true
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
    }

    return DefWindowProc(m_Handle, uMsg, wParam, lParam);
}


