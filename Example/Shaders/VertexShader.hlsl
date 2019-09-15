
cbuffer Cbuff {
    row_major matrix transform;
    row_major matrix viewMatrix;
    row_major matrix projMatrix;
    float4 lightPos;
    float4 cameraPos;
};

struct VSOut {
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float3 lightPos : LightPos;
    float3 cameraPos : CameraPos;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 normal : Normal)
{
    float4 transformedPosition = mul(projMatrix, mul(viewMatrix, mul(transform, float4(pos, 1.0f))));

    VSOut vertexOut;
    vertexOut.normal = (float3) mul(transform, float4(normal, 0.0f));
    vertexOut.pos = transformedPosition;
    vertexOut.worldPos = (float3) mul(transform, float4(pos, 1.0f));
    vertexOut.lightPos = (float3) lightPos;
    vertexOut.cameraPos = (float3) cameraPos;

	return vertexOut;
}