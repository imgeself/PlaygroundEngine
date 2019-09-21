#pragma once

#include "Mesh.h"

#include <unordered_map>
#include <memory>

typedef std::shared_ptr<Mesh> MeshRef;

//TODO: Obviously we will load meshes from the disk when we implement obj loader.
static MeshRef CreateCubeMesh() {
    const std::vector<Vertex> vertices = {
        { { -1, 1, 1 }, { -1, 0, 0 } },
        { { -1, 1, -1 }, { -1, 0, 0 } },
        { { -1, -1, -1 }, { -1, 0, 0 } },
        { { -1, -1, 1 }, { -1, 0, 0 } },
        { { -1, 1, -1 }, { 0, 0, -1 } },
        { { 1, 1, -1 }, { 0, 0, -1 } },
        { { 1, -1, -1 }, { 0, 0, -1 } },
        { { -1, -1, -1}, { 0, 0, -1 } },
        { { 1, 1, -1 }, { 1, 0, 0 } },
        { { 1, 1, 1 }, { 1, 0, 0 } },
        { { 1, -1, 1 }, { 1, 0, 0 } },
        { { 1, -1, -1 }, { 1, 0, 0 } },
        { { 1, 1, 1 }, { 0, 0, 1 } },
        { { -1, 1, 1 }, { 0, 0, 1 } },
        { { -1, -1, 1 }, { 0, 0, 1 } },
        { { 1, -1, 1 }, { 0, 0, 1 } },
        { { -1, -1, 1 }, { 0, -1, 0 } },
        { { -1, -1, -1 }, { 0, -1, 0 } },
        { { 1, -1, -1 }, { 0, -1, 0 } },
        { { 1, -1, 1 }, { 0, -1, 0 } },
        { { 1, 1, 1 }, { 0, 1, 0 } },
        { { 1, 1, -1 }, { 0, 1, 0 } },
        { { -1, 1, -1 }, { 0, 1, 0 } },
        { { -1, 1, 1 }, { 0, 1, 0 } }
    };

    const std::vector<uint32_t> indices = {
        0, 1, 3,
        1, 2, 3,
        4, 5, 7,
        5, 6, 7,
        8, 9, 11,
        9, 10, 11,
        12, 13, 15,
        13, 14, 15,
        16, 17, 19,
        17, 18, 19,
        20, 21, 23,
        21, 22, 23
    };

    Material defaultMaterial;
    Transform defaultTransform;
    return std::make_shared<Mesh>("Cube", vertices, indices, defaultMaterial, defaultTransform);
}

static std::unordered_map<std::string, MeshRef> LoadDefaultMeshes() {
    std::unordered_map<std::string, MeshRef> defaultMeshMap;

    MeshRef cubeMesh = CreateCubeMesh();
    defaultMeshMap[cubeMesh->name] = cubeMesh;

    return defaultMeshMap;
}
