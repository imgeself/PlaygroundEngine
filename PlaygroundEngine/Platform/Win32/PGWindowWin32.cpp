#include "../PGWindow.h"

LRESULT CALLBACK PGWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_CREATE) {
        LONG_PTR window = (LONG_PTR)reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, window);
    }

    PGWindow* window = hwnd ? reinterpret_cast<PGWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)) : NULL;

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
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
};

PGWindow::PGWindow(const char* name, int width, int height) {
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
