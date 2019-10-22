#ifndef _SHADER_DEFINITIONS_H_
#define _SHADER_DEFINITIONS_H_

#define PER_DRAW_CBUFFER_SLOT 0
#define PER_FRAME_CBUFFER_SLOT 1

#define SHADOW_MAP_TEXTURE2D_SLOT 0

#define POINT_BORDER_SAMPLER_STATE_SLOT 0

#ifdef _HLSL
#define CBUFFER(name, slotNumber) cbuffer name : register(b ## slotNumber)
#define TEXTURE2D(name, slotNumber) Texture2D name : register(t ## slotNumber)
#define SAMPLER_STATE(name, slotNumber) SamplerState name : register(s ## slotNumber)
#define Matrix4 row_major matrix
#define Vector4 float4
#define Vector3 float3
#else 
#define CBUFFER(name, registerNumber) struct name
#define TEXTURE2D(name, slotNumber)
#define SAMPLER_STATE(name, slotNumber)
#include "../../Math/math_util.h"

enum SystemConstantBufferSlots {
    SystemConstantBufferSlot_PerDraw = PER_DRAW_CBUFFER_SLOT,
    SystemConstantBufferSlot_PerFrame = PER_FRAME_CBUFFER_SLOT,

    SystemConstantBufferSlot_Count
};

#endif

///////////// Constant buffers
CBUFFER(PerDrawGlobalConstantBuffer, PER_DRAW_CBUFFER_SLOT) {
    Matrix4 modelMatrix;
};

CBUFFER(PerFrameGlobalConstantBuffer, PER_FRAME_CBUFFER_SLOT) { // SystemConstantBufferSlot_PerFrame = 1
    Matrix4 viewMatrix;
    Matrix4 projMatrix;
    Vector4 cameraPos;
    Vector4 lightPos;
    Matrix4 lightViewMatrix;
    Matrix4 lightProjMatrix;
};

/////////// Textures
TEXTURE2D(g_ShadowMapTexture, SHADOW_MAP_TEXTURE2D_SLOT);

SAMPLER_STATE(g_PointBorderSampler, POINT_BORDER_SAMPLER_STATE_SLOT);


#endif
