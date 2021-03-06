#pragma once

#ifdef PLATFORM_WINDOWS
#include "Win32/PGPlatformDefinesWin32.h"
#endif // PLATFORM_WINDOWS

#include "../Core.h"
#include "../Events/PGSystemEventDispatcher.h"
#include "../Math/math_util.h"

class PG_API PGWindow {
public:
    PGWindow(const char* name, uint32_t width, uint32_t height, PGSystemEventDispatcher* eventDispatcher);
    ~PGWindow();

    void Show();

    // Processes all window messages
    // Returns false if quit requested otherwise returns true
    bool ProcessMessages();

    WindowHandle GetWindowHandle();
    uint32_t GetWidth();
    uint32_t GetHeight();
    Vector2 GetClientSize();

private:
    WindowHandle m_Handle;
    uint32_t m_Width;
    uint32_t m_Height;
    PGSystemEventDispatcher* m_SystemEventDispatcher = nullptr;

#ifdef PLATFORM_WINDOWS
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
};
