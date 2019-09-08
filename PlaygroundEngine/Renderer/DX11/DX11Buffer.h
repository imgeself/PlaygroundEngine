#pragma once

#include "../PGBuffer.h"

#include <d3d11.h>

class DX11ConstantBuffer : public IConstantBuffer {
public:
    DX11ConstantBuffer(ID3D11Device* device, void* data, size_t size);
    ~DX11ConstantBuffer() override;

    ID3D11Buffer* GetDXBuffer() { return m_Buffer; }

private:
    ID3D11Buffer* m_Buffer;
};

class DX11VertexBuffer : public IVertexBuffer {
public:
    DX11VertexBuffer(ID3D11Device* device, void* data, size_t size);
    ~DX11VertexBuffer() override;

    ID3D11Buffer* GetDXBuffer() { return m_Buffer; }

private:
    ID3D11Buffer* m_Buffer;
};

class DX11IndexBuffer : public IIndexBuffer {
public:
    DX11IndexBuffer(ID3D11Device* device, uint32_t* data, uint32_t count);
    ~DX11IndexBuffer() override;

    ID3D11Buffer* GetDXBuffer() { return m_Buffer; }
    uint32_t GetCount() { return m_Count; }

private:
    ID3D11Buffer* m_Buffer;
    uint32_t m_Count;
};
