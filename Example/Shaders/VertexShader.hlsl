
cbuffer Cbuff {
    row_major matrix transform;
    row_major matrix projMatrix;
};

float4 main(float3 pos : POSITION) : SV_Position
{
    float4 transformedPosition = mul(projMatrix, mul(transform, float4(pos, 1.0f)));
	return transformedPosition;
}