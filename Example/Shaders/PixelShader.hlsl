cbuffer Colors {
    float4 colors[6];
};

struct PSIn {
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float3 lightPos : LightPos;
    float3 cameraPos : CameraPos;
    float4 pos : SV_Position;
};

float4 main(PSIn input, uint pid : SV_PrimitiveID) : SV_Target
{
    float3 color = colors[pid / 2];

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