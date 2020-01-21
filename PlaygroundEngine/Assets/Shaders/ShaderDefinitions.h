#ifndef _SHADER_DEFINITIONS_H_
#define _SHADER_DEFINITIONS_H_

#define PER_DRAW_CBUFFER_SLOT 0
#define PER_FRAME_CBUFFER_SLOT 1
#define PER_SHADOWGEN_CBUFFER_SLOT 2
#define POST_PROCESS_CBUFFER_SLOT 3

#define RENDERER_VARIABLES_CBUFFER_SLOT 7

// Texture slots
#define SHADOW_MAP_TEXTURE2D_SLOT 0

#define ALBEDO_TEXTURE2D_SLOT 1
#define ROUGHNESS_TEXTURE2D_SLOT 2
#define METALLIC_TEXTURE2D_SLOT 3
#define AO_TEXTURE2D_SLOT 4

#define RADIANCE_TEXTURECUBE_SLOT 5
#define IRRADIANCE_TEXTURECUBE_SLOT 6
#define BRDF_LUT_TEXTURE_SLOT 7

#define SKYBOX_TEXTURECUBE_SLOT 5

#define POST_PROCESS_TEXTURE0_SLOT 10

// Samler state slots
#define SHADOW_SAMPLER_COMPARISON_STATE_SLOT 0
#define POINT_CLAMP_SAMPLER_STATE_SLOT 1
#define LINEAR_CLAMP_SAMPLER_STATE_SLOT 2
#define LINEAR_WRAP_SAMPLER_STATE_SLOT 3
#define POINT_WRAP_SAMPLER_STATE_SLOT 4

// Variable defines
#define MAX_SHADOW_CASCADE_COUNT 5

#ifdef _HLSL
#define CBUFFER(name, slotNumber) cbuffer name : register(b ## slotNumber)
#define TEXTURE2D(name, slotNumber) Texture2D name : register(t ## slotNumber)
#define TEXTURE2DARRAY(name, slotNumber) Texture2DArray name : register(t ## slotNumber)
#define TEXTURECUBE(name, slotNumber) TextureCube name : register(t ## slotNumber)
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
#define TEXTURECUBE(name, slotNumber)
#define SAMPLER_STATE(name, slotNumber)
#define SAMPLER_COMPARISON_STATE(name, slotNumber)
#include "../../Math/math_util.h"
#endif

struct DrawMaterial {
    Vector4 ambientColor;
    Vector4 emissiveColor;
    Vector4 diffuseColor;

    float roughness;
    float metallic;
    float indexOfRefraction;
    float opacity;

    uint32_t hasAlbedoTexture;
    uint32_t hasRoughnessTexture;
    uint32_t hasMetallicTexture;
    uint32_t hasAOTexture;
};

///////////// Constant buffers
CBUFFER(PerDrawGlobalConstantBuffer, PER_DRAW_CBUFFER_SLOT) {
    Matrix4 g_ModelMatrix;
    DrawMaterial g_Material;
};

CBUFFER(PerFrameGlobalConstantBuffer, PER_FRAME_CBUFFER_SLOT) {
    Matrix4 g_ViewMatrix;
    Matrix4 g_ProjMatrix;
    Vector4 g_CameraPos;
    Vector4 g_LightPos;
    Matrix4 g_LightViewMatrix;
    Matrix4 g_LightProjMatrices[MAX_SHADOW_CASCADE_COUNT];

    Matrix4 g_InverseViewProjMatrix;
    Vector4 g_ScreenDimensions;
};

CBUFFER(PerShadowGenConstantBuffer, PER_SHADOWGEN_CBUFFER_SLOT) {
    uint32_t g_ShadowGenCascadeIndex;
    Vector3 pad00000001;
};

CBUFFER(PostProcessConstantBuffer, POST_PROCESS_CBUFFER_SLOT) {
    float g_PPExposure;
    float g_PPGamma;
    Vector2 pad0003;
};

CBUFFER(RendererVariablesConstantBuffer, RENDERER_VARIABLES_CBUFFER_SLOT) {
    uint32_t g_ShadowCascadeCount;
    uint32_t g_ShadowMapSize;
    Vector2 pad0000002;
};

/////////// Textures
TEXTURE2DARRAY(g_ShadowMapTexture, SHADOW_MAP_TEXTURE2D_SLOT);

// PBR textures
TEXTURE2D(g_AlbedoTexture, ALBEDO_TEXTURE2D_SLOT);
TEXTURE2D(g_RoughnessTexture, ROUGHNESS_TEXTURE2D_SLOT);
TEXTURE2D(g_MetallicTexture, METALLIC_TEXTURE2D_SLOT);
TEXTURE2D(g_AOTexture, AO_TEXTURE2D_SLOT);

TEXTURECUBE(g_RadianceTexture, RADIANCE_TEXTURECUBE_SLOT);
TEXTURECUBE(g_IrradianceTexture, IRRADIANCE_TEXTURECUBE_SLOT);
TEXTURE2D(g_EnvBrdfTexture, BRDF_LUT_TEXTURE_SLOT);

// Skybox
TEXTURECUBE(g_SkyboxTexture, SKYBOX_TEXTURECUBE_SLOT);

// Post process textures
TEXTURE2D(g_PostProcessTexture0, POST_PROCESS_TEXTURE0_SLOT);

/////////// Samplers
SAMPLER_COMPARISON_STATE(g_ShadowMapSampler, SHADOW_SAMPLER_COMPARISON_STATE_SLOT);

SAMPLER_STATE(g_PointClampSampler,   POINT_CLAMP_SAMPLER_STATE_SLOT);
SAMPLER_STATE(g_PointWrapSampler,    POINT_WRAP_SAMPLER_STATE_SLOT);
SAMPLER_STATE(g_LinearWrapSampler,   LINEAR_WRAP_SAMPLER_STATE_SLOT);
SAMPLER_STATE(g_LinearClampSampler,  LINEAR_CLAMP_SAMPLER_STATE_SLOT);


#endif
