#pragma once

#ifdef PLATFORM_WINDOWS
#include "Win32/PGPlatformDefinesWin32.h"
#endif // PLATFORM_WINDOWS

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

#ifdef PLATFORM_WINDOWS
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
};
