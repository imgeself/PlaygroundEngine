#pragma once

#include "../Core.h"
#include "../MeshUtils.h"
#include "../Scene.h"
#include "../PGProfiler.h"

#include "HWRendererAPI.h"

#include "PGRendererResources.h"

const size_t MAX_RENDER_LIST_ELEMENT_COUNT = 1024;

enum RenderListType {
    RL_OPAQUE,
    RL_TRANSPARENT,

    RL_SHADOW_GEN_CASCADE_1,
    RL_SHADOW_GEN_CASCADE_2,
    RL_SHADOW_GEN_CASCADE_3,
    RL_SHADOW_GEN_CASCADE_4,
    RL_SHADOW_GEN_CASCADE_5,

    RL_COUNT
};

struct RenderList {
    struct Element {
        SubMesh* mesh;
        Transform transform;
        // the index of pipeline state in PGRendererResources::s_CachedPipelineStates
        uint8_t pipelineStates[SCENE_PASS_TYPE_COUNT] = {0};

        union DepthKey {
            struct {
                uint64_t mesh  : 16;
                uint64_t depth : 32;
                uint64_t pipeline : 16;
            };

            uint64_t key;
        } depthKey;

        union SortKey {
            struct {
                //from least significant to most significant in sort
                uint64_t mesh     : 32;
                uint64_t material : 16;
                uint64_t pipeline : 16;
            };

            uint64_t key;
        } sortKey;

    };

    std::array<RenderList::Element, MAX_RENDER_LIST_ELEMENT_COUNT> elements;
    uint32_t elementCount = 0;

    void AddSceneObjects(const PGSceneObject* sceneObjects, size_t sceneObjectCount, PGCamera* sceneCamera) {
        PG_PROFILE_FUNCTION();
        for (size_t sceneObjectIndex = 0; sceneObjectIndex < sceneObjectCount; ++sceneObjectIndex) {
            const PGSceneObject* sceneObject = sceneObjects + sceneObjectIndex;

            for (SubMesh* submesh : sceneObject->mesh->submeshes) {
                RenderList::Element& element = elements[elementCount++];
                element.mesh = submesh;
                element.transform = sceneObject->transform;

                // Calculate depth 
                Box boundingBox = submesh->boundingBox;
                Vector3 boxCenter = boundingBox.min + ((boundingBox.max - boundingBox.min) * 0.5f);
                Vector3 distanceVector = boxCenter - sceneCamera->GetPosition();
                float distanceSq = DotProduct(distanceVector, distanceVector);

                element.depthKey.depth = (uint32_t)(distanceSq * 100);

                // Mesh hash
                element.depthKey.mesh = submesh->GetGeometryHash();
                element.sortKey.mesh = submesh->GetGeometryHash();

                // Material
                element.sortKey.material = submesh->material->GetMaterialHash();
                
                element.sortKey.pipeline = submesh->material->GetPipelineHash();
                element.depthKey.pipeline = submesh->material->GetPipelineHash();
            }
        }
    }

    void AddSubmesh(SubMesh* submesh, Transform& transform, Vector3 cameraPos) {
        RenderList::Element& element = elements[elementCount++];
        element.mesh = submesh;
        element.transform = transform;

        // Calculate depth 
        Box boundingBox = submesh->boundingBox;
        Vector3 boxCenter = boundingBox.min + ((boundingBox.max - boundingBox.min) * 0.5f);
        Vector3 distanceVector = boxCenter - cameraPos;
        float distanceSq = DotProduct(distanceVector, distanceVector);

        element.depthKey.depth = (uint32_t)(distanceSq * 100);

        // Mesh hash
        element.depthKey.mesh = submesh->GetGeometryHash();
        element.sortKey.mesh = submesh->GetGeometryHash();

        // Material
        element.sortKey.material = submesh->material->GetMaterialHash();

        element.sortKey.pipeline = submesh->material->GetPipelineHash();
        element.depthKey.pipeline = submesh->material->GetPipelineHash();
    }

    void ValidatePipelineStates(PGShaderLib* shaderLib, HWRendererAPI* rendererAPI) {
        for (size_t i = 0; i < elementCount; ++i) {
            RenderList::Element& element = elements[i];

            for (SceneRenderPassType passType = SceneRenderPassType::DEPTH_PASS; passType < SCENE_PASS_TYPE_COUNT; passType = SceneRenderPassType(passType + 1)) {
                PGPipelineDesc psoDesc = {};
                BuildPSODesc(passType, element, shaderLib, psoDesc);

                element.pipelineStates[passType] = PGRendererResources::CreateCachedPipelineState(rendererAPI, passType, psoDesc);
            }
        }
    }

    void SortByKey() {
        //PG_PROFILE_FUNCTION();
        std::sort(elements.begin(), elements.begin() + elementCount, [](RenderList::Element& left, RenderList::Element& right) {
            return left.sortKey.key < right.sortKey.key;
        });
    }

    void SortByReverseDepth() {
        //PG_PROFILE_FUNCTION();
        std::sort(elements.begin(), elements.begin() + elementCount, [](RenderList::Element& left, RenderList::Element& right) {
            return left.depthKey.key > right.depthKey.key;
        });
    }

    void SortByDepth() {
        //PG_PROFILE_FUNCTION();
        std::sort(elements.begin(), elements.begin() + elementCount, [](RenderList::Element& left, RenderList::Element& right) {
            return left.depthKey.key < right.depthKey.key;
        });
    }

    void Clear() {
        elementCount = 0;
    }

private:
    inline void BuildPSODesc(SceneRenderPassType passType, const RenderList::Element& element, PGShaderLib* shaderLib, PGPipelineDesc& outDesc) {
        Material* material = element.mesh->material;
        uint32_t shaderFlags = 0;

        outDesc.doubleSided = material->doubleSided;
        outDesc.shader = material->shader;
        outDesc.writeDepth = true;
        outDesc.depthFunction = COMPARISON_LESS;

        if (material->alphaMode == AlphaMode_ALPHA_TEST) {
            shaderFlags |= PGShaderFlags::ALPHA_TEST;
        }

        if (passType == SceneRenderPassType::DEPTH_PASS) {
            outDesc.shader = shaderLib->GetDefaultShader("DepthGen");
            if (material->alphaMode == AlphaMode_ALPHA_TEST) {
                outDesc.layoutType = InputLayoutType::POS_TC;
            } else {
                outDesc.layoutType = InputLayoutType::POS;
            }
        } else if (passType == SceneRenderPassType::FORWARD) {
            if (material->normalMappingEnabled) {
                outDesc.layoutType = InputLayoutType::POS_NOR_TC_TANGENT;
                shaderFlags |= PGShaderFlags::NORMAL_MAPPING;
            } else {
                outDesc.layoutType = InputLayoutType::POS_NOR_TC;
            }

            outDesc.writeDepth = false;
            outDesc.depthFunction = COMPARISON_EQUAL;
        }

        if (material->alphaMode == AlphaMode_BLEND_OVER_OP) {
            outDesc.blendEnable = true;
            outDesc.writeDepth = false;
            outDesc.depthFunction = COMPARISON_LESS;
        } else {
            outDesc.blendEnable = false;
        }

        outDesc.shaderFlags = shaderFlags;
    }
};

struct PGRenderView {
    RenderList* renderList;

    Matrix4 viewMatrix;
    Matrix4 projMatrix;
    Matrix4 projViewMatrix;
    Matrix4 inverseProjViewMatrix;

    Vector3 cameraPos;

    void UpdatePerViewConstantBuffer(HWRendererAPI* rendererAPI) {
        PerViewGlobalConstantBuffer perViewData = {};
        perViewData.g_ViewMatrix = viewMatrix;
        perViewData.g_ProjMatrix = projMatrix;
        perViewData.g_ProjViewMatrix = projViewMatrix;
        perViewData.g_InverseViewProjMatrix = inverseProjViewMatrix;
        perViewData.g_CameraPos = Vector4(cameraPos, 1.0f);

        void* data = rendererAPI->Map(PGRendererResources::s_PerViewGlobalConstantBuffer);
        memcpy(data, &perViewData, sizeof(PerViewGlobalConstantBuffer));
        rendererAPI->Unmap(PGRendererResources::s_PerViewGlobalConstantBuffer);
    }
};

void RenderScene(HWRendererAPI* rendererAPI, const RenderList* renderList, SceneRenderPassType scenePassType);
void RenderSceneDebug(HWRendererAPI* rendererAPI, const RenderList* renderList, HWBuffer* positionBuffer);
