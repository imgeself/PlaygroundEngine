#pragma once

#include "PGKeycode.h"
#include "PGWindow.h"
#include "../Math/math_util.h"

#include <utility>

class PG_API PGInput {
    friend class PGWindow;
public:
    inline static bool IsKeyPressed(KeyCode keycode) {
        return keyPressedState[keycode];
    }

    inline static bool IsMouseButtonPressed(KeyCode keycode) {
        return mouseButtonPressedState[keycode];
    }

    inline static Vector2 GetMousePos(PGWindow* window);

private:
    PGInput();

    static bool keyPressedState[256];
    static bool mouseButtonPressedState[5];
};

