#include "../PGInput.h"

bool PGInput::keyPressedState[256];
bool PGInput::mouseButtonPressedState[5];

inline bool PGInput::IsKeyPressed(KeyCode keycode) {
    return keyPressedState[keycode];
}

inline bool PGInput::IsMouseButtonPressed(KeyCode keycode) {
    return mouseButtonPressedState[keycode];
}

inline Vector2 PGInput::GetMousePos() {
    POINT point;
    GetCursorPos(&point);
    HWND foregroundWindow = GetForegroundWindow();
    ScreenToClient(foregroundWindow, &point);

    return { (float) point.x, (float) point.y };
}


