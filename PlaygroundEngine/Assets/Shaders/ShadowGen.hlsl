#include "../PlaygroundEngine/Assets/Shaders/ShaderDefinitions.h"

///////////////////////////////////////////////////////////////////////////
/////// VERTEX SHADER
///////////////////////////////////////////////////////////////////////////

struct VSInput {
    float3 pos : POSITION;
#ifdef ALPHA_TEST
    float2 texCoord : TEXCOORD;
#endif
};

struct VSOut {
#ifdef ALPHA_TEST
    float2 texCoord : TEXCOORD;
#endif
    float4 pos : SV_Position;
    uint depthSlice : SV_RenderTargetArrayIndex;
};

VSOut VSMain(VSInput input) {
    VSOut output;
#ifdef ALPHA_TEST
    output.texCoord = input.texCoord;
#endif
    output.pos = mul(g_LightProjMatrices[g_ShadowGenCascadeIndex], mul(g_LightViewMatrix, mul(g_ModelMatrix, float4(input.pos, 1.0f))));
    output.depthSlice = g_ShadowGenCascadeIndex;
    return output;
}

///////////////////////////////////////////////////////////////////////////
/////// FRAGMENT SHADER
///////////////////////////////////////////////////////////////////////////
#ifdef ALPHA_TEST
void PSMain(VSOut input) {
    float alpha = g_Material.diffuseColor.a;
    if (g_Material.hasAlbedoTexture) {
        alpha = g_AlbedoTexture.Sample(g_LinearWrapSampler, input.texCoord).a;
    }

    clip(alpha - 0.1f);
}
#else
void PSMain() {

}
#endif

