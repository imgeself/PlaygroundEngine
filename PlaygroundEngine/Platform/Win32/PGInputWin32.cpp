#include "../PGInput.h"

bool PGInput::keyPressedState[256];

bool PGInput::IsKeyPressed(KeyCode keycode) {
    return keyPressedState[keycode];
};