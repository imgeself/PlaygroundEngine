#include "../PGInput.h"

bool PGInput::keyPressedState[256];
bool PGInput::mouseButtonPressedState[5];

inline Vector2 PGInput::GetMousePos(PGWindow* window) {
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(window->GetWindowHandle(), &point);

    return { (float) point.x, (float) point.y };
}


