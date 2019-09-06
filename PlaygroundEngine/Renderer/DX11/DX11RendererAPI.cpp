#include "DX11RendererAPI.h"
#include "../../Math/math_util.h"

#include <time.h>

#define SAFE_RELEASE(p) { if(p) { p->Release(); } }

static ShaderFile ReadBinaryFile(const char* filename) {
    HANDLE shaderFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    PG_ASSERT(shaderFile != INVALID_HANDLE_VALUE, "File is invalid");

    LARGE_INTEGER size;
    BOOL result = GetFileSizeEx(shaderFile, &size);
    PG_ASSERT(result, "File size error");

    char* shaderSource = (char*) malloc(size.QuadPart);
    DWORD readSize;
    result = ReadFile(shaderFile, (LPVOID)shaderSource, size.QuadPart, &readSize, NULL);
    PG_ASSERT(result, "Couldn't read file");
    PG_ASSERT(readSize == size.QuadPart, "Wanted size and actual read size don't match");
    // Put 0 char on end of the source buffer for indicating this file ended.
    //shaderSource[size.QuadPart] = '\0';
    CloseHandle(shaderFile);

    ShaderFile file = {};
    file.fileData = shaderSource;
    file.fileSize = size.QuadPart;

    return file;
}

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

    const float vertexData[] = {
         -1.0f, -1.0f,  -1.0f,
         -1.0f,  1.0f,  -1.0f,
          1.0f,  1.0f,  -1.0f,
          1.0f, -1.0f,  -1.0f,
         -1.0f, -1.0f,  1.0f,
         -1.0f,  1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,
          1.0f, -1.0f,  1.0f,
    };

    const float colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
    };

    const uint32_t indices[] = {
        0, 1, 2,
        0, 2, 3,
        4, 6, 5,
        4, 7, 6,
        4, 5, 1,
        4, 1, 0,
        3, 2, 6,
        3, 6, 7,
        1, 5, 6,
        1, 6, 2,
        4, 0, 3,
        4, 3, 7
    };

    D3D11_BUFFER_DESC vertexBufferDescriptor = {};
    vertexBufferDescriptor.ByteWidth = sizeof(vertexData);
    vertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDescriptor.MiscFlags = 0;
    vertexBufferDescriptor.StructureByteStride = sizeof(float) * 3;

    D3D11_SUBRESOURCE_DATA vertexBufferSubresourceData = {};
    vertexBufferSubresourceData.pSysMem = vertexData;

    ID3D11Buffer* vertexBuffer = nullptr;
    result = m_Device->CreateBuffer(&vertexBufferDescriptor, &vertexBufferSubresourceData, &vertexBuffer);
    PG_ASSERT(SUCCEEDED(result), "Error at creating vertex buffer");

    D3D11_BUFFER_DESC indexBufferDescriptor = {};
    indexBufferDescriptor.ByteWidth = sizeof(indices);
    indexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
    indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    indexBufferDescriptor.MiscFlags = 0;
    indexBufferDescriptor.StructureByteStride = sizeof(uint32_t) * 3;

    D3D11_SUBRESOURCE_DATA indexBufferSubresourceData = {};
    indexBufferSubresourceData.pSysMem = indices;

    ID3D11Buffer* indexBuffer = nullptr;
    result = m_Device->CreateBuffer(&indexBufferDescriptor, &indexBufferSubresourceData, &indexBuffer);
    PG_ASSERT(SUCCEEDED(result), "Error at creating index buffer");

    // Vertex Shader creation
    const char* vertexShaderFileName = "../bin/shaders/VertexShader.cso";
    ShaderFile vertexShaderFile = ReadBinaryFile(vertexShaderFileName);
    ID3D11VertexShader* vertexShader = nullptr;
    result = m_Device->CreateVertexShader(vertexShaderFile.fileData, vertexShaderFile.fileSize, 0, &vertexShader);
    PG_ASSERT(SUCCEEDED(result), "Error at creating vertex shader");

    // Input layout creation
    D3D11_INPUT_ELEMENT_DESC layoutDesciption[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    ID3D11InputLayout* inputLayout = nullptr;
    result = m_Device->CreateInputLayout(layoutDesciption, ARRAYSIZE(layoutDesciption), vertexShaderFile.fileData, vertexShaderFile.fileSize, &inputLayout);
    PG_ASSERT(SUCCEEDED(result), "Error at creating input layout");

    // Pixel shader creation
    const char* pixelShaderFileName = "../bin/shaders/PixelShader.cso";
    ShaderFile pixelShaderFile = ReadBinaryFile(pixelShaderFileName);
    ID3D11PixelShader* pixelShader = nullptr;
    result = m_Device->CreatePixelShader(pixelShaderFile.fileData, pixelShaderFile.fileSize, 0, &pixelShader);
    PG_ASSERT(SUCCEEDED(result), "Error at creating pixel shader");

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

    // Bindings
    UINT stride = sizeof(float) * 3;
    UINT offset = 0;
    m_DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    m_DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    m_DeviceContext->IASetInputLayout(inputLayout);
    m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_DeviceContext->VSSetShader(vertexShader, 0, 0);

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

    m_DeviceContext->PSSetShader(pixelShader, 0, 0);
    m_DeviceContext->PSSetConstantBuffers(0, 1, &constantBuffer);

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};

    // Depth test parameters
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    // Stencil test parameters
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    ID3D11DepthStencilState* pDSState;
    result = m_Device->CreateDepthStencilState(&dsDesc, &pDSState);
    PG_ASSERT(SUCCEEDED(result), "Error at creating depth-stencil state");

    m_DeviceContext->OMSetRenderTargets(1, &m_BackbufferRenderTargetView, m_BackbufferDepthStencilView);
    m_DeviceContext->OMSetDepthStencilState(pDSState, 0);
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

void DX11RendererAPI::Render() {
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
    m_DeviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

    m_DeviceContext->DrawIndexed(36, 0, 0);
}

void DX11RendererAPI::EndFrame() {
    HRESULT result = m_SwapChain->Present(1, 0);
    PG_ASSERT(SUCCEEDED(result), "Error at presenting");
}


IVertexBuffer* DX11RendererAPI::CreateVertexBuffer(void* bufferData, size_t size) {
    return new DX11VertexBuffer(m_Device, bufferData, size);
}

IIndexBuffer* DX11RendererAPI::CreateIndexBuffer(void* bufferData, size_t size) {
    return new DX11IndexBuffer(m_Device, bufferData, size);
}

IShaderProgram* DX11RendererAPI::CreateShaderProgram(const char* vertexShaderFileName, const char* pixelShaderFileName) {
    return new DX11ShaderProgram(m_Device, vertexShaderFileName, pixelShaderFileName);
}

IVertexInputLayout* DX11RendererAPI::CreateVertexInputLayout(std::vector<VertexInputElement> inputElements, IShaderProgram* shaderProgram) {
    return new DX11VertexInputLayout(m_Device, inputElements, (DX11ShaderProgram*) shaderProgram);
}

