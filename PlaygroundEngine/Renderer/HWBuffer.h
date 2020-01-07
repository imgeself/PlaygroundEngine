#pragma once

#include "HWResource.h"
#include "../Core.h"

class HWConstantBuffer : public HWResource {
public:
    virtual ~HWConstantBuffer() = default;
};

class HWVertexBuffer : public HWResource {
public:
    virtual ~HWVertexBuffer() = default;
};

class HWIndexBuffer : public HWResource {
public:
    virtual ~HWIndexBuffer() = default;
};

