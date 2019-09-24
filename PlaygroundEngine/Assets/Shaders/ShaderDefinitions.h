#ifndef _SHADER_DEFINITIONS_H_
#define _SHADER_DEFINITIONS_H_

#ifdef _HLSL
#define CBUFFER(name, registerNumber) cbuffer name : register(registerNumber)
#define Matrix4 row_major matrix
#define Vector4 float4
#define Vector3 float3
#else 
#define CBUFFER(name, registerNumber) struct name
#include "../../Math/math_util.h"
#endif

CBUFFER(PerFrameData, b0) {
    Matrix4 modelMatrix;
    Matrix4 viewMatrix;
    Matrix4 projMatrix;
    Vector4 lightPos;
    Vector4 cameraPos;
};

#endif
