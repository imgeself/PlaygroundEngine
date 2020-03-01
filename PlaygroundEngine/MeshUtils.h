#pragma once

#include "Mesh.h"

#include "Core.h"
#include "PGLog.h"

#include <unordered_map>
#include <memory>

#include "Utility/tiny_obj_loader.h"

typedef Mesh* MeshRef;

static MeshRef LoadMeshFromOBJFile(HWRendererAPI* rendererAPI, const std::string& filename, bool flipZ = true, bool flipWindingDirection = true) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warnings;
    std::string errors;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warnings, &errors, filename.c_str());
    if (!warnings.empty()) {
        PG_LOG_WARNING("%s Obj file load warnings: %s", filename.c_str(), warnings.c_str());
    }
    if (!errors.empty()) {
        PG_LOG_ERROR("%s Obj file load errors: %s", filename.c_str(), errors.c_str());
        return nullptr;
    }


    std::vector<Vertex_POS> positions;
    std::vector<Vertex_NOR_TEXCOORD> normalsTexCoords;
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
                Vertex_POS vertexPOS;
                vertexPOS.position = {
                    attrib.vertices[reorderedIndex.vertex_index * 3],
                    attrib.vertices[reorderedIndex.vertex_index * 3 + 1],
                    attrib.vertices[reorderedIndex.vertex_index * 3 + 2],
                };

                Vertex_NOR_TEXCOORD vertexNormalTexCoord;
                vertexNormalTexCoord.normal = {
                    attrib.normals[reorderedIndex.normal_index * 3],
                    attrib.normals[reorderedIndex.normal_index * 3 + 1],
                    attrib.normals[reorderedIndex.normal_index * 3 + 2],
                };

                vertexNormalTexCoord.texCoord = {
                    attrib.texcoords[reorderedIndex.texcoord_index * 2],
                    attrib.texcoords[reorderedIndex.texcoord_index * 2 + 1]
                };

                if (flipZ) {
                    vertexPOS.position.z *= -1.0f;
                    vertexNormalTexCoord.normal.z *= -1.0f;
                }

                size_t combinedHash = 0;
                std::hash<int> hasher;
                combinedHash ^= hasher(reorderedIndex.vertex_index) + 2305990;
                combinedHash ^= hasher(reorderedIndex.normal_index) + 135253 + (combinedHash << 4);
                combinedHash ^= hasher(reorderedIndex.texcoord_index) + 135253 + (combinedHash << 6);

                if (uniqueVertices.count(combinedHash) == 0) {
                    uniqueVertices[combinedHash] = (uint32_t) positions.size();
                    positions.push_back(vertexPOS);
                    normalsTexCoords.push_back(vertexNormalTexCoord);
                }
                indices.push_back(uniqueVertices[combinedHash]);
            }
        }
    }

    MeshRef mesh = new Mesh;
    mesh->name = shapes[0].name;
    mesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_POSITIONS] = rendererAPI->CreateVertexBuffer(positions.data(), positions.size() * sizeof(Vertex_POS), HWResourceFlags::USAGE_IMMUTABLE);
    mesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_NOR_TEXCOORDS] = rendererAPI->CreateVertexBuffer(normalsTexCoords.data(), normalsTexCoords.size() * sizeof(Vertex_NOR_TEXCOORD), HWResourceFlags::USAGE_IMMUTABLE);
    mesh->indexBuffer = rendererAPI->CreateIndexBuffer(indices.data(), indices.size(), HWResourceFlags::USAGE_IMMUTABLE);

    return mesh;
}

static MeshRef CreatePlaneMesh(HWRendererAPI* rendererAPI) {
    std::vector<Vertex_POS> positions = {
        { { -1, 0, -1 } },
        { { -1, 0, 1 } },
        { { 1, 0, -1 } },
        { { 1, 0, 1 } }
    };

    std::vector<Vertex_NOR_TEXCOORD> normalsTexCoords = {
        { { 0, 1, 0 }, { 0, 1 } },
        { { 0, 1, 0 }, { 0, 0 } },
        { { 0, 1, 0 }, { 1, 1 } },
        { { 0, 1, 0 }, { 1, 0 } },
    };

    std::vector<uint32_t> indices = {
        0, 1, 3,
        3, 2, 0,
    };

    MeshRef mesh = new Mesh;
    mesh->name = "Plane";
    mesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_POSITIONS] = rendererAPI->CreateVertexBuffer(positions.data(), positions.size() * sizeof(Vertex_POS), HWResourceFlags::USAGE_IMMUTABLE);
    mesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_NOR_TEXCOORDS] = rendererAPI->CreateVertexBuffer(normalsTexCoords.data(), normalsTexCoords.size() * sizeof(Vertex_NOR_TEXCOORD), HWResourceFlags::USAGE_IMMUTABLE);
    mesh->indexBuffer = rendererAPI->CreateIndexBuffer(indices.data(), indices.size(), HWResourceFlags::USAGE_IMMUTABLE);

    return mesh;
}
