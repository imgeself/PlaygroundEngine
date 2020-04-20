#pragma once

#include "../HWQuery.h"

#include <d3d11.h>

class DX11Query : public HWQuery {
public:
    DX11Query(ID3D11Device* device, const HWQueryDesc& queryDesc, const char* debugName = 0);
    ~DX11Query() override;

    ID3D11Query* GetDXQuery() { return m_Query; }

private:
    ID3D11Query* m_Query;
};

