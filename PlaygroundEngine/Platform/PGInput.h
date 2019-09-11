#pragma once

#include "PGKeycode.h"
#include "PGWindow.h"
#include "../Math/math_util.h"

#include <utility>

class PG_API PGInput {
    friend class PGWindow;
public:
    inline static bool IsKeyPressed(KeyCode keycode);
    inline static bool IsMouseButtonPressed(KeyCode keycode);
    inline static Vector2 GetMousePos();

private:
    PGInput();

    static bool keyPressedState[256];
    static bool mouseButtonPressedState[5];
};

