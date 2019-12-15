#pragma once

#include "PGCamera.h"
#include "Renderer/Skybox.h"

struct PGLight {
    Vector3 position;
    //Vector3 color;
    //float intensity;
    //float radius;
};

struct PGScene {
    PGCamera* camera;
    PGLight* light;
    Skybox* skybox;
};

