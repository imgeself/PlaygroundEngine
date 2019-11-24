#include "../PlaygroundEngine/Assets/Shaders/ShaderDefinitions.h"
#include "../PlaygroundEngine/Assets/Shaders/ShadowUtils.hlsli"
#include "../PlaygroundEngine/Assets/Shaders/BRDF.hlsli"


///////////////////////////////////////////////////////////////////////////
/////// VERTEX SHADER
///////////////////////////////////////////////////////////////////////////
struct VSOut {
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float4 pos : SV_Position;
};

VSOut VSMain(float3 pos : Position, float3 normal : Normal) {
    float4 transformedPosition = mul(g_ProjMatrix, mul(g_ViewMatrix, mul(g_ModelMatrix, float4(pos, 1.0f))));

    VSOut vertexOut;
    vertexOut.normal = (float3) mul(g_ModelMatrix, float4(normal, 0.0f));
    vertexOut.pos = transformedPosition;
    vertexOut.worldPos = (float3) mul(g_ModelMatrix, float4(pos, 1.0f));

    return vertexOut;
}


///////////////////////////////////////////////////////////////////////////
/////// FRAGMENT SHADER
///////////////////////////////////////////////////////////////////////////
float4 PSMain(VSOut input, uint pid : SV_PrimitiveID) : SV_Target {
    float3 cameraPos = g_CameraPos.xyz;
    float3 lightPos = g_LightPos.xyz;

    float3 normalVector = normalize(input.normal);
    float3 viewVector = normalize(cameraPos - input.worldPos);
    float3 lightVector = normalize(lightPos - input.worldPos);

    float3 Lo = BRDF(lightVector, normalVector, viewVector, g_Material.diffuseColor.xyz, g_Material.roughness, g_Material.metallic);

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
    float intensity = 2.0f;
    float3 cascadeColor = cascadeVisualizeColors[hitCascadeIndex];

    float3 color = Lo * lightColor * intensity * shadowFactor;

    // Gamma correction
    color = pow(color, (float3)(1.0 / 2.2));

    return float4(color, 1.0f);
}