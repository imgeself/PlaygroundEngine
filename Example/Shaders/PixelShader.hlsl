cbuffer Colors {
    float4 colors[6];
};

float4 main(uint pid : SV_PrimitiveID) : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f); // colors[pid / 2];
}