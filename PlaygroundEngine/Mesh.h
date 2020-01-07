#pragma once

#include "Math/math_util.h"
#include "Renderer/Material.h"
#include "Transform.h"

#include <vector>
#include <string>

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 texCoord;
};


struct Mesh {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    Material* material;
    Transform transform;

    Mesh() {};
    Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
        : name(name), vertices(vertices), indices(indices) {}
    Mesh(const Mesh& copy)
        : name(copy.name), vertices(copy.vertices), indices(copy.indices), material(copy.material), transform(copy.transform) {}
};
