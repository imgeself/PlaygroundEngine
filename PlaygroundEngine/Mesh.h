#pragma once

#include "Math/math_util.h"
#include "Core.h"

#include <vector>
#include <string>

struct Vertex {
    Vector3 position;
    Vector3 normal;
};

struct Mesh {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
        : name(name), vertices(vertices), indices(indices) {}
};
