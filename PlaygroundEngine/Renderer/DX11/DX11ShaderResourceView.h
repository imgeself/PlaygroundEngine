#pragma once

#include "../HWShaderResourceView.h"

#include <d3d11.h>

class DX11ShaderResourceView : public HWShaderResourceView {
public:
    DX11ShaderResourceView(ID3D11Device* device, ID3D11Texture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName = 0);
    ~DX11ShaderResourceView();

    inline ID3D11ShaderResourceView* GetDXShaderResouceView() { return m_ShaderResourceView; }

private:
    ID3D11ShaderResourceView* m_ShaderResourceView = nullptr;
};

class DX11UnorderedAccessView : public HWUnorderedAccessView {
public:
    DX11UnorderedAccessView(ID3D11Device* device, ID3D11Texture2D* texture, const HWResourceViewDesc& resourceViewDesc, const char* debugName = 0);
    ~DX11UnorderedAccessView();

    inline ID3D11UnorderedAccessView* GetDXUnorderedAccessView() { return m_UnorderedAccessView; }

private:
    ID3D11UnorderedAccessView* m_UnorderedAccessView = nullptr;
};

