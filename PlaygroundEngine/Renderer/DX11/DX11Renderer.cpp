#include "DX11Renderer.h"

#define SAFE_RELEASE(p) { if(p) { p->Release(); } }

struct ShaderFile {
    char* fileData;
    size_t fileSize;
};

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

DX11Renderer::DX11Renderer(PGWindow* window) {

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

    const float vertexData[] = {
          0.0f, 0.5f, 1.0f,
          0.5f, -0.5f, 1.0f,
         -0.5f, -0.5f, 1.0f,
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

    // Bindings
    UINT stride = sizeof(float) * 3;
    UINT offset = 0;
    m_DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    m_DeviceContext->IASetInputLayout(inputLayout);
    m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_DeviceContext->VSSetShader(vertexShader, 0, 0);

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    
    m_DeviceContext->RSSetViewports(1, &viewport);

    m_DeviceContext->PSSetShader(pixelShader, 0, 0);


}

DX11Renderer::~DX11Renderer() {
    SAFE_RELEASE(m_BackbufferRenderTargetView);
    SAFE_RELEASE(m_SwapChain);
    SAFE_RELEASE(m_Device);
    SAFE_RELEASE(m_DeviceContext);
}

void DX11Renderer::ClearScreen(const float* color) {
    m_DeviceContext->OMSetRenderTargets(1, &m_BackbufferRenderTargetView, nullptr);
    m_DeviceContext->ClearRenderTargetView(m_BackbufferRenderTargetView, color);

    m_DeviceContext->Draw(3, 0);
}

void DX11Renderer::EndFrame() {
    HRESULT result = m_SwapChain->Present(1, 0);
    m_DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    PG_ASSERT(SUCCEEDED(result), "Error at presenting");
}

