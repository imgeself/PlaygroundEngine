#pragma once

#include "../Core.h"

//TODO: We implementing uniform shader data interface like DX11 constant buffer interface.
// But this will probably be changed in the future when we support more graphics APIs. 
class IConstantBuffer {
public:
    virtual ~IConstantBuffer() = default;
};

class IVertexBuffer {
public:
    virtual ~IVertexBuffer() = default;
};

class IIndexBuffer {
public:
    virtual ~IIndexBuffer() = default;
};

