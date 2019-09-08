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

DX11VertexInputLayout::DX11VertexInputLayout(ID3D11Device* device, std::vector<VertexInputElement> inputElements, DX11ShaderProgram* shaderProgram) {
    // Input layout creation
    const int size = inputElements.size();
    std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDescription;
    layoutDescription.reserve(size);

    for (VertexInputElement& element : inputElements) {
        D3D11_INPUT_ELEMENT_DESC dx11Description = {};
        dx11Description.SemanticName = element.name.c_str();
        dx11Description.SemanticIndex = 0;
        dx11Description.Format = ConvertInputFormatToDXGIFormat(element.format);
        dx11Description.InputSlot = element.inputSlot;
        dx11Description.AlignedByteOffset = element.offset;
        dx11Description.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        dx11Description.InstanceDataStepRate = 0;

        layoutDescription.push_back(dx11Description);
    }

    ShaderFile vertexShaderFile = shaderProgram->GetVertexShaderFile();
    HRESULT result = device->CreateInputLayout(layoutDescription.data(), layoutDescription.size(), 
        vertexShaderFile.fileData, vertexShaderFile.fileSize, &m_InputLayout);
    PG_ASSERT(SUCCEEDED(result), "Error at creating input layout");
}

DX11VertexInputLayout::~DX11VertexInputLayout() {
    SAFE_RELEASE(m_InputLayout);
}

