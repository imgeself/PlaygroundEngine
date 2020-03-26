#pragma once

#include "../Core.h"
#include "HWResource.h"

class HWShaderResourceView {
public:
    virtual ~HWShaderResourceView() = default;
};

class HWUnorderedAccessView {
public:
    virtual ~HWUnorderedAccessView() = default;
};

