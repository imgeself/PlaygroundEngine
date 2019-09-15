cbuffer Cbuff {
    row_major matrix transform;
    row_major matrix viewMatrix;
    row_major matrix projMatrix;
    float4 lightPos;
    float4 cameraPos;
};


float4 main(float3 pos : Position, float3 normal : Normal) : SV_POSITION
{
	return mul(projMatrix, mul(viewMatrix, mul(transform, float4(pos, 1.0f))));
}