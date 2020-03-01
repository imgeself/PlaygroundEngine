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

struct Vertex_NOR_TEXCOORD {
    Vector3 normal;
    Vector2 texCoord;
};

enum VertexBuffers : uint8_t {
    VERTEX_BUFFER_POSITIONS,
    VERTEX_BUFFER_NOR_TEXCOORDS,

    VERTEX_BUFFER_COUNT,
};

struct Mesh {
    std::string name;
    Material* material = nullptr;

    // TODO: Instead of storing raw pointers we should have handles for buffers. Because this is not a safe struct to copy.
    // If one of the copies get deleted, other's buffers will be invalidated.
    HWVertexBuffer* vertexBuffers[VertexBuffers::VERTEX_BUFFER_COUNT] = {0};
    HWIndexBuffer* indexBuffer = nullptr;

    ~Mesh() {
        for (size_t i = 0; i < VertexBuffers::VERTEX_BUFFER_COUNT; ++i) {
            SAFE_DELETE(vertexBuffers[i]);
        }
        SAFE_DELETE(indexBuffer);
    }
};
