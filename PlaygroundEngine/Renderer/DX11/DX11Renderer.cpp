#include "DX11Renderer.h"

#define SAFE_RELEASE(p) { if(p) { p->Release(); } }

DX11Renderer::DX11Renderer(PGWindow* window) {

    // NOTE: We don't specify refresh rate at the moment.
    // Both numerator and denominator are 0;
    DXGI_RATIONAL refreshRate = {};

    RECT rect;
    GetClientRect(window->GetWindowHandle(), &rect);

    DXGI_MODE_DESC modeDescriptor = {};
    modeDescriptor.Width = rect.right - rect.left;
    modeDescriptor.Height = rect.bottom - rect.top;
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
    swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
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
}

DX11Renderer::~DX11Renderer() {
    SAFE_RELEASE(m_BackbufferRenderTargetView);
    SAFE_RELEASE(m_SwapChain);
    SAFE_RELEASE(m_Device);
    SAFE_RELEASE(m_DeviceContext);
}

void DX11Renderer::ClearScreen(const float* color) {
    m_DeviceContext->ClearRenderTargetView(m_BackbufferRenderTargetView, color);
}

void DX11Renderer::EndFrame() {
    HRESULT result = m_SwapChain->Present(0, 0);
    PG_ASSERT(SUCCEEDED(result), "Error at presenting");
}

