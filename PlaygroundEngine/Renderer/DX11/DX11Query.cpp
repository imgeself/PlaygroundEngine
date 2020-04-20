#include "DX11Query.h"

static D3D11_QUERY GetD3D11Query(HWQueryType queryType) {
    switch (queryType) {
        case QUERY_EVENT:
            return D3D11_QUERY_EVENT;
        case QUERY_OCCLUSION:
            return D3D11_QUERY_OCCLUSION;
        case QUERY_TIMESTAMP:
            return D3D11_QUERY_TIMESTAMP;
        case QUERY_TIMESTAMP_DISJOINT:
            return D3D11_QUERY_TIMESTAMP_DISJOINT;
        case QUERY_PIPELINE_STATISTICS:
            return D3D11_QUERY_PIPELINE_STATISTICS;
        case QUERY_OCCLUSION_PREDICATE:
            return D3D11_QUERY_OCCLUSION_PREDICATE;
        default:
            return D3D11_QUERY();
    }
}

static D3D11_QUERY_MISC_FLAG GetD3D11QueryMiscFlags(uint8_t flags) {
    return D3D11_QUERY_MISC_FLAG(
        ((flags & HWQueryFlags::QUERY_MISC_PREDICATEHINT) ? D3D11_QUERY_MISC_PREDICATEHINT : 0)
    );
}

DX11Query::DX11Query(ID3D11Device* device, const HWQueryDesc& queryDesc, const char* debugName) {
    D3D11_QUERY_DESC dxQueryDesc;
    dxQueryDesc.Query = GetD3D11Query(queryDesc.queryType);
    dxQueryDesc.MiscFlags = GetD3D11QueryMiscFlags(queryDesc.flags);

    HRESULT result = device->CreateQuery(&dxQueryDesc, &m_Query);
    PG_ASSERT(SUCCEEDED(result), "Error at creating query");

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_Query->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)debugNameLen, debugName);
    }
#endif
}

DX11Query::~DX11Query() {
    SAFE_RELEASE(m_Query);
}

