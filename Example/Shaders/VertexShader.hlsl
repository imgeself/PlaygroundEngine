
cbuffer Cbuff {
    row_major matrix transform;
};

float4 main(float3 pos : POSITION) : SV_Position
{
    float4 transformedPosition = mul(transform, float4(pos, 1.0f));
	return transformedPosition;
}