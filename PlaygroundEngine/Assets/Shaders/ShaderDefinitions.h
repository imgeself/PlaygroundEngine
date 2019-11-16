#ifndef _SHADER_DEFINITIONS_H_
#define _SHADER_DEFINITIONS_H_

#define PER_DRAW_CBUFFER_SLOT 0
#define PER_FRAME_CBUFFER_SLOT 1
#define PER_SHADOWGEN_CBUFFER_SLOT 2

#define RENDERER_VARIABLES_CBUFFER_SLOT 7

#define SHADOW_MAP_TEXTURE2D_SLOT 0

#define SHADOW_SAMPLER_COMPARISON_STATE_SLOT 0
#define POINT_CLAMP_SAMPLER_STATE_SLOT 1

// Variable defines
#define MAX_SHADOW_CASCADE_COUNT 5

#ifdef _HLSL
#define CBUFFER(name, slotNumber) cbuffer name : register(b ## slotNumber)
#define TEXTURE2D(name, slotNumber) Texture2D name : register(t ## slotNumber)
#define TEXTURE2DARRAY(name, slotNumber) Texture2DArray name : register(t ## slotNumber)
#define SAMPLER_STATE(name, slotNumber) SamplerState name : register(s ## slotNumber)
#define SAMPLER_COMPARISON_STATE(name, slotNumber) SamplerComparisonState name : register(s ## slotNumber)
#define Matrix4 row_major matrix
#define Vector4 float4
#define Vector3 float3
#define Vector2 float2
#define uint32_t uint
#else 
#define CBUFFER(name, registerNumber) struct name
#define TEXTURE2D(name, slotNumber)
#define TEXTURE2DARRAY(name, slotNumber)
#define SAMPLER_STATE(name, slotNumber)
#define SAMPLER_COMPARISON_STATE(name, slotNumber)
#include "../../Math/math_util.h"
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
    Matrix4 lightProjMatrix[MAX_SHADOW_CASCADE_COUNT];

    Matrix4 g_InverseViewProjMatrix;
    Vector4 g_ScreenDimensions;
};

CBUFFER(PerShadowGenConstantBuffer, PER_SHADOWGEN_CBUFFER_SLOT) {
    uint32_t g_ShadowGenCascadeIndex;
    Vector3 pad00000001;
};

CBUFFER(RendererVariablesConstantBuffer, RENDERER_VARIABLES_CBUFFER_SLOT) {
    uint32_t g_ShadowCascadeCount;
    uint32_t g_ShadowMapSize;
    Vector2 pad0000002;
};

/////////// Textures
TEXTURE2DARRAY(g_ShadowMapTexture, SHADOW_MAP_TEXTURE2D_SLOT);

/////////// Samplers
SAMPLER_COMPARISON_STATE(g_ShadowMapSampler, SHADOW_SAMPLER_COMPARISON_STATE_SLOT);
SAMPLER_STATE(g_PointClampSampler, POINT_CLAMP_SAMPLER_STATE_SLOT);


#endif
