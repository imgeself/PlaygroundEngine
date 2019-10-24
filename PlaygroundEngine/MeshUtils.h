#pragma once

#include "Mesh.h"

#include "Core.h"

#include <unordered_map>
#include <memory>

#include "Utility/tiny_obj_loader.h"

typedef std::shared_ptr<Mesh> MeshRef;

static MeshRef LoadMeshFromOBJFile(const std::string& filename, bool flipZ = true, bool flipWindingDirection = true) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warnings;
    std::string errors;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warnings, &errors, filename.c_str());
    if (!warnings.empty()) {
        printf("%s Obj file load warnings: %s", filename.c_str(), warnings.c_str());
    }
    if (!errors.empty()) {
        printf("%s Obj file load errors: %s", filename.c_str(), errors.c_str());
    }
    PG_ASSERT(success, "Assertion failed: Obj file load error!");

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (tinyobj::shape_t& shape : shapes) {
        std::unordered_map<size_t, uint32_t> uniqueVertices = {};

        for (size_t i = 0; i < shape.mesh.indices.size(); i += 3) {
            tinyobj::index_t reorderedIndices[] = {
                shape.mesh.indices[i],
                shape.mesh.indices[i + 1],
                shape.mesh.indices[i + 2]
            };

            if (flipWindingDirection) {
                std::swap(reorderedIndices[1], reorderedIndices[2]);
            }

            for (tinyobj::index_t& reorderedIndex : reorderedIndices) {
                Vertex vertex;
                vertex.position = {
                    attrib.vertices[reorderedIndex.vertex_index * 3],
                    attrib.vertices[reorderedIndex.vertex_index * 3 + 1],
                    attrib.vertices[reorderedIndex.vertex_index * 3 + 2],
                };

                vertex.normal = {
                    attrib.normals[reorderedIndex.normal_index * 3],
                    attrib.normals[reorderedIndex.normal_index * 3 + 1],
                    attrib.normals[reorderedIndex.normal_index * 3 + 2],
                };

                if (flipZ) {
                    vertex.position.z *= -1.0f;
                    vertex.normal.z *= -1.0f;
                }

                size_t combinedHash = 0;
                std::hash<int> hasher;
                combinedHash ^= hasher(reorderedIndex.vertex_index) + 2305990;
                combinedHash ^= hasher(reorderedIndex.normal_index) + 135253 + (combinedHash << 4);
                combinedHash ^= hasher(reorderedIndex.texcoord_index) + 135253 + (combinedHash << 6);

                if (uniqueVertices.count(combinedHash) == 0) {
                    uniqueVertices[combinedHash] = (uint32_t) vertices.size();
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[combinedHash]);
            }
        }
    }

    return std::make_shared<Mesh>(shapes[0].name, vertices, indices);
}

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

    return std::make_shared<Mesh>("Cube", vertices, indices);
}

static MeshRef CreatePlaneMesh() {
    const std::vector<Vertex> vertices = {
        { { -1, 0, -1 }, { 0, 1, 0 } },
        { { -1, 0, 1 }, { 0, 1, 0 } },
        { { 1, 0, -1 }, { 0, 1, 0 } },
        { { 1, 0, 1 }, { 0, 1, 0 } },
    };

    const std::vector<uint32_t> indices = {
        0, 1, 3,
        3, 2, 0,
    };

    return std::make_shared<Mesh>("Plane", vertices, indices);
}

static std::unordered_map<std::string, MeshRef> LoadDefaultMeshes() {
    std::unordered_map<std::string, MeshRef> defaultMeshMap;

    MeshRef cubeMesh = CreateCubeMesh();
    defaultMeshMap[cubeMesh->name] = cubeMesh;

    MeshRef planeMesh = CreatePlaneMesh();
    defaultMeshMap[planeMesh->name] = planeMesh;

    return defaultMeshMap;
}
