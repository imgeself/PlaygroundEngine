#include "../PGWindow.h"

PGWindow::PGWindow(const char* name, int width, int height) {
    HINSTANCE hInstance = GetModuleHandle(0);

    const char* windowClassName = "WindowClass";
    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc = WindowProc;
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
        NULL);


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
