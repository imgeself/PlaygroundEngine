#pragma once

#include "../Core.h"
#include "../MeshUtils.h"
#include "../Scene.h"

#include "HWRendererAPI.h"

#include "PGRendererResources.h"

const size_t MAX_RENDER_LIST_ELEMENT_COUNT = 256;

struct RenderList {
    struct Element {
        MeshRef mesh;
        Transform transform;

        union DepthKey {
            struct {
                int32_t depth : 16;
                int32_t mesh  : 16;
            };

            uint32_t key;
        } depthKey;

        union SortKey {
            struct {
                //from least significant to most significant in sort
                uint64_t mesh     : 24;
                uint64_t depth    : 16;
                uint64_t material : 24;
            };

            uint64_t key;
        } sortKey;
    };

    std::array<RenderList::Element, MAX_RENDER_LIST_ELEMENT_COUNT> elements;
    uint32_t elementCount = 0;

    void AddSceneObjects(const PGSceneObject* sceneObjects, size_t sceneObjectCount, PGCamera* sceneCamera) {
        std::hash<std::string> stringHasher;
        for (size_t sceneObjectIndex = 0; sceneObjectIndex < sceneObjectCount; ++sceneObjectIndex) {
            const PGSceneObject* sceneObject = sceneObjects + sceneObjectIndex;

            RenderList::Element& element = elements[elementCount++];
            element.mesh = sceneObject->mesh;
            element.transform = sceneObject->transform;

            // Calculate depth 
            Vector3 distanceVector = element.transform.position - sceneCamera->GetPosition();
            float distance = sqrtf(DotProduct(distanceVector, distanceVector));

            element.depthKey.depth = (uint32_t) (distance * 10);
            element.sortKey.depth = (uint64_t) (distance * 10);

            // Mesh hash
            size_t meshNameHash = stringHasher(element.mesh->name);
            element.depthKey.mesh = (uint32_t) meshNameHash;
            element.sortKey.mesh = (uint64_t) meshNameHash;

            // Material
            element.sortKey.material = (uint64_t) element.mesh->material;
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
};

enum SceneRenderPassType {
    PRE_DEPTH,
    SHADOW_GEN,
    FORWARD,

    SCENE_PASS_TYPE_COUNT
};

void RenderScene(HWRendererAPI* rendererAPI, const RenderList& renderList, SceneRenderPassType scenePassType);
