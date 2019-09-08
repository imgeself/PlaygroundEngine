#include "DX11RendererAPI.h"
#include "../../Math/math_util.h"

#include <time.h>

DX11RendererAPI::DX11RendererAPI(PGWindow* window) {

    // NOTE: We don't specify refresh rate at the moment.
    // Both numerator and denominator are 0;
    DXGI_RATIONAL refreshRate = {};

    RECT rect;
    GetClientRect(window->GetWindowHandle(), &rect);
    const UINT width = rect.right - rect.left;
    const UINT height = rect.bottom - rect.top;

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

    D3D_FEATURE_LEVEL deviceFeatureLevel;
    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_DEBUG,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &swapChainDescriptor,
        &m_SwapChain,
        &m_Device,
        &deviceFeatureLevel,
        &m_DeviceContext
    );

    PG_ASSERT(SUCCEEDED(result), "Device context creation failed");
    PG_ASSERT(featureLevels[0] == deviceFeatureLevel, "Hardware doesn't support DX11");

    ID3D11Resource* backbuffer = nullptr;
    result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), (void**) &backbuffer);
    PG_ASSERT(SUCCEEDED(result), "Couldn't get the backbuffer");

    result = m_Device->CreateRenderTargetView(backbuffer, nullptr, &m_BackbufferRenderTargetView);
    PG_ASSERT(SUCCEEDED(result), "Couldn't get the render target view");
    backbuffer->Release();

    D3D11_TEXTURE2D_DESC depthStencilTextureDesc = {};
    depthStencilTextureDesc.Width = width;
    depthStencilTextureDesc.Height = height;
    depthStencilTextureDesc.MipLevels = 1;
    depthStencilTextureDesc.ArraySize = 1;
    depthStencilTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilTextureDesc.SampleDesc.Count = 1;
    depthStencilTextureDesc.SampleDesc.Quality = 0;
    depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilTextureDesc.CPUAccessFlags = 0;
    depthStencilTextureDesc.MiscFlags = 0;

    ID3D11Texture2D* depthStencilTexture = nullptr;
    result = m_Device->CreateTexture2D(&depthStencilTextureDesc, nullptr, &depthStencilTexture);
    PG_ASSERT(SUCCEEDED(result), "Error at creating depth-stencil texture");

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Flags = 0;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    result = m_Device->CreateDepthStencilView(depthStencilTexture, &depthStencilViewDesc, &m_BackbufferDepthStencilView);
    PG_ASSERT(SUCCEEDED(result), "Error at creating depth-stencil view");

    /*
    D3D11_BUFFER_DESC constantBufferDescriptor = {};
    constantBufferDescriptor.ByteWidth = sizeof(colors);
    constantBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constantBufferDescriptor.MiscFlags = 0;
    constantBufferDescriptor.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA constantBufferSubresourceData = {};
    constantBufferSubresourceData.pSysMem = colors;

    ID3D11Buffer* constantBuffer = nullptr;
    result = m_Device->CreateBuffer(&constantBufferDescriptor, &constantBufferSubresourceData, &constantBuffer);
    PG_ASSERT(SUCCEEDED(result), "Error at creating constant buffer");
    */
    m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    
    m_DeviceContext->RSSetViewports(1, &viewport);

    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
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
    m_DeviceContext->OMSetRenderTargets(1, &m_BackbufferRenderTargetView, m_BackbufferDepthStencilView);
}

DX11RendererAPI::~DX11RendererAPI() {
    SAFE_RELEASE(m_BackbufferRenderTargetView);
    SAFE_RELEASE(m_SwapChain);
    SAFE_RELEASE(m_Device);
    SAFE_RELEASE(m_DeviceContext);
}

void DX11RendererAPI::ClearScreen(const float* color) {
    m_DeviceContext->ClearRenderTargetView(m_BackbufferRenderTargetView, color);
    m_DeviceContext->ClearDepthStencilView(m_BackbufferDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DX11RendererAPI::DrawIndexed(IIndexBuffer* indexBuffer) {
    /*
    struct ConstantBuffer {
        Matrix4 transform;
    } cBuff;

    cBuff.transform = IdentityMatrix;

    Vector3 translate(0.0f, 0.0f, 6.0f);
    Matrix4 transMatrix = TranslateMatrix(translate);
    
    time_t time = clock();
    float seed = time % 125263 / 662.9f;
    Matrix4 xAxisRotate = RotateMatrixXAxis(seed);
    Matrix4 yAxisRotate = RotateMatrixYAxis(seed);
    Matrix4 rotateMatrix = yAxisRotate * xAxisRotate;

    Matrix4 projMatrix = PerspectiveMatrix(1280, 720, 0.01f, 100.f, PI / 4.0f);

    cBuff.transform = projMatrix * transMatrix * rotateMatrix * cBuff.transform;

    D3D11_BUFFER_DESC constantBufferDescriptor = {};
    constantBufferDescriptor.ByteWidth = sizeof(ConstantBuffer);
    constantBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constantBufferDescriptor.MiscFlags = 0;
    constantBufferDescriptor.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA constantBufferSubresourceData = {};
    constantBufferSubresourceData.pSysMem = &cBuff;

    ID3D11Buffer* constantBuffer = nullptr;
    HRESULT result = m_Device->CreateBuffer(&constantBufferDescriptor, &constantBufferSubresourceData, &constantBuffer);
    PG_ASSERT(SUCCEEDED(result), "Error at creating constant buffer");
    m_DeviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);*/

    DX11IndexBuffer* dx11IndexBuffer = (DX11IndexBuffer*)indexBuffer;
    m_DeviceContext->DrawIndexed(dx11IndexBuffer->GetCount(), 0, 0);
}

void DX11RendererAPI::Present() {
    HRESULT result = m_SwapChain->Present(1, 0);
    PG_ASSERT(SUCCEEDED(result), "Error at presenting");
}

IConstantBuffer* DX11RendererAPI::CreateConstantBuffer(void* bufferData, size_t size) {
    return new DX11ConstantBuffer(m_Device, bufferData, size);
}

IVertexBuffer* DX11RendererAPI::CreateVertexBuffer(void* bufferData, size_t size) {
    return new DX11VertexBuffer(m_Device, bufferData, size);
}

IIndexBuffer* DX11RendererAPI::CreateIndexBuffer(uint32_t* bufferData, uint32_t count) {
    return new DX11IndexBuffer(m_Device, bufferData, count);
}

IShaderProgram* DX11RendererAPI::CreateShaderProgram(const char* vertexShaderFileName, const char* pixelShaderFileName) {
    return new DX11ShaderProgram(m_Device, vertexShaderFileName, pixelShaderFileName);
}

IVertexInputLayout* DX11RendererAPI::CreateVertexInputLayout(std::vector<VertexInputElement> inputElements, IShaderProgram* shaderProgram) {
    return new DX11VertexInputLayout(m_Device, inputElements, (DX11ShaderProgram*) shaderProgram);
}


// Bindings
void DX11RendererAPI::SetVertexBuffer(IVertexBuffer* vertexBuffer, uint32_t stride) {
    DX11VertexBuffer* dx11VertexBuffer = (DX11VertexBuffer*) vertexBuffer;
    ID3D11Buffer* buffer = dx11VertexBuffer->GetDXBuffer();
    UINT strides = stride;
    UINT offsets = 0;
    m_DeviceContext->IASetVertexBuffers(0, 1, &buffer, &strides, &offsets);
}

void DX11RendererAPI::SetIndexBuffer(IIndexBuffer* indexBuffer) {
    DX11IndexBuffer* dx11IndexBuffer = (DX11IndexBuffer*) indexBuffer;
    ID3D11Buffer* buffer = dx11IndexBuffer->GetDXBuffer();
    m_DeviceContext->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
}

void DX11RendererAPI::SetInputLayout(IVertexInputLayout* vertexInputLayout) {
    DX11VertexInputLayout* dx11VertexInputLayout = (DX11VertexInputLayout*) vertexInputLayout;
    ID3D11InputLayout* inputLayout = dx11VertexInputLayout->GetDXInputLayout();
    m_DeviceContext->IASetInputLayout(inputLayout);
}

void DX11RendererAPI::SetShaderProgram(IShaderProgram* shaderProgram) {
    DX11ShaderProgram* dx11ShaderProgram = (DX11ShaderProgram*) shaderProgram;
    ID3D11VertexShader* vertexShader = dx11ShaderProgram->GetDXVertexShader();
    ID3D11PixelShader* pixelShader = dx11ShaderProgram->GetDXPixelShader();
    m_DeviceContext->VSSetShader(vertexShader, nullptr, 0);
    m_DeviceContext->PSSetShader(pixelShader, nullptr, 0);
}

void DX11RendererAPI::SetConstanBufferVS(IConstantBuffer* constantBuffer) {
    DX11ConstantBuffer* dx11ConstantBuffer = (DX11ConstantBuffer*) constantBuffer;
    ID3D11Buffer* buffer = dx11ConstantBuffer->GetDXBuffer();
    m_DeviceContext->VSSetConstantBuffers(0, 1, &buffer);
}

void DX11RendererAPI::SetConstanBufferPS(IConstantBuffer* constantBuffer) {
    DX11ConstantBuffer* dx11ConstantBuffer = (DX11ConstantBuffer*)constantBuffer;
    ID3D11Buffer* buffer = dx11ConstantBuffer->GetDXBuffer();
    m_DeviceContext->PSSetConstantBuffers(0, 1, &buffer);
}


