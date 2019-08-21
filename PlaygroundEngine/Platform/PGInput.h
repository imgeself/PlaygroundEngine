#pragma once

#include "PGKeycode.h"

class PGInput {
    friend class PGWindow;
public:
    static bool IsKeyPressed(KeyCode keycode);

private:
    static bool keyPressedState[256];
};

