#include "DX11PipelineStates.h"

static D3D11_BLEND GetD3D11Blend(HWBlend blend) {
    switch (blend) {
        case BLEND_ZERO:
            return D3D11_BLEND_ZERO;
        case BLEND_ONE:
            return D3D11_BLEND_ONE;
        case BLEND_SRC_COLOR:
            return D3D11_BLEND_SRC_COLOR;
        case BLEND_INV_SRC_COLOR:
            return D3D11_BLEND_INV_SRC_COLOR;
        case BLEND_SRC_ALPHA:
            return D3D11_BLEND_SRC_ALPHA;
        case BLEND_INV_SRC_ALPHA:
            return D3D11_BLEND_INV_SRC_ALPHA;
        case BLEND_DEST_ALPHA:
            return D3D11_BLEND_DEST_ALPHA;
        case BLEND_INV_DEST_ALPHA:
            return D3D11_BLEND_INV_DEST_ALPHA;
        case BLEND_DEST_COLOR:
            return D3D11_BLEND_DEST_COLOR;
        case BLEND_INV_DEST_COLOR:
            return D3D11_BLEND_DEST_COLOR;
        case BLEND_SRC_ALPHA_SAT:
            return D3D11_BLEND_SRC_ALPHA_SAT;
        case BLEND_BLEND_FACTOR:
            return D3D11_BLEND_BLEND_FACTOR;
        case BLEND_INV_BLEND_FACTOR:
            return D3D11_BLEND_INV_BLEND_FACTOR;
        default:
            return D3D11_BLEND();
    }
}

static D3D11_BLEND_OP GetD3D11BlendOp(HWBlendOP blendOP) {
    switch (blendOP) {
        case BLEND_OP_ADD:
            return D3D11_BLEND_OP_ADD;
        case BLEND_OP_SUBTRACT:
            return D3D11_BLEND_OP_SUBTRACT;
        case BLEND_OP_REV_SUBTRACT:
            return D3D11_BLEND_OP_REV_SUBTRACT;
        case BLEND_OP_MIN:
            return D3D11_BLEND_OP_MIN;
        case BLEND_OP_MAX:
            return D3D11_BLEND_OP_MAX;
        default:
            return D3D11_BLEND_OP();
    }
}

static D3D11_COLOR_WRITE_ENABLE GetD3D11ColorWriteEnable(HWColorWriteEnable colorWriteEnable) {
    switch (colorWriteEnable) {
        case COLOR_WRITE_ENABLE_RED:
            return D3D11_COLOR_WRITE_ENABLE_RED;
        case COLOR_WRITE_ENABLE_GREEN:
            return D3D11_COLOR_WRITE_ENABLE_GREEN;
        case COLOR_WRITE_ENABLE_BLUE:
            return D3D11_COLOR_WRITE_ENABLE_BLUE;
        case COLOR_WRITE_ENABLE_ALPHA:
            return D3D11_COLOR_WRITE_ENABLE_ALPHA;
        case COLOR_WRITE_ENABLE_ALL:
            return D3D11_COLOR_WRITE_ENABLE_ALL;
        default:
            return D3D11_COLOR_WRITE_ENABLE();
    }
}

static D3D11_FILL_MODE GetD3D11FillMode(HWFillMode fillMode) {
    switch (fillMode) {
        case FILL_SOLID:
            return D3D11_FILL_SOLID;
        case FILL_WIREFRAME:
            return D3D11_FILL_WIREFRAME;
        default:
            return D3D11_FILL_MODE();
    }
}

static D3D11_CULL_MODE GetD3D11CullMode(HWCullMode cullMode) {
    switch (cullMode) {
        case CULL_NONE:
            return D3D11_CULL_NONE;
        case CULL_FRONT:
            return D3D11_CULL_FRONT;
        case CULL_BACK:
            return D3D11_CULL_BACK;
        default:
            return D3D11_CULL_MODE();
    }
}

static D3D11_DEPTH_WRITE_MASK GetD3D11DepthWriteMask(HWDepthWriteMask depthWriteMask) {
    switch (depthWriteMask) {
        case DEPTH_WRITE_MASK_ZERO:
            return D3D11_DEPTH_WRITE_MASK_ZERO;
        case DEPTH_WRITE_MASK_ALL:
            return D3D11_DEPTH_WRITE_MASK_ALL;
        default:
            return D3D11_DEPTH_WRITE_MASK();
    }
}

static D3D11_COMPARISON_FUNC GetD3D11ComparisonFunc(HWComparionsFunc comparisonFunc) {
    switch (comparisonFunc) {
        case COMPARISON_NEVER:
            return D3D11_COMPARISON_NEVER;
        case COMPARISON_LESS:
            return D3D11_COMPARISON_LESS;
        case COMPARISON_EQUAL:
            return D3D11_COMPARISON_EQUAL;
        case COMPARISON_LESS_EQUAL:
            return D3D11_COMPARISON_LESS_EQUAL;
        case COMPARISON_GREATER:
            return D3D11_COMPARISON_GREATER;
        case COMPARISON_NOT_EQUAL:
            return D3D11_COMPARISON_NOT_EQUAL;
        case COMPARISON_GREATER_EQUAL:
            return D3D11_COMPARISON_GREATER_EQUAL;
        case COMPARISON_ALWAYS:
            return D3D11_COMPARISON_ALWAYS;
        default:
            return D3D11_COMPARISON_FUNC();
    }
}

static D3D11_STENCIL_OP GetD3D11StencilOP(HWStencilOP stencilOP) {
    switch (stencilOP) {
        case STENCIL_OP_KEEP:
            return D3D11_STENCIL_OP_KEEP;
        case STENCIL_OP_ZERO:
            return D3D11_STENCIL_OP_ZERO;
        case STENCIL_OP_REPLACE:
            return D3D11_STENCIL_OP_REPLACE;
        case STENCIL_OP_INCR_SAT:
            return D3D11_STENCIL_OP_INCR_SAT;
        case STENCIL_OP_DECR_SAT:
            return D3D11_STENCIL_OP_DECR_SAT;
        case STENCIL_OP_INVERT:
            return D3D11_STENCIL_OP_INVERT;
        case STENCIL_OP_INCR:
            return D3D11_STENCIL_OP_INCR;
        case STENCIL_OP_DECR:
            return D3D11_STENCIL_OP_DECR;
        default:
            return D3D11_STENCIL_OP();
    }
}

static DXGI_FORMAT ConvertInputFormatToDXGIFormat(HWVertexDataFormat format) {
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

static D3D11_INPUT_CLASSIFICATION InputClassificationToD3D11(HWInputClassification classification) {
    switch (classification) {
    case PER_VERTEX_DATA: return D3D11_INPUT_PER_VERTEX_DATA;
    case PER_INSTANCE_DATA: return D3D11_INPUT_PER_INSTANCE_DATA;
    }
}

static D3D11_PRIMITIVE_TOPOLOGY GetD3D11PrimitiveTopology(HWPrimitiveTopology primitiveTopology) {
    switch (primitiveTopology) {
        case PRIMITIVE_TOPOLOGY_UNDEFINED:
            return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
        case PRIMITIVE_TOPOLOGY_POINTLIST:
            return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        case PRIMITIVE_TOPOLOGY_LINELIST:
            return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        case PRIMITIVE_TOPOLOGY_LINESTRIP:
            return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case PRIMITIVE_TOPOLOGY_TRIANGLELIST:
            return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
            return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        default:
            PG_ASSERT(false, "Unsupported primitive topology");
            break;
    }
}

DX11PipelineState::DX11PipelineState(ID3D11Device* device, const HWPipelineStateDesc& pipelineDesc, const char* debugName) {
    // Shaders
    HWShaderBytecode vertexBytecode = pipelineDesc.vertexShader;
    HRESULT result = device->CreateVertexShader(vertexBytecode.shaderBytecode, vertexBytecode.bytecodeLength, 0, &m_VertexShader);
    PG_ASSERT(SUCCEEDED(result), "Error at creating vertex shader");


    HWShaderBytecode pixelBytecode = pipelineDesc.pixelShader;
    if (pixelBytecode.shaderBytecode) {
        result = device->CreatePixelShader(pixelBytecode.shaderBytecode, pixelBytecode.bytecodeLength, 0, &m_PixelShader);
        PG_ASSERT(SUCCEEDED(result), "Error at creating pixel shader");
    }

    // Blend state
    HWBlendDesc blendDesc = pipelineDesc.blendDesc;
    D3D11_BLEND_DESC dxBlendDesc = {};
    dxBlendDesc.AlphaToCoverageEnable = blendDesc.alphaToCoverageEnable;
    dxBlendDesc.IndependentBlendEnable = blendDesc.independentBlendEnable;
    for (size_t i = 0; i < ARRAYSIZE(dxBlendDesc.RenderTarget); ++i) {
        const HWRenderTargetBlendDesc& rtBlendDesc = blendDesc.renderTarget[i];
        D3D11_RENDER_TARGET_BLEND_DESC& dxRenderTargetBlendDesc = dxBlendDesc.RenderTarget[i];

        dxRenderTargetBlendDesc.BlendEnable = rtBlendDesc.blendEnable;
        dxRenderTargetBlendDesc.SrcBlend = GetD3D11Blend(rtBlendDesc.srcBlend);
        dxRenderTargetBlendDesc.DestBlend = GetD3D11Blend(rtBlendDesc.destBlend);
        dxRenderTargetBlendDesc.BlendOp = GetD3D11BlendOp(rtBlendDesc.blendOp);
        dxRenderTargetBlendDesc.SrcBlendAlpha = GetD3D11Blend(rtBlendDesc.srcBlendAlpha);
        dxRenderTargetBlendDesc.DestBlendAlpha = GetD3D11Blend(rtBlendDesc.destBlendAlpha);
        dxRenderTargetBlendDesc.BlendOpAlpha = GetD3D11BlendOp(rtBlendDesc.blendOpAlpha);
        dxRenderTargetBlendDesc.RenderTargetWriteMask = GetD3D11ColorWriteEnable(rtBlendDesc.renderTargetWriteMask);
    }

    result = device->CreateBlendState(&dxBlendDesc, &m_BlendState);
    PG_ASSERT(SUCCEEDED(result), "Error at creating blend state");

    // Rasterizer state
    HWRasterizerDesc rasterizerDesc = pipelineDesc.rasterizerDesc;
    D3D11_RASTERIZER_DESC dxRasterizerDesc = {};
    dxRasterizerDesc.FillMode = GetD3D11FillMode(rasterizerDesc.fillMode);
    dxRasterizerDesc.CullMode = GetD3D11CullMode(rasterizerDesc.cullMode);
    dxRasterizerDesc.FrontCounterClockwise = rasterizerDesc.frontCounterClockwise;
    dxRasterizerDesc.DepthBias = rasterizerDesc.depthBias;
    dxRasterizerDesc.DepthBiasClamp = rasterizerDesc.depthBiasClamp;
    dxRasterizerDesc.SlopeScaledDepthBias = rasterizerDesc.slopeScaledDepthBias;
    dxRasterizerDesc.DepthClipEnable = rasterizerDesc.depthClipEnable;
    dxRasterizerDesc.ScissorEnable = rasterizerDesc.scissorEnable;
    dxRasterizerDesc.MultisampleEnable = rasterizerDesc.multisampleEnable;
    dxRasterizerDesc.AntialiasedLineEnable = rasterizerDesc.antialiasedLineEnable;

    result = device->CreateRasterizerState(&dxRasterizerDesc, &m_RasterizerState);
    PG_ASSERT(SUCCEEDED(result), "Error at creating rasterizer state");

    // Input layout
    HWInputLayoutDesc inputLayoutDesc = pipelineDesc.inputLayoutDesc;
    if (inputLayoutDesc.elements && inputLayoutDesc.elementCount > 0) {
        D3D11_INPUT_ELEMENT_DESC* dxInputElements = (D3D11_INPUT_ELEMENT_DESC*)alloca(sizeof(D3D11_INPUT_ELEMENT_DESC) * inputLayoutDesc.elementCount);
        for (size_t i = 0; i < inputLayoutDesc.elementCount; ++i) {
            const HWVertexInputElement* element = inputLayoutDesc.elements + i;
            D3D11_INPUT_ELEMENT_DESC dx11Element = {};
            dx11Element.SemanticName = element->semanticName;
            dx11Element.SemanticIndex = element->semanticIndex;
            dx11Element.Format = ConvertInputFormatToDXGIFormat(element->format);
            dx11Element.InputSlot = element->inputSlot;
            dx11Element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            dx11Element.InputSlotClass = InputClassificationToD3D11(element->classification);
            dx11Element.InstanceDataStepRate = element->instanceStepRate;

            memcpy(dxInputElements + i, &dx11Element, sizeof(D3D11_INPUT_ELEMENT_DESC));
        }

        result = device->CreateInputLayout(dxInputElements, (UINT)inputLayoutDesc.elementCount,
                                           vertexBytecode.shaderBytecode, vertexBytecode.bytecodeLength,
                                           &m_InputLayout);
        PG_ASSERT(SUCCEEDED(result), "Error at creating input layout");
    }

    // DepthStencil state
    HWDepthStencilDesc depthStencilDesc = pipelineDesc.depthStencilDesc;

    D3D11_DEPTH_STENCIL_DESC dxDepthStencilDesc = {};
    dxDepthStencilDesc.DepthEnable = depthStencilDesc.depthEnable;
    dxDepthStencilDesc.DepthWriteMask = GetD3D11DepthWriteMask(depthStencilDesc.depthWriteMask);
    dxDepthStencilDesc.DepthFunc = GetD3D11ComparisonFunc(depthStencilDesc.depthFunc);
    dxDepthStencilDesc.StencilEnable = depthStencilDesc.stencilEnable;
    dxDepthStencilDesc.StencilReadMask = depthStencilDesc.stencilReadMask;
    dxDepthStencilDesc.StencilWriteMask = depthStencilDesc.stencilWriteMask;
    dxDepthStencilDesc.FrontFace.StencilFunc = GetD3D11ComparisonFunc(depthStencilDesc.frontFace.stencilFunc);
    dxDepthStencilDesc.BackFace.StencilFunc = GetD3D11ComparisonFunc(depthStencilDesc.backFace.stencilFunc);;
    dxDepthStencilDesc.FrontFace.StencilFailOp = GetD3D11StencilOP(depthStencilDesc.frontFace.stencilFailOp);
    dxDepthStencilDesc.BackFace.StencilFailOp = GetD3D11StencilOP(depthStencilDesc.backFace.stencilFailOp);
    dxDepthStencilDesc.FrontFace.StencilPassOp = GetD3D11StencilOP(depthStencilDesc.frontFace.stencilPassOp);
    dxDepthStencilDesc.BackFace.StencilPassOp = GetD3D11StencilOP(depthStencilDesc.backFace.stencilPassOp);
    dxDepthStencilDesc.FrontFace.StencilDepthFailOp = GetD3D11StencilOP(depthStencilDesc.frontFace.stencilDepthFailOp);
    dxDepthStencilDesc.BackFace.StencilDepthFailOp = GetD3D11StencilOP(depthStencilDesc.backFace.stencilDepthFailOp);

    result = device->CreateDepthStencilState(&dxDepthStencilDesc, &m_DepthStencilState);
    PG_ASSERT(SUCCEEDED(result), "Error at creating depth-stencil state");

    // Variables
    m_PrimitiveTopology = GetD3D11PrimitiveTopology(pipelineDesc.primitiveTopology);
    m_SampleMask = pipelineDesc.sampleMask;

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_VertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)debugNameLen, debugName);
        m_PixelShader->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)debugNameLen, debugName);

        m_BlendState->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)debugNameLen, debugName);
        m_RasterizerState->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)debugNameLen, debugName);
        m_InputLayout->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)debugNameLen, debugName);
        m_DepthStencilState->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)debugNameLen, debugName);
    }
#endif
}

DX11PipelineState::~DX11PipelineState() {
    SAFE_RELEASE(m_VertexShader);
    SAFE_RELEASE(m_PixelShader);

    SAFE_RELEASE(m_BlendState);
    SAFE_RELEASE(m_RasterizerState);
    SAFE_RELEASE(m_InputLayout);
    SAFE_RELEASE(m_DepthStencilState);
}
