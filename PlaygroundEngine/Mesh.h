#pragma once

#include "Math/math_util.h"
#include "Renderer/Material.h"
#include "Transform.h"

#include "Renderer/HWRendererAPI.h"

#include <vector>
#include <string>

struct Vertex_POS {
    Vector3 position;
};

struct Vertex_NOR {
    Vector3 normal;
};

struct Vertex_TEXCOORD {
    Vector2 texCoord;
};

enum VertexBuffers : uint8_t {
    VERTEX_BUFFER_POSITIONS,
    VERTEX_BUFFER_NORMAL,
    VERTEX_BUFFER_TEXCOORD,
    VERTEX_BUFFER_TANGENT,

    VERTEX_BUFFER_COUNT
};

struct Box {
    Vector3 min;
    Vector3 max;
};

struct SubMesh {
    // TODO: Instead of storing raw pointers we should have handles for buffers. Because this is not a safe struct to copy.
    // If one of the copies get deleted, other's buffers will be invalidated.
    HWBuffer* vertexBuffers[VertexBuffers::VERTEX_BUFFER_COUNT] = {0};
    uint32_t vertexStrides[VertexBuffers::VERTEX_BUFFER_COUNT] = {0};
    uint32_t vertexOffsets[VertexBuffers::VERTEX_BUFFER_COUNT] = {0};


    HWBuffer* indexBuffer = nullptr;
    uint32_t indexBufferStride = 0;
    uint32_t indexBufferOffset = 0;

    uint32_t indexStart = 0;
    uint32_t indexCount = 0;

    // Not included in hash
    Material* material = nullptr;
    Box boundingBox;

    size_t GetGeometryHash() {
        size_t materialOffset = offsetof(SubMesh, material);
        size_t hash = Hash(((const uint8_t*) this), materialOffset);

        return hash;
    }

    ~SubMesh() {
        for (size_t i = 0; i < VertexBuffers::VERTEX_BUFFER_COUNT; ++i) {
            SAFE_DELETE(vertexBuffers[i]);
        }
        SAFE_DELETE(indexBuffer);
    }
};

struct Mesh {
    std::string name;
    std::vector<SubMesh*> submeshes;
};

typedef Mesh* MeshRef;

