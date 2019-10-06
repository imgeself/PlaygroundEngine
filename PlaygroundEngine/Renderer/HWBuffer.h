#pragma once

#include "../Core.h"

//TODO: We implementing uniform shader data interface like DX11 constant buffer interface.
// But this will probably be changed in the future when we support more graphics APIs. 
class HWConstantBuffer {
public:
    virtual ~HWConstantBuffer() = default;
};

class HWVertexBuffer {
public:
    virtual ~HWVertexBuffer() = default;
};

class HWIndexBuffer {
public:
    virtual ~HWIndexBuffer() = default;
};

