#include "DX11RendererAPI.h"

DX11RendererAPI::DX11RendererAPI(PGWindow* window) {

    // NOTE: We don't specify refresh rate at the moment.
    // Both numerator and denominator are 0;
    DXGI_RATIONAL refreshRate = {};

    Vector2 clientSize = window->GetClientSize();
    const UINT width = (UINT) clientSize.x;
    const UINT height = (UINT) clientSize.y;

    m_ClientWidth = width;
    m_ClientHeight = height;

    DXGI_MODE_DESC modeDescriptor = {};
    modeDescriptor.Width = width;
    modeDescriptor.Height = height;
    modeDescriptor.RefreshRate = refreshRate;
    modeDescriptor.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    modeDescriptor.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    modeDescriptor.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Antialiasing descriptor
    DXGI_SAMPLE_DESC sampleDescriptor = {};
    sampleDescriptor.Count = 1;
    sampleDescriptor.Quality = 0;

    DXGI_SWAP_CHAIN_DESC swapChainDescriptor = {};
    swapChainDescriptor.BufferDesc = modeDescriptor;
    swapChainDescriptor.SampleDesc = sampleDescriptor;
    swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescriptor.BufferCount = 2;
    swapChainDescriptor.OutputWindow = window->GetWindowHandle();
    swapChainDescriptor.Windowed = true;
    swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDescriptor.Flags = 0;

    const D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0
    };

    UINT createDeviceFlags = 0;
#ifdef PG_DEBUG_GPU_DEVICE
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL deviceFeatureLevel;
    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        (D3D11_CREATE_DEVICE_FLAG) createDeviceFlags,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &swapChainDescriptor,
        &m_SwapChain,
        &m_Device,
        &deviceFeatureLevel,
        &m_DeviceContext
    );

#ifdef PG_DEBUG_GPU_DEVICE
    m_Device->QueryInterface(__uuidof(ID3D11Debug), (void**) (&m_Debug));
#endif

    PG_ASSERT(SUCCEEDED(result), "Device context creation failed");
    PG_ASSERT(featureLevels[0] == deviceFeatureLevel, "Hardware doesn't support DX11");

    ID3D11Texture2D* backbuffer = nullptr;
    result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backbuffer);
    PG_ASSERT(SUCCEEDED(result), "Couldn't get the backbuffer");

    m_BackbufferRenderTargetView = new DX11RenderTargetView(m_Device, backbuffer, 0, 1, 0, swapChainDescriptor.SampleDesc.Count);
    backbuffer->Release();

    m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_DefaultViewport.TopLeftX = 0;
    m_DefaultViewport.TopLeftY = 0;
    m_DefaultViewport.Width = clientSize.x;
    m_DefaultViewport.Height = clientSize.y;
    m_DefaultViewport.MinDepth = 0.0f;
    m_DefaultViewport.MaxDepth = 1.0f;
    
    m_DeviceContext->RSSetViewports(1, &m_DefaultViewport);

    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.AntialiasedLineEnable = FALSE;

    ID3D11RasterizerState* rasterizerState = nullptr;
    result = m_Device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    PG_ASSERT(SUCCEEDED(result), "Error at creating rasterizer state");

    m_DeviceContext->RSSetState(rasterizerState);
    rasterizerState->Release();

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

    ID3D11DepthStencilState* depthStencilState = nullptr;
    result = m_Device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
    PG_ASSERT(SUCCEEDED(result), "Error at creating depth-stencil state");

    m_DeviceContext->OMSetDepthStencilState(depthStencilState, 0);
    depthStencilState->Release();
}

DX11RendererAPI::~DX11RendererAPI() {
    delete m_BackbufferRenderTargetView;

    SAFE_RELEASE(m_SwapChain);
    SAFE_RELEASE(m_DeviceContext);
    SAFE_RELEASE(m_Device);

#ifdef PG_DEBUG_GPU_DEVICE
    m_Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    m_Debug->Release();
#endif
}

void DX11RendererAPI::ResizeBackBuffer(size_t clientWidth, size_t clientHeight) {
    delete m_BackbufferRenderTargetView;

    m_ClientWidth = clientWidth;
    m_ClientHeight = clientHeight;

    m_SwapChain->ResizeBuffers(2, (UINT) clientWidth, (UINT) clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

    ID3D11Texture2D* backbuffer = nullptr;
    HRESULT result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer);
    PG_ASSERT(SUCCEEDED(result), "Couldn't get the backbuffer");

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    m_SwapChain->GetDesc(&swapChainDesc);

    m_BackbufferRenderTargetView = new DX11RenderTargetView(m_Device, backbuffer, 0, 1, 0, swapChainDesc.SampleDesc.Count);
    backbuffer->Release();

    m_DefaultViewport.Width = (FLOAT) clientWidth;
    m_DefaultViewport.Height = (FLOAT) clientHeight;
}


void DX11RendererAPI::ClearScreen(const float* color) {
    ClearRenderTarget(m_BackbufferRenderTargetView, (float*) color);
}

void DX11RendererAPI::Draw(HWVertexBuffer* vertexBuffer) {
    DX11VertexBuffer* dx11VertexBuffer = (DX11VertexBuffer*) vertexBuffer;
    m_DeviceContext->Draw(dx11VertexBuffer->GetCount(), 0);
}

void DX11RendererAPI::Draw(size_t vertexCount, size_t vertexBaseLocation) {
    m_DeviceContext->Draw((UINT) vertexCount, (UINT) vertexBaseLocation);
}

void DX11RendererAPI::DrawIndexed(HWIndexBuffer* indexBuffer) {
    DX11IndexBuffer* dx11IndexBuffer = (DX11IndexBuffer*)indexBuffer;
    m_DeviceContext->DrawIndexed((UINT) dx11IndexBuffer->GetCount(), 0, 0);
}

void DX11RendererAPI::Present() {
    HRESULT result = m_SwapChain->Present(1, 0);
    PG_ASSERT(SUCCEEDED(result), "Error at presenting");
}

HWRenderTargetView* DX11RendererAPI::GetBackbufferRenderTargetView() {
    return (HWRenderTargetView*) m_BackbufferRenderTargetView;
}

HWViewport DX11RendererAPI::GetDefaultViewport() {
    HWViewport result = {};
    result.topLeftX = (FLOAT) m_DefaultViewport.TopLeftX;
    result.topLeftY = (FLOAT) m_DefaultViewport.TopLeftY;
    result.width = (FLOAT) m_DefaultViewport.Width;
    result.height = (FLOAT) m_DefaultViewport.Height;

    return result;
}

HWConstantBuffer* DX11RendererAPI::CreateConstantBuffer(void* bufferData, size_t size, const char* debugName) {
    return new DX11ConstantBuffer(m_Device, bufferData, size, debugName);
}

HWVertexBuffer* DX11RendererAPI::CreateVertexBuffer(void* bufferData, size_t size, size_t strideSize, const char* debugName) {
    return new DX11VertexBuffer(m_Device, bufferData, size, strideSize, debugName);
}

HWIndexBuffer* DX11RendererAPI::CreateIndexBuffer(uint32_t* bufferData, size_t count, const char* debugName) {
    return new DX11IndexBuffer(m_Device, bufferData, count, debugName);
}

HWShaderProgram* DX11RendererAPI::CreateShaderProgramFromBinarySource(ShaderFileData* vertexShaderFileData, ShaderFileData* pixelShaderFileData, const char* debugName) {
    return new DX11ShaderProgram(m_Device, vertexShaderFileData, pixelShaderFileData);
}

HWVertexInputLayout* DX11RendererAPI::CreateVertexInputLayout(std::vector<VertexInputElement> inputElements, HWShaderProgram* shaderProgram, const char* debugName) {
    return new DX11VertexInputLayout(m_Device, inputElements, (DX11ShaderProgram*) shaderProgram, debugName);
}

HWTexture2D* DX11RendererAPI::CreateTexture2D(Texture2DDesc* initParams, TextureSubresourceData* subresources, const char* debugName) {
    return new DX11Texture2D(m_Device, initParams, subresources, debugName);
}

HWRenderTargetView* DX11RendererAPI::CreateRenderTargetView(HWTexture2D* texture, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount, const char* debugName) {
    DX11Texture2D* dxTexture2D = (DX11Texture2D*) texture;
    return new DX11RenderTargetView(m_Device, dxTexture2D->GetDXTexture2D(), firstSlice, sliceCount, firstMip, mipCount, debugName);
}

HWDepthStencilView* DX11RendererAPI::CreateDepthStencilView(HWTexture2D* texture, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount, const char* debugName) {
    DX11Texture2D* dxTexture2D = (DX11Texture2D*) texture;
    return new DX11DepthStencilView(m_Device, dxTexture2D->GetDXTexture2D(), firstSlice, sliceCount, firstMip, mipCount, debugName);
}

HWShaderResourceView* DX11RendererAPI::CreateShaderResourceView(HWTexture2D* texture, const char* debugName) {
    DX11Texture2D* dxTexture2D = (DX11Texture2D*) texture;
    return new DX11ShaderResourceView(m_Device, dxTexture2D->GetDXTexture2D(), debugName);
}

HWSamplerState* DX11RendererAPI::CreateSamplerState(SamplerStateInitParams* initParams, const char* debugName) {
    return new DX11SamplerState(m_Device, initParams, debugName);
}



// Bindings
void DX11RendererAPI::SetVertexBuffer(HWVertexBuffer* vertexBuffer, size_t stride) {
    DX11VertexBuffer* dx11VertexBuffer = (DX11VertexBuffer*) vertexBuffer;
    ID3D11Buffer* buffer = dx11VertexBuffer->GetDXBuffer();
    UINT strides = (UINT) stride;
    UINT offsets = 0;
    m_DeviceContext->IASetVertexBuffers(0, 1, &buffer, &strides, &offsets);
}

void DX11RendererAPI::SetIndexBuffer(HWIndexBuffer* indexBuffer) {
    DX11IndexBuffer* dx11IndexBuffer = (DX11IndexBuffer*) indexBuffer;
    ID3D11Buffer* buffer = dx11IndexBuffer->GetDXBuffer();
    m_DeviceContext->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
}

void DX11RendererAPI::SetInputLayout(HWVertexInputLayout* vertexInputLayout) {
    if (vertexInputLayout) {
        DX11VertexInputLayout* dx11VertexInputLayout = (DX11VertexInputLayout*)vertexInputLayout;
        ID3D11InputLayout* inputLayout = dx11VertexInputLayout->GetDXInputLayout();
        m_DeviceContext->IASetInputLayout(inputLayout);
    } else {
        m_DeviceContext->IASetInputLayout(nullptr);
    }
}

void DX11RendererAPI::SetShaderProgram(HWShaderProgram* shaderProgram) {
    DX11ShaderProgram* dx11ShaderProgram = (DX11ShaderProgram*) shaderProgram;
    ID3D11VertexShader* vertexShader = dx11ShaderProgram->GetDXVertexShader();
    ID3D11PixelShader* pixelShader = dx11ShaderProgram->GetDXPixelShader();
    m_DeviceContext->VSSetShader(vertexShader, nullptr, 0);
    m_DeviceContext->PSSetShader(pixelShader, nullptr, 0);
}

void DX11RendererAPI::SetRenderTargets(HWRenderTargetView** renderTargets, size_t renderTargetCount, HWDepthStencilView* depthStencilView) {
    DX11DepthStencilView* dxDepthStencilView = (DX11DepthStencilView*) depthStencilView;
    DX11RenderTargetView** dxRenderTargetViews = (DX11RenderTargetView**) renderTargets;
    ID3D11RenderTargetView** destRenderTargetViews = (ID3D11RenderTargetView**) alloca(sizeof(ID3D11RenderTargetView*) * renderTargetCount);
    for (size_t i = 0; i < renderTargetCount; ++i) {
        DX11RenderTargetView* dxRenderTargetView = *dxRenderTargetViews;
        *(destRenderTargetViews + i) = dxRenderTargetView ? dxRenderTargetView->GetDXRenderTargetView() : nullptr;
        dxRenderTargetViews++;
    }

    ID3D11DepthStencilView* destDepthStencilView = dxDepthStencilView ? dxDepthStencilView->GetDXDepthStencilView() : nullptr;
    m_DeviceContext->OMSetRenderTargets((UINT) renderTargetCount, destRenderTargetViews, destDepthStencilView);
}


void DX11RendererAPI::SetConstanBuffersVS(size_t startSlot, HWConstantBuffer** constantBuffers, size_t count) {
    DX11ConstantBuffer** dx11ConstantBuffers = (DX11ConstantBuffer**) constantBuffers;
    ID3D11Buffer** buffers = (ID3D11Buffer**) alloca(sizeof(ID3D11Buffer*) * count);
    for (size_t i = 0; i < count; ++i) {
        DX11ConstantBuffer* dxConstantBuffer = *dx11ConstantBuffers;
        *(buffers + i) = dxConstantBuffer ? dxConstantBuffer->GetDXBuffer() : nullptr;
        dx11ConstantBuffers++;
    }

    m_DeviceContext->VSSetConstantBuffers((UINT) startSlot, (UINT) count, buffers);
}

void DX11RendererAPI::SetConstanBuffersPS(size_t startSlot, HWConstantBuffer** constantBuffers, size_t count) {
    DX11ConstantBuffer** dx11ConstantBuffers = (DX11ConstantBuffer **) constantBuffers;
    ID3D11Buffer** buffers = (ID3D11Buffer **) alloca(sizeof(ID3D11Buffer*) * count);
    for (size_t i = 0; i < count; ++i) {
        DX11ConstantBuffer* dxConstantBuffer = *dx11ConstantBuffers;
        *(buffers + i) = dxConstantBuffer ? dxConstantBuffer->GetDXBuffer() : nullptr;
        dx11ConstantBuffers++;
    }

    m_DeviceContext->PSSetConstantBuffers((UINT) startSlot, (UINT) count, buffers);
}

void DX11RendererAPI::SetShaderResourcesVS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) {
    DX11ShaderResourceView** dxShaderResourceViews = (DX11ShaderResourceView**) shaderResources;
    ID3D11ShaderResourceView** destShaderResouces = (ID3D11ShaderResourceView**) alloca(sizeof(ID3D11ShaderResourceView*) * shaderResourceCount);
    for (size_t i = 0; i < shaderResourceCount; ++i) {
        DX11ShaderResourceView* dxShaderResouceView = *dxShaderResourceViews;
        *(destShaderResouces + i) = dxShaderResouceView ? dxShaderResouceView->GetDXShaderResouceView() : nullptr;
        dxShaderResourceViews++;
    }

    m_DeviceContext->VSSetShaderResources((UINT) startSlot, (UINT) shaderResourceCount, destShaderResouces);
}

void DX11RendererAPI::SetShaderResourcesPS(size_t startSlot, HWShaderResourceView** shaderResources, size_t shaderResourceCount) {
    DX11ShaderResourceView** dxShaderResourceViews = (DX11ShaderResourceView**) shaderResources;
    ID3D11ShaderResourceView** destShaderResouces = (ID3D11ShaderResourceView**) alloca(sizeof(ID3D11ShaderResourceView*) * shaderResourceCount);
    for (size_t i = 0; i < shaderResourceCount; ++i) {
        DX11ShaderResourceView* dxShaderResouceView = *dxShaderResourceViews;
        *(destShaderResouces + i) = dxShaderResouceView ? dxShaderResouceView->GetDXShaderResouceView() : nullptr;
        dxShaderResourceViews++;
    }

    m_DeviceContext->PSSetShaderResources((UINT) startSlot, (UINT) shaderResourceCount, destShaderResouces);
}

void DX11RendererAPI::SetSamplerStatesVS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) {
    DX11SamplerState** dxSamplerStates = (DX11SamplerState**) samplerStates;
    ID3D11SamplerState** destSamplerStates = (ID3D11SamplerState**) alloca(sizeof(ID3D11SamplerState*) * samplerStateCount);
    for (size_t i = 0; i < samplerStateCount; ++i) {
        DX11SamplerState* dxSamplerState = *dxSamplerStates;
        *(destSamplerStates + i) = dxSamplerState ? dxSamplerState->GetDXSamplerState() : nullptr;
        dxSamplerStates++;
    }

    m_DeviceContext->VSSetSamplers((UINT) startSlot, (UINT) samplerStateCount, destSamplerStates);
}

void DX11RendererAPI::SetSamplerStatesPS(size_t startSlot, HWSamplerState** samplerStates, size_t samplerStateCount) {
    DX11SamplerState** dxSamplerStates = (DX11SamplerState**) samplerStates;
    ID3D11SamplerState** destSamplerStates = (ID3D11SamplerState**) alloca(sizeof(ID3D11SamplerState*) * samplerStateCount);
    for (size_t i = 0; i < samplerStateCount; ++i) {
        DX11SamplerState* dxSamplerState = *dxSamplerStates;
        *(destSamplerStates + i) = dxSamplerState ? dxSamplerState->GetDXSamplerState() : nullptr;
        dxSamplerStates++;
    }

    m_DeviceContext->PSSetSamplers((UINT) startSlot, (UINT) samplerStateCount, destSamplerStates);
}

void DX11RendererAPI::SetViewport(HWViewport* viewport) {
    D3D11_VIEWPORT dxViewport = {};
    dxViewport.TopLeftX = (FLOAT) viewport->topLeftX;
    dxViewport.TopLeftY = (FLOAT) viewport->topLeftY;
    dxViewport.Width = (FLOAT) viewport->width;
    dxViewport.Height = (FLOAT) viewport->height;
    dxViewport.MinDepth = 0.0f;
    dxViewport.MaxDepth = 1.0f;

    m_DeviceContext->RSSetViewports(1, &dxViewport);
}


void DX11RendererAPI::ClearRenderTarget(HWRenderTargetView* renderTargetView, float color[4]) {
    DX11RenderTargetView* dxRenderTargetView = (DX11RenderTargetView*) renderTargetView;
    m_DeviceContext->ClearRenderTargetView(dxRenderTargetView->GetDXRenderTargetView(), color);
}

void DX11RendererAPI::ClearDepthStencilView(HWDepthStencilView* depthStencilView, bool clearStencil, float depthClearData, uint8_t stencilClearData) {
    DX11DepthStencilView* dxDepthStencilView = (DX11DepthStencilView*) depthStencilView;

    D3D11_CLEAR_FLAG clearFlag = D3D11_CLEAR_DEPTH;
    if (clearStencil) {
        clearFlag = D3D11_CLEAR_FLAG(clearFlag | D3D11_CLEAR_STENCIL);
    }
    m_DeviceContext->ClearDepthStencilView(dxDepthStencilView->GetDXDepthStencilView(), clearFlag, depthClearData, stencilClearData);
}


void* DX11RendererAPI::Map(HWConstantBuffer* resource) {
    DX11ConstantBuffer* dx11ConstantBuffer = (DX11ConstantBuffer*) resource;
    ID3D11Buffer* buffer = dx11ConstantBuffer->GetDXBuffer();

    D3D11_MAPPED_SUBRESOURCE mappedResource = {};
    m_DeviceContext->Map(buffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    return mappedResource.pData;
}

void DX11RendererAPI::Unmap(HWConstantBuffer* resource) {
    DX11ConstantBuffer* dx11ConstantBuffer = (DX11ConstantBuffer*)resource;
    ID3D11Buffer* buffer = dx11ConstantBuffer->GetDXBuffer();

    m_DeviceContext->Unmap(buffer, NULL);
}

void DX11RendererAPI::MSAAResolve(HWTexture2D* dest, HWTexture2D* source) {
    DX11Texture2D* dxDestTexture = (DX11Texture2D*) dest;
    DX11Texture2D* dxSourceTexture = (DX11Texture2D*) source;
    ID3D11Texture2D* dst = dxDestTexture->GetDXTexture2D();
    D3D11_TEXTURE2D_DESC destDesc = {};
    dst->GetDesc(&destDesc);
    m_DeviceContext->ResolveSubresource(dst, 0, dxSourceTexture->GetDXTexture2D(), 0, destDesc.Format);
}


