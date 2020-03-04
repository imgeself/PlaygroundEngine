#include "DX11Buffer.h"

static inline D3D11_BIND_FLAG GetBindFlagsFromResourceFlags(uint32_t resourceFlags) {
    return D3D11_BIND_FLAG(
        ((resourceFlags & HWResourceFlags::BIND_DEPTH_STENCIL) ? D3D11_BIND_DEPTH_STENCIL : 0) |
        ((resourceFlags & HWResourceFlags::BIND_RENDER_TARGET) ? D3D11_BIND_RENDER_TARGET : 0) |
        ((resourceFlags & HWResourceFlags::BIND_VERTEX_BUFFER) ? D3D11_BIND_VERTEX_BUFFER : 0) |
        ((resourceFlags & HWResourceFlags::BIND_INDEX_BUFFER) ? D3D11_BIND_INDEX_BUFFER : 0) |
        ((resourceFlags & HWResourceFlags::BIND_CONSTANT_BUFFER) ? D3D11_BIND_CONSTANT_BUFFER : 0) |
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

DX11Buffer::DX11Buffer(ID3D11Device* device, SubresourceData* subresource, size_t size, uint32_t flags, const char* debugName) {
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = (UINT) size;
    bufferDesc.Usage = GetUsageFromResourceFlags(flags);
    bufferDesc.BindFlags = GetBindFlagsFromResourceFlags(flags);
    bufferDesc.CPUAccessFlags = GetCPUAccessFlagFromResourceFlags(flags);
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    if (subresource) {
        D3D11_SUBRESOURCE_DATA bufferSubresourceData = {};
        bufferSubresourceData.pSysMem = subresource->data;
        bufferSubresourceData.SysMemPitch = subresource->memPitch;
        bufferSubresourceData.SysMemSlicePitch = subresource->memSlicePitch;

        HRESULT result = device->CreateBuffer(&bufferDesc, &bufferSubresourceData, &m_Buffer);
        PG_ASSERT(SUCCEEDED(result), "Error at creating buffer");
    } else {
        HRESULT result = device->CreateBuffer(&bufferDesc, nullptr, &m_Buffer);
        PG_ASSERT(SUCCEEDED(result), "Error at creating buffer");
    }

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) debugNameLen, debugName);
    }
#endif
}

DX11Buffer::~DX11Buffer() {
    SAFE_RELEASE(m_Buffer);
}
