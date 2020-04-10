#pragma once

#include "PGCamera.h"
#include "Renderer/Skybox.h"

struct PGDirectionalLight {
    Vector3 direction;
    Vector3 color;
    float intensity;
};

struct PGPointLight {
    Vector3 position;
    Vector3 color;
    float intensity;
};

struct PGSpotLight {
    Vector3 position;
    float minConeAngleCos;
    Vector3 direction;
    float maxConeAngleCos;
    Vector3 color;
    float intensity;
};

struct PGSceneObject {
    MeshRef mesh;
    Transform transform;
};

struct PGScene {
    PGCamera* camera;
    PGDirectionalLight* directionalLight;
    std::vector<PGPointLight> pointLights;
    std::vector<PGSpotLight> spotLights;
    PGTexture* skyboxTexture;
    Box boundingBox;
    std::vector<PGSceneObject> sceneObjects;
};

