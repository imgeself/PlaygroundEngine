#ifndef _SHADER_DEFINITIONS_H_
#define _SHADER_DEFINITIONS_H_

#define PER_DRAW_CBUFFER_SLOT 0
#define PER_MATERIAL_CBUFFER_SLOT 1
#define PER_VIEW_CBUFFER_SLOT 2
#define PER_FRAME_CBUFFER_SLOT 3
#define POST_PROCESS_CBUFFER_SLOT 5

#define RENDERER_VARIABLES_CBUFFER_SLOT 7

#define UTILITY_CBUFFER_SLOT 10

// Texture slots
#define SHADOW_MAP_TEXTURE2D_SLOT 0

#define ALBEDO_TEXTURE2D_SLOT 1
#define ROUGHNESS_TEXTURE2D_SLOT 2
#define METALLIC_TEXTURE2D_SLOT 3
#define AO_TEXTURE2D_SLOT 4
#define METALLIC_ROUGHNESS_TEXTURE2D_SLOT 5
#define NORMAL_TEXTURE2D_SLOT 6
#define EMISSIVE_TEXTURE2D_SLOT 7

#define RADIANCE_TEXTURECUBE_SLOT 8
#define IRRADIANCE_TEXTURECUBE_SLOT 9
#define BRDF_LUT_TEXTURE_SLOT 10

#define SKYBOX_TEXTURECUBE_SLOT 5

#define POST_PROCESS_TEXTURE0_SLOT 10

// Samler state slots
#define SHADOW_SAMPLER_COMPARISON_STATE_SLOT 0
#define POINT_CLAMP_SAMPLER_STATE_SLOT 1
#define LINEAR_CLAMP_SAMPLER_STATE_SLOT 2
#define LINEAR_WRAP_SAMPLER_STATE_SLOT 3
#define POINT_WRAP_SAMPLER_STATE_SLOT 4
#define OBJECT_SAMPLER_STATE_SLOT 5


// Variable defines
#define MAX_SHADOW_CASCADE_COUNT 5

#ifdef _HLSL
#define Matrix4 row_major matrix
#define Vector4 float4
#define Vector3 float3
#define Vector2 float2
#define uint32_t uint
#define CBUFFER(name, slotNumber) cbuffer name : register(b ## slotNumber)
#define TEXTURE2D(name, format, slotNumber) Texture2D<format> name : register(t ## slotNumber)
#define RWTEXTURE2D(name, format, slotNumber) RWTexture2D<format> name : register(t ## slotNumber)
#define TEXTURE2DARRAY(name, format, slotNumber) Texture2DArray<format> name : register(t ## slotNumber)
#define RWTEXTURE2DARRAY(name, format, slotNumber) RWTexture2DArray<format> name : register(u ## slotNumber)
#define TEXTURECUBE(name, format, slotNumber) TextureCube<format> name : register(t ## slotNumber)
#define SAMPLER_STATE(name, slotNumber) SamplerState name : register(s ## slotNumber)
#define SAMPLER_COMPARISON_STATE(name, slotNumber) SamplerComparisonState name : register(s ## slotNumber)
#else 
#define CBUFFER(name, registerNumber) struct name
#define TEXTURE2D(name, format, slotNumber)
#define RWTEXTURE2D(name, format, slotNumber)
#define TEXTURE2DARRAY(name, format, slotNumber)
#define RWTEXTURE2DARRAY(name, format, slotNumber)
#define TEXTURECUBE(name, format, slotNumber)
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

    uint32_t hasAlbedoTexture;
    uint32_t hasRoughnessTexture;
    uint32_t hasMetallicTexture;
    uint32_t hasAOTexture;

    uint32_t hasMetallicRoughnessTexture;
    uint32_t hasEmissiveTexture;
};

///////////// Constant buffers
CBUFFER(PerDrawGlobalConstantBuffer, PER_DRAW_CBUFFER_SLOT) {
    Matrix4 g_ModelMatrix;
};

CBUFFER(PerMaterialGlobalConstantBuffer, PER_MATERIAL_CBUFFER_SLOT) {
    DrawMaterial g_Material;
};

CBUFFER(PerViewGlobalConstantBuffer, PER_VIEW_CBUFFER_SLOT) {
    Matrix4 g_ViewMatrix;
    Matrix4 g_ProjMatrix;
    Matrix4 g_ProjViewMatrix;
    Vector4 g_CameraPos;

    Matrix4 g_InverseViewProjMatrix;
};

CBUFFER(PerFrameGlobalConstantBuffer, PER_FRAME_CBUFFER_SLOT) {
    Vector4 g_DirectionLightDirection;
    Vector4 g_DirectionLightColor; // xyz color, w intensity
    Matrix4 g_DirectionLightViewMatrix;
    Matrix4 g_DirectionLightProjMatrices[MAX_SHADOW_CASCADE_COUNT];

    Vector4 g_ScreenDimensions;
};


CBUFFER(PostProcessConstantBuffer, POST_PROCESS_CBUFFER_SLOT) {
    float g_PPExposure;
    float g_PPGamma;
    Vector2 pad0003;
};

CBUFFER(RendererVariablesConstantBuffer, RENDERER_VARIABLES_CBUFFER_SLOT) {
    uint32_t g_ShadowCascadeCount;
    uint32_t g_ShadowMapSize;

    // Debug
    uint32_t g_VisualizeCascades;
    uint32_t pad0000002;
};

/////////// Textures
TEXTURE2DARRAY(g_ShadowMapTexture, float, SHADOW_MAP_TEXTURE2D_SLOT);

// PBR textures
TEXTURE2D(g_AlbedoTexture, float4, ALBEDO_TEXTURE2D_SLOT);
TEXTURE2D(g_RoughnessTexture, float4, ROUGHNESS_TEXTURE2D_SLOT);
TEXTURE2D(g_MetallicTexture, float4, METALLIC_TEXTURE2D_SLOT);
TEXTURE2D(g_AOTexture, float4, AO_TEXTURE2D_SLOT);
TEXTURE2D(g_MetallicRoughnessTexture, float4, METALLIC_ROUGHNESS_TEXTURE2D_SLOT);
TEXTURE2D(g_NormalTexture, float4, NORMAL_TEXTURE2D_SLOT);
TEXTURE2D(g_EmissiveTexture, float4, EMISSIVE_TEXTURE2D_SLOT);

TEXTURECUBE(g_RadianceTexture, float4, RADIANCE_TEXTURECUBE_SLOT);
TEXTURECUBE(g_IrradianceTexture, float4, IRRADIANCE_TEXTURECUBE_SLOT);
TEXTURE2D(g_EnvBrdfTexture, float2, BRDF_LUT_TEXTURE_SLOT);

// Skybox
TEXTURECUBE(g_SkyboxTexture, float4, SKYBOX_TEXTURECUBE_SLOT);

// Post process textures
TEXTURE2D(g_PostProcessTexture0, float4, POST_PROCESS_TEXTURE0_SLOT);

/////////// Samplers
SAMPLER_COMPARISON_STATE(g_ShadowMapSampler, SHADOW_SAMPLER_COMPARISON_STATE_SLOT);

SAMPLER_STATE(g_PointClampSampler,   POINT_CLAMP_SAMPLER_STATE_SLOT);
SAMPLER_STATE(g_PointWrapSampler,    POINT_WRAP_SAMPLER_STATE_SLOT);
SAMPLER_STATE(g_LinearWrapSampler,   LINEAR_WRAP_SAMPLER_STATE_SLOT);
SAMPLER_STATE(g_LinearClampSampler,  LINEAR_CLAMP_SAMPLER_STATE_SLOT);
SAMPLER_STATE(g_ObjectSampler,       OBJECT_SAMPLER_STATE_SLOT);



#endif
