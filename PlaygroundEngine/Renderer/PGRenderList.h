#pragma once

#include "../Core.h"
#include "../MeshUtils.h"
#include "../Scene.h"

#include "HWRendererAPI.h"

#include "PGRendererResources.h"

const size_t MAX_RENDER_LIST_ELEMENT_COUNT = 1024;

struct RenderList {
    struct Element {
        SubMesh* mesh;
        Transform transform;
        // the index of pipeline state in PGRendererResources::s_CachedPipelineStates
        uint8_t pipelineStates[SCENE_PASS_TYPE_COUNT] = {0};

        union DepthKey {
            struct {
                uint64_t depth : 16;
                uint64_t mesh  : 16;
                uint64_t pipeline : 32;
            };

            uint64_t key;
        } depthKey;

        union SortKey {
            struct {
                //from least significant to most significant in sort
                uint64_t mesh     : 32;
                uint64_t material : 32;
            };

            uint64_t key;
        } sortKey;

    };

    std::array<RenderList::Element, MAX_RENDER_LIST_ELEMENT_COUNT> elements;
    uint32_t elementCount = 0;

    void AddSceneObjects(const PGSceneObject* sceneObjects, size_t sceneObjectCount, PGCamera* sceneCamera) {
        for (size_t sceneObjectIndex = 0; sceneObjectIndex < sceneObjectCount; ++sceneObjectIndex) {
            const PGSceneObject* sceneObject = sceneObjects + sceneObjectIndex;

            for (SubMesh* submesh : sceneObject->mesh->submeshes) {
                RenderList::Element& element = elements[elementCount++];
                element.mesh = submesh;
                element.transform = sceneObject->transform;

                // Calculate depth 
                Vector3 distanceVector = element.transform.position - sceneCamera->GetPosition();
                float distanceSq = DotProduct(distanceVector, distanceVector);

                element.depthKey.depth = (uint32_t)(distanceSq);

                // Mesh hash
                element.depthKey.mesh = submesh->GetGeometryHash();
                element.sortKey.mesh = submesh->GetGeometryHash();

                // Material
                element.sortKey.material = submesh->material->GetMaterialPipelineCombinedHash();
                element.depthKey.pipeline = submesh->material->GetPipelineHash();
            }
        }
    }

    void ValidatePipelineStates(PGShaderLib* shaderLib, HWRendererAPI* rendererAPI) {
        for (size_t i = 0; i < elementCount; ++i) {
            RenderList::Element& element = elements[i];

            for (SceneRenderPassType passType = SceneRenderPassType::DEPTH_PASS; passType < SCENE_PASS_TYPE_COUNT; passType = SceneRenderPassType(passType + 1)) {
                PGPipelineDesc psoDesc = {};
                BuildPSODesc(passType, element, shaderLib, psoDesc);

                element.pipelineStates[passType] = PGRendererResources::CreatePipelineState(rendererAPI, passType, psoDesc);
            }
        }

    }

    void SortByKey() {
        std::sort(elements.begin(), elements.begin() + elementCount, [](RenderList::Element& left, RenderList::Element& right) {
            return left.sortKey.key < right.sortKey.key;
        });
    }

    void SortByDepth() {
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
        outDesc.doubleSided = material->doubleSided;
        outDesc.shader = material->shader;
        outDesc.layoutType = InputLayoutType::POS_NOR_TC;

        if (passType == SceneRenderPassType::DEPTH_PASS) {
            if (material->alphaMode == AlphaMode_ALWAYS_PASS) {
                outDesc.layoutType = InputLayoutType::POS;
                outDesc.shader = shaderLib->GetDefaultShader("ShadowGen");
            }
            else if (material->alphaMode == AlphaMode_ALPHA_TEST) {
                outDesc.layoutType = InputLayoutType::POS_TC;
                outDesc.shader = shaderLib->GetDefaultShader("ShadowGenAlphaTest");
            }
            else {
                PG_ASSERT(false, "Unsupported alpha mode");
            }
        }
    }

};

void RenderScene(HWRendererAPI* rendererAPI, const RenderList& renderList, SceneRenderPassType scenePassType);
