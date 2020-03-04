#pragma once

#include "../HWBuffer.h"

#include <d3d11.h>

class DX11Buffer : public HWBuffer {
public:
    DX11Buffer(ID3D11Device* device, SubresourceData* subresource, size_t size, uint32_t flags, const char* debugName = 0);
    ~DX11Buffer() override;

    ID3D11Buffer* GetDXBuffer() { return m_Buffer; }
    void* GetResourceHandle() override { return (void*) m_Buffer; }

private:
    ID3D11Buffer* m_Buffer;
};
