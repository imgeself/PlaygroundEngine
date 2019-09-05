#pragma once

#include "../PGBuffer.h"

#include <d3d11.h>

class DX11VertexBuffer : public IVertexBuffer {
public:
    DX11VertexBuffer(ID3D11Device* device, void* data, size_t size);
    ~DX11VertexBuffer() override;

private:
    ID3D11Buffer* bufferHandle;
};

class DX11IndexBuffer : public IIndexBuffer {
public:
    DX11IndexBuffer(ID3D11Device* device, void* data, size_t size);
    ~DX11IndexBuffer() override;

private:
    ID3D11Buffer* bufferHandle;
};
