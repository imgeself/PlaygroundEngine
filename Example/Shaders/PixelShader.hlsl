cbuffer Colors {
    float4 colors[6];
};

float4 main(uint pid : SV_PrimitiveID) : SV_Target
{
    return colors[pid / 2];
}