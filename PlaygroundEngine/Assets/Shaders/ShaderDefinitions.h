#ifndef _SHADER_DEFINITIONS_H_
#define _SHADER_DEFINITIONS_H_

#ifdef _HLSL
#define CBUFFER(name, slotNumber) cbuffer name : register(b ## slotNumber)
#define Matrix4 row_major matrix
#define Vector4 float4
#define Vector3 float3
#else 
#define CBUFFER(name, registerNumber) struct name
#include "../../Math/math_util.h"

enum SystemConstantBufferSlots {
    SystemConstantBufferSlot_PerDraw,
    SystemConstantBufferSlot_PerFrame,

    SystemConstantBufferSlot_Count
};

#endif

CBUFFER(PerDrawGlobalConstantBuffer, 0) { // SystemConstantBufferSlot_PerDraw = 0
    Matrix4 modelMatrix;
};

CBUFFER(PerFrameGlobalConstantBuffer, 1) { // SystemConstantBufferSlot_PerFrame = 1
    Matrix4 viewMatrix;
    Matrix4 projMatrix;
    Vector4 lightPos;
    Vector4 cameraPos;
};

#endif
