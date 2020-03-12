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

DX11BlendState::DX11BlendState(ID3D11Device* device, const HWBlendDesc& blendDesc, const char* debugName) {
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

    HRESULT result = device->CreateBlendState(&dxBlendDesc, &m_BlendState);
    PG_ASSERT(SUCCEEDED(result), "Error at creating blend state");

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_BlendState->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)debugNameLen, debugName);
    }
#endif
}

DX11BlendState::~DX11BlendState() {
    SAFE_RELEASE(m_BlendState);
}

DX11RasterizerState::DX11RasterizerState(ID3D11Device* device, const HWRasterizerDesc& blendDesc, const char* debugName) {
    D3D11_RASTERIZER_DESC dxRasterizerDesc = {};
    dxRasterizerDesc.FillMode = GetD3D11FillMode(blendDesc.fillMode);
    dxRasterizerDesc.CullMode = GetD3D11CullMode(blendDesc.cullMode);
    dxRasterizerDesc.FrontCounterClockwise = blendDesc.frontCounterClockwise;
    dxRasterizerDesc.DepthBias = blendDesc.depthBias;
    dxRasterizerDesc.DepthBiasClamp = blendDesc.depthBiasClamp;
    dxRasterizerDesc.SlopeScaledDepthBias = blendDesc.slopeScaledDepthBias;
    dxRasterizerDesc.DepthClipEnable = blendDesc.depthClipEnable;
    dxRasterizerDesc.ScissorEnable = blendDesc.scissorEnable;
    dxRasterizerDesc.MultisampleEnable = blendDesc.multisampleEnable;
    dxRasterizerDesc.AntialiasedLineEnable = blendDesc.antialiasedLineEnable;

    HRESULT result = device->CreateRasterizerState(&dxRasterizerDesc, &m_RasterizerState);
    PG_ASSERT(SUCCEEDED(result), "Error at creating rasterizer state");

#ifdef PG_DEBUG_GPU_DEVICE
    if (debugName) {
        size_t debugNameLen = strlen(debugName);
        m_RasterizerState->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)debugNameLen, debugName);
    }
#endif
}

DX11RasterizerState::~DX11RasterizerState() {
    SAFE_RELEASE(m_RasterizerState);
}

