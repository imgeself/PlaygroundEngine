#pragma once

#include "PGCamera.h"
#include "Renderer/Skybox.h"

struct PGLight {
    Vector3 position;
    Vector3 color;
    float intensity;
};

struct PGSceneObject {
    MeshRef mesh;
    Transform transform;
};

struct PGScene {
    PGCamera* camera;
    PGLight* directionalLight;
    PGTexture* skyboxTexture;
    Box boundingBox;
    std::vector<PGSceneObject> sceneObjects;
};

