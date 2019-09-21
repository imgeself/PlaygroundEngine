#pragma once

#include "Math/math_util.h"
#include "Core.h"
#include "Renderer/Material.h"
#include "Transform.h"

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
    Material material;
    Transform transform;

    Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, 
         const Material& material, const Transform& transform)
        : name(name), vertices(vertices), indices(indices), material(material), transform(transform) {}
};
