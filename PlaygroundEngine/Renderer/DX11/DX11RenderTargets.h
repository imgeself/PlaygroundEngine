#pragma once

#include "../../Core.h"
#include "../HWRenderTarget.h"

#include <d3d11.h>

class DX11RenderTargetView : public HWRenderTargetView {
public:
    DX11RenderTargetView(ID3D11Device* device, ID3D11Texture2D* texture, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount);
    ~DX11RenderTargetView();

    inline ID3D11RenderTargetView* GetDXRenderTargetView() { return m_RenderTargetView; }

private:
    ID3D11RenderTargetView* m_RenderTargetView = nullptr;
};

class DX11DepthStencilView : public HWDepthStencilView {
public:
    DX11DepthStencilView(ID3D11Device* device, ID3D11Texture2D* texture, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount);
    ~DX11DepthStencilView();

    inline ID3D11DepthStencilView* GetDXDepthStencilView() { return m_DepthStencilView; }

private:
    ID3D11DepthStencilView* m_DepthStencilView = nullptr;
};

