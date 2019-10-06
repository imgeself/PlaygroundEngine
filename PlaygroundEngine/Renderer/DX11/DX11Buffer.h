#pragma once

#include "../HWBuffer.h"

#include <d3d11.h>

class DX11ConstantBuffer : public HWConstantBuffer {
public:
    DX11ConstantBuffer(ID3D11Device* device, void* data, size_t size);
    ~DX11ConstantBuffer() override;

    ID3D11Buffer* GetDXBuffer() { return m_Buffer; }

private:
    ID3D11Buffer* m_Buffer;
};

class DX11VertexBuffer : public HWVertexBuffer {
public:
    DX11VertexBuffer(ID3D11Device* device, void* data, size_t size, size_t strideSize);
    ~DX11VertexBuffer() override;

    ID3D11Buffer* GetDXBuffer() { return m_Buffer; }
    uint32_t GetCount() { return m_Count; }

private:
    ID3D11Buffer* m_Buffer;
    uint32_t m_Count;
};

class DX11IndexBuffer : public HWIndexBuffer {
public:
    DX11IndexBuffer(ID3D11Device* device, uint32_t* data, size_t count);
    ~DX11IndexBuffer() override;

    ID3D11Buffer* GetDXBuffer() { return m_Buffer; }
    size_t GetCount() { return m_Count; }

private:
    ID3D11Buffer* m_Buffer;
    size_t m_Count;
};
