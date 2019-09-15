cbuffer Colors {
    float4 colors[6];
};

struct PSIn {
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float3 lightPos : LightPos;
    float4 pos : SV_Position;
};

float4 main(PSIn input, uint pid : SV_PrimitiveID) : SV_Target
{
    float3 color = colors[pid / 2];
    float3 lightDir = normalize(input.lightPos - input.worldPos);
    float cos = dot(lightDir, input.normal);
    cos = max(0.2f, cos);
    return float4((color * cos), 1.0f);
}