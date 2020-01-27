#include "DX11InputLayout.h"

#include <array>

static DXGI_FORMAT ConvertInputFormatToDXGIFormat(VertexDataFormat format) {
    switch (format) {
        case VertexDataFormat_FLOAT:    return DXGI_FORMAT_R32_FLOAT;
        case VertexDataFormat_FLOAT2:   return DXGI_FORMAT_R32G32_FLOAT;
        case VertexDataFormat_FLOAT3:   return DXGI_FORMAT_R32G32B32_FLOAT;
        case VertexDataFormat_FLOAT4:   return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case VertexDataFormat_INT:      return DXGI_FORMAT_R32_SINT;
        case VertexDataFormat_INT2:     return DXGI_FORMAT_R32G32_SINT;
        case VertexDataFormat_INT3:     return DXGI_FORMAT_R32G32B32_SINT;
        case VertexDataFormat_INT4:     return DXGI_FORMAT_R32G32B32A32_SINT;
        case VertexDataFormat_UINT:     return DXGI_FORMAT_R32_UINT;
        case VertexDataFormat_UINT2:    return DXGI_FORMAT_R32G32_UINT;
        case VertexDataFormat_UINT3:    return DXGI_FORMAT_R32G32B32_UINT;
        case VertexDataFormat_UINT4:    return DXGI_FORMAT_R32G32B32A32_UINT;
    }

    PG_ASSERT(false, "Unknown VertexDataFormat type");
    return DXGI_FORMAT_UNKNOWN;
}

static D3D11_INPUT_CLASSIFICATION InputClassificationToD3D11(InputClassification classification) {
    switch (classification) { 
        case PER_VERTEX_DATA: return D3D11_INPUT_PER_VERTEX_DATA;
        case PER_INSTANCE_DATA: return D3D11_INPUT_PER_INSTANCE_DATA;
    }
}

DX11VertexInputLayout::DX11VertexInputLayout(ID3D11Device* device, std::vector<VertexInputElement> inputElements, DX11VertexShader* vertexShader, const char* debugName) {
    // Input layout creation
    const size_t size = inputElements.size();
    std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDescription;
    layoutDescription.reserve(size);

    for (VertexInputElement& element : inputElements) {
        D3D11_INPUT_ELEMENT_DESC dx11Description = {};
        dx11Description.SemanticName = element.semanticName.c_str();
        dx11Description.SemanticIndex = element.semanticIndex;
        dx11Description.Format = ConvertInputFormatToDXGIFormat(element.format);
        dx11Description.InputSlot = element.inputSlot;
        dx11Description.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        dx11Description.InputSlotClass = InputClassificationToD3D11(element.classification);
        dx11Description.InstanceDataStepRate = element.instanceStepRate;

        layoutDescription.push_back(dx11Description);
    }

    ID3DBlob* vertexShaderBlob = vertexShader->GetVertexShaderBlob();
    HRESULT result = device->CreateInputLayout(layoutDescription.data(), (UINT) layoutDescription.size(), 
        vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &m_InputLayout);
    PG_ASSERT(SUCCEEDED(result), "Error at creating input layout");

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_InputLayout->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) debugNameLen, debugName);
    }
#endif
}

DX11VertexInputLayout::~DX11VertexInputLayout() {
    SAFE_RELEASE(m_InputLayout);
}

