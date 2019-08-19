#pragma once

#ifdef PLATFORM_WIN32
#include "Win32/PGPlatformDefinesWin32.h"
#endif // PLATFORM_WIN32

class PGWindow {
public:
    PGWindow(const char* name, int width, int height);
    ~PGWindow();

    void Show();
    bool ProcessMessages();

private:
    WindowHandle m_Handle;
};
