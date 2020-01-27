#include "DX11Buffer.h"

static inline D3D11_BIND_FLAG GetBindFlagsFromResourceFlags(uint32_t resourceFlags) {
    return D3D11_BIND_FLAG(
        ((resourceFlags & HWResourceFlags::BIND_SHADER_RESOURCE) ? D3D11_BIND_SHADER_RESOURCE : 0)
    );
}

static inline D3D11_USAGE GetUsageFromResourceFlags(uint32_t resourceFlags) {
    return D3D11_USAGE(
        ((resourceFlags & HWResourceFlags::USAGE_STAGING)   ? D3D11_USAGE_STAGING :
        ((resourceFlags & HWResourceFlags::USAGE_DYNAMIC)   ? D3D11_USAGE_DYNAMIC :
        ((resourceFlags & HWResourceFlags::USAGE_IMMUTABLE) ? D3D11_USAGE_IMMUTABLE :
                                                                  D3D11_USAGE_DEFAULT)))
    );
}

static inline D3D11_CPU_ACCESS_FLAG GetCPUAccessFlagFromResourceFlags(uint32_t resourceFlags) {
    return D3D11_CPU_ACCESS_FLAG(
        ((resourceFlags & HWResourceFlags::CPU_ACCESS_READ)  ? D3D11_CPU_ACCESS_READ : 0) |
        ((resourceFlags & HWResourceFlags::CPU_ACCESS_WRITE) ? D3D11_CPU_ACCESS_WRITE : 0)
    );
}

DX11ConstantBuffer::DX11ConstantBuffer(ID3D11Device* device, void* data, size_t size, uint32_t flags, const char* debugName) {
    D3D11_BUFFER_DESC constantBufferDescriptor = {};
    constantBufferDescriptor.ByteWidth = (UINT) size;
    constantBufferDescriptor.Usage = GetUsageFromResourceFlags(flags);
    constantBufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER | GetBindFlagsFromResourceFlags(flags);
    constantBufferDescriptor.CPUAccessFlags = GetCPUAccessFlagFromResourceFlags(flags);
    constantBufferDescriptor.MiscFlags = 0;
    constantBufferDescriptor.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA constantBufferSubresourceData = {};
    constantBufferSubresourceData.pSysMem = data;

    HRESULT result = device->CreateBuffer(&constantBufferDescriptor, &constantBufferSubresourceData, &m_Buffer);
    PG_ASSERT(SUCCEEDED(result), "Error at creating constant buffer");

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) debugNameLen, debugName);
    }
#endif
}

DX11ConstantBuffer::~DX11ConstantBuffer() {
    SAFE_RELEASE(m_Buffer);
}

DX11VertexBuffer::DX11VertexBuffer(ID3D11Device* device, void* data, size_t size, uint32_t flags, const char* debugName) {
    D3D11_BUFFER_DESC vertexBufferDescriptor = {};
    vertexBufferDescriptor.ByteWidth = (UINT) size;
    vertexBufferDescriptor.Usage = GetUsageFromResourceFlags(flags);
    vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER | GetBindFlagsFromResourceFlags(flags);
    vertexBufferDescriptor.CPUAccessFlags = GetCPUAccessFlagFromResourceFlags(flags);
    vertexBufferDescriptor.MiscFlags = 0;
    vertexBufferDescriptor.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferSubresourceData = {};
    vertexBufferSubresourceData.pSysMem = data;

    HRESULT result = device->CreateBuffer(&vertexBufferDescriptor, &vertexBufferSubresourceData, &m_Buffer);
    PG_ASSERT(SUCCEEDED(result), "Error at creating vertex buffer");

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) debugNameLen, debugName);
    }
#endif
}

DX11VertexBuffer::~DX11VertexBuffer() {
    SAFE_RELEASE(m_Buffer);
}

DX11IndexBuffer::DX11IndexBuffer(ID3D11Device* device, uint32_t* data, size_t count, uint32_t flags, const char* debugName)
    : m_Count(count) {
    D3D11_BUFFER_DESC vertexBufferDescriptor = {};
    vertexBufferDescriptor.ByteWidth = (UINT) (count * sizeof(uint32_t));
    vertexBufferDescriptor.Usage = GetUsageFromResourceFlags(flags);
    vertexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER | GetBindFlagsFromResourceFlags(flags);
    vertexBufferDescriptor.CPUAccessFlags = GetCPUAccessFlagFromResourceFlags(flags);
    vertexBufferDescriptor.MiscFlags = 0;
    vertexBufferDescriptor.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferSubresourceData = {};
    vertexBufferSubresourceData.pSysMem = data;

    HRESULT result = device->CreateBuffer(&vertexBufferDescriptor, &vertexBufferSubresourceData, &m_Buffer);
    PG_ASSERT(SUCCEEDED(result), "Error at creating vertex buffer");

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) debugNameLen, debugName);
    }
#endif
}

DX11IndexBuffer::~DX11IndexBuffer() {
    SAFE_RELEASE(m_Buffer);
}

