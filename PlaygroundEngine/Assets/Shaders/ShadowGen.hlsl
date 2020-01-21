#include "../PlaygroundEngine/Assets/Shaders/ShaderDefinitions.h"

///////////////////////////////////////////////////////////////////////////
/////// VERTEX SHADER
///////////////////////////////////////////////////////////////////////////
struct VSOut {
    float4 pos : SV_Position;
    uint depthSlice : SV_RenderTargetArrayIndex;
};
VSOut VSMain(float3 pos : POSITION, float3 normal : NORMAL, float2 texCoord : TEXCOORD) {
    VSOut output;
    output.pos = mul(g_LightProjMatrices[g_ShadowGenCascadeIndex], mul(g_LightViewMatrix, mul(g_ModelMatrix, float4(pos, 1.0f))));
    output.depthSlice = g_ShadowGenCascadeIndex;
    return output;
}

///////////////////////////////////////////////////////////////////////////
/////// FRAGMENT SHADER
///////////////////////////////////////////////////////////////////////////
void PSMain()
{
}

