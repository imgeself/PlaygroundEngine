#include "../PlaygroundEngine/Assets/Shaders/ShaderDefinitions.h"
#include "../PlaygroundEngine/Assets/Shaders/ShadowUtils.hlsli"
#include "../PlaygroundEngine/Assets/Shaders/BRDF.hlsli"


///////////////////////////////////////////////////////////////////////////
/////// VERTEX SHADER
///////////////////////////////////////////////////////////////////////////
struct VSOut {
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float2 texCoord : TEXCOORD;
    float4 pos : SV_Position;
};

VSOut VSMain(float3 pos : POSITION, float3 normal : NORMAL, float2 texCoord : TEXCOORD) {
    float4 transformedPosition = mul(g_ProjMatrix, mul(g_ViewMatrix, mul(g_ModelMatrix, float4(pos, 1.0f))));

    VSOut vertexOut;
    vertexOut.normal = (float3) mul(g_ModelMatrix, float4(normal, 0.0f));
    vertexOut.pos = transformedPosition;
    vertexOut.worldPos = (float3) mul(g_ModelMatrix, float4(pos, 1.0f));
    vertexOut.texCoord = texCoord;

    return vertexOut;
}


///////////////////////////////////////////////////////////////////////////
/////// FRAGMENT SHADER
///////////////////////////////////////////////////////////////////////////
float4 PSMain(VSOut input) : SV_Target {
    float3 cameraPos = g_CameraPos.xyz;
    float3 lightPos = g_LightPos.xyz;

    float3 normalVector = normalize(input.normal);
    float3 viewVector = normalize(cameraPos - input.worldPos);
    float3 lightVector = normalize(lightPos - input.worldPos);

    float3 albedoColor = g_Material.diffuseColor.rgb;
    float alpha = g_Material.diffuseColor.a;
    if (g_Material.hasAlbedoTexture) {
        float4 color = g_AlbedoTexture.Sample(g_LinearWrapSampler, input.texCoord);
        albedoColor = color.rgb;
        alpha = color.a;
    }

    clip(alpha - 0.1f);

    float roughness = g_Material.roughness;
    if (g_Material.hasRoughnessTexture) {
        roughness = g_RoughnessTexture.Sample(g_LinearWrapSampler, input.texCoord).r;
    }

    float metallic = g_Material.metallic;
    if (g_Material.hasMetallicTexture) {
        metallic = g_MetallicTexture.Sample(g_LinearWrapSampler, input.texCoord).r;
    }

    if (g_Material.hasMetallicRoughnessTexture) {
        float4 t = g_MetallicRoughnessTexture.Sample(g_LinearWrapSampler, input.texCoord);
        // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
        metallic = t.b;
        roughness = t.g;
    }

    float ao = 1.0f;
    if (g_Material.hasAOTexture) {
        ao = g_AOTexture.Sample(g_LinearWrapSampler, input.texCoord).r;
    }

    float3 Lo = BRDF(lightVector, normalVector, viewVector, albedoColor, roughness, metallic);

    uint hitCascadeIndex;
    float shadowFactor = CalculateShadowValue(input.worldPos, hitCascadeIndex);

    // Visualize cascades
    float3 cascadeVisualizeColors[4] = {
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f,
    };

    float3 lightColor = (float3) 1.0f;
    float intensity = 5.0f;
    float3 cascadeColor = cascadeVisualizeColors[hitCascadeIndex];

    float3 f0 = lerp((float3) 0.04f, albedoColor, metallic);
    float NdotV = saturate(dot(normalVector, viewVector));
    float3 kS = FresnelSchlickRoughness(NdotV, f0, roughness);
    float3 kD = 1.0f - kS;
    kD *= 1.0 - metallic;

    float3 eyeVector = -viewVector;
    float3 reflectedVector = reflect(eyeVector, input.normal);
    float3 radiance = g_RadianceTexture.SampleLevel(g_LinearWrapSampler, reflectedVector, roughness * 6.0f).rgb;
    float3 irradiance = g_IrradianceTexture.Sample(g_LinearWrapSampler, input.normal).rgb;
    
    float2 lutVector = float2(NdotV, roughness);
    float2 envBRDF = g_EnvBrdfTexture.Sample(g_LinearClampSampler, lutVector).rg;
    
    float3 diffuseAmbient = kD * (irradiance * albedoColor);
    float3 ambient = diffuseAmbient + radiance * (f0 * envBRDF.x + envBRDF.y);
    ambient *= ao * 0.15f;

    float3 color = Lo * lightColor * intensity * shadowFactor + ambient;

    return float4(color, alpha);
}
