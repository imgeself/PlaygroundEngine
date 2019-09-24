#include "../PlaygroundEngine/Assets/Shaders/ShaderDefinitions.h"

float4 main(float3 pos : Position, float3 normal : Normal) : SV_POSITION
{
	return mul(projMatrix, mul(viewMatrix, mul(modelMatrix, float4(pos, 1.0f))));
}