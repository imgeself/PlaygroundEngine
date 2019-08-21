#pragma once

#ifdef PLATFORM_WIN32
#include "Win32/PGPlatformDefinesWin32.h"
#endif // PLATFORM_WIN32

#include "PGInput.h"

#include <set>

class PGWindow {
public:
    PGWindow(const char* name, int width, int height);
    ~PGWindow();

    void Show();
    bool ProcessMessages();

private:
    WindowHandle m_Handle;

#ifdef PLATFORM_WIN32
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
};
