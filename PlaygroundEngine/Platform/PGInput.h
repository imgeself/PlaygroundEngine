#pragma once

#include "PGKeycode.h"
#include "PGWindow.h"
#include "../Math/math_util.h"

#include <utility>

class PG_API PGInput {
    friend class PGWindow;
public:
    static bool IsKeyPressed(KeyCode keycode);
    static bool IsMouseButtonPressed(KeyCode keycode);
    static Vector2 GetMousePos();

private:
    PGInput();

    static bool keyPressedState[256];
    static bool mouseButtonPressedState[5];
};

