#include "../PlaygroundEngine/Assets/Shaders/ShaderDefinitions.h"


///////////////////////////////////////////////////////////////////////////
/////// VERTEX SHADER
///////////////////////////////////////////////////////////////////////////
struct VSOut {
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float3 lightPos : LightPos;
    float3 cameraPos : CameraPos;
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
    float3 color = (float3) colors[pid / 2];

    // Diffuse
    float3 lightDir = normalize(input.lightPos - input.worldPos);
    float diffuse = dot(lightDir, input.normal);
    diffuse = max(0.2f, diffuse);

    // Sepecular
    float specularStrenght = 0.8f;
    float3 reflectedVector = reflect(-lightDir, input.normal);
    float3 viewDir = normalize(input.cameraPos - input.worldPos);
    float specular = max(dot(reflectedVector, viewDir), 0.0f);
    specular = pow(specular, 64);
    specular *= specularStrenght;

    return float4((color * (diffuse + specular)), 1.0f);
}

