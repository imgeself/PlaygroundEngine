#include "DX11Buffer.h"

DX11VertexBuffer::DX11VertexBuffer(ID3D11Device* device, void* data, size_t size) {
    D3D11_BUFFER_DESC vertexBufferDescriptor = {};
    vertexBufferDescriptor.ByteWidth = size;
    vertexBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDescriptor.CPUAccessFlags = 0;
    vertexBufferDescriptor.MiscFlags = 0;
    vertexBufferDescriptor.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferSubresourceData = {};
    vertexBufferSubresourceData.pSysMem = data;

    HRESULT result = device->CreateBuffer(&vertexBufferDescriptor, &vertexBufferSubresourceData, &m_Buffer);
    PG_ASSERT(SUCCEEDED(result), "Error at creating vertex buffer");
}

DX11VertexBuffer::~DX11VertexBuffer() {
    SAFE_RELEASE(m_Buffer);
}

DX11IndexBuffer::DX11IndexBuffer(ID3D11Device* device, uint32_t* data, uint32_t count) 
    : m_Count(count) {
    D3D11_BUFFER_DESC vertexBufferDescriptor = {};
    vertexBufferDescriptor.ByteWidth = count * sizeof(uint32_t);
    vertexBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
    vertexBufferDescriptor.CPUAccessFlags = 0;
    vertexBufferDescriptor.MiscFlags = 0;
    vertexBufferDescriptor.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferSubresourceData = {};
    vertexBufferSubresourceData.pSysMem = data;

    HRESULT result = device->CreateBuffer(&vertexBufferDescriptor, &vertexBufferSubresourceData, &m_Buffer);
    PG_ASSERT(SUCCEEDED(result), "Error at creating vertex buffer");
}

DX11IndexBuffer::~DX11IndexBuffer() {
    SAFE_RELEASE(m_Buffer);
}

