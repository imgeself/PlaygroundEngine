#pragma once

#include "PGCamera.h"
#include "Renderer/Skybox.h"

struct PGLight {
    Vector3 position;
    //Vector3 color;
    //float intensity;
    //float radius;
};

struct PGSceneObject {
    MeshRef mesh;
    Transform transform;
};

struct PGScene {
    PGCamera* camera;
    PGLight* light;
    PGTexture* skyboxTexture;
    std::vector<PGSceneObject> sceneObjects;
};

