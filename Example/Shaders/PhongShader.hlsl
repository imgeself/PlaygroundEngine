#include "../PlaygroundEngine/Assets/Shaders/ShaderDefinitions.h"


///////////////////////////////////////////////////////////////////////////
/////// VERTEX SHADER
///////////////////////////////////////////////////////////////////////////
struct VSOut {
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float3 lightPos : LightPos;
    float3 cameraPos : CameraPos;
    float4 worldPosLightSpace : WordPosInLightSpace;
    float4 pos : SV_Position;
};

VSOut VSMain(float3 pos : Position, float3 normal : Normal)
{
    float4 transformedPosition = mul(projMatrix, mul(viewMatrix, mul(modelMatrix, float4(pos, 1.0f))));

    VSOut vertexOut;
    vertexOut.normal = (float3) mul(modelMatrix, float4(normal, 0.0f));
    vertexOut.pos = transformedPosition;
    vertexOut.worldPos = (float3) mul(modelMatrix, float4(pos, 1.0f));
    vertexOut.lightPos = (float3) lightPos;
    vertexOut.cameraPos = (float3) cameraPos;
    vertexOut.worldPosLightSpace = mul(lightProjMatrix, mul(lightViewMatrix, mul(modelMatrix, float4(pos, 1.0f))));

    return vertexOut;
}


///////////////////////////////////////////////////////////////////////////
/////// FRAGMENT SHADER
///////////////////////////////////////////////////////////////////////////

float4 PSMain(VSOut input, uint pid : SV_PrimitiveID) : SV_Target
{
    float4 colors[6] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
    };
    float3 color = float3(0.7f, 0.8f, 0.9f); //(float3) colors[pid / 2];


    // Diffuse
    float3 lightDir = normalize(input.lightPos - input.worldPos);
    float diffuse = dot(lightDir, input.normal);
    diffuse = max(0.2f, diffuse);

    // Sepecular
    float specularStrenght = 0.3f;
    float3 reflectedVector = reflect(-lightDir, input.normal);
    float3 viewDir = normalize(input.cameraPos - input.worldPos);
    float specular = max(dot(reflectedVector, viewDir), 0.0f);
    specular = pow(specular, 32);
    specular *= specularStrenght;

    // ShadowCoordinates
    float3 shadowCoord = input.worldPosLightSpace.xyz / input.worldPosLightSpace.w;
    shadowCoord.x = shadowCoord.x * 0.5f + 0.5f;
    shadowCoord.y = shadowCoord.y * -0.5f + 0.5f;

    float sampleDepth = g_ShadowMapTexture.Sample(g_PointBorderSampler, shadowCoord.xy).r;
    float bias = 0.0004f;
    float shadowFactor = sampleDepth < shadowCoord.z - bias ? 0.2f : 1.0f;

    return float4((color * shadowFactor * (diffuse + specular)), 1.0f);
}

