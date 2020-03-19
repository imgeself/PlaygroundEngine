#include "PGRenderList.h"
#include "../PGProfiler.h"

void RenderScene(HWRendererAPI* rendererAPI, const RenderList& renderList, SceneRenderPassType scenePassType) {
    PG_PROFILE_FUNCTION();
    // TODO: We need some sort of batching to take advantage of instanced drawing!
    SubMesh* lastMesh = nullptr;
    Material* lastMaterial = nullptr;
    uint64_t lastPipelineIndex = ~0;
    bool depthOnlyPass = scenePassType == SceneRenderPassType::DEPTH_PASS;

    for (size_t elementIndex = 0; elementIndex < renderList.elementCount; ++elementIndex) {
        RenderList::Element element = renderList.elements[elementIndex];

        SubMesh* submesh = element.mesh;
        Material* material = element.mesh->material;

        PerDrawGlobalConstantBuffer perDrawGlobalConstantBuffer = {};
        perDrawGlobalConstantBuffer.g_ModelMatrix = element.transform.GetTransformMatrix();
        memcpy(&perDrawGlobalConstantBuffer.g_Material, element.mesh->material, sizeof(DrawMaterial));

        void* data = rendererAPI->Map(PGRendererResources::s_PerDrawGlobalConstantBuffer);
        memcpy(data, &perDrawGlobalConstantBuffer, sizeof(PerDrawGlobalConstantBuffer));
        rendererAPI->Unmap(PGRendererResources::s_PerDrawGlobalConstantBuffer);

        bool bindGeometry = false;
        bool bindMaterial = false;
        if (lastMesh != submesh) {
            bindGeometry = true;
        }

        if (lastMaterial != material) {
            if (depthOnlyPass) {
                bindMaterial = material->alphaMode != AlphaMode_ALWAYS_PASS;
            } else {
                bindMaterial = true;
            }
        }

        uint8_t pipelineIndex = element.pipelineStates[scenePassType];
        if (lastPipelineIndex != pipelineIndex) {
            HWPipelineState* pipelineState = PGRendererResources::s_CachedPipelineStates[scenePassType][pipelineIndex].pipelineState;
            rendererAPI->SetPipelineState(pipelineState);
        }

        if (bindMaterial) {
            PGTexture* albedoTexture = material->albedoTexture;
            PGTexture* roughnessTexture = material->roughnessTexture;
            PGTexture* metallicTexture = material->metallicTexture;
            PGTexture* aoTexture = material->aoTexture;
            PGTexture* metallicRoughnessTexture = material->metallicRoughnessTexture;
            PGTexture* normalTexture = material->normalTexture;
            PGTexture* radianceMap = material->radianceMap;
            PGTexture* irradianceMap = material->irradianceMap;
            PGTexture* envBrdf = material->envBrdf;
            HWShaderResourceView* textureResources[] = {
                albedoTexture ? albedoTexture->GetHWResourceView() : nullptr,
                roughnessTexture ? roughnessTexture->GetHWResourceView() : nullptr,
                metallicTexture ? metallicTexture->GetHWResourceView() : nullptr,
                aoTexture ? aoTexture->GetHWResourceView() : nullptr,
                metallicRoughnessTexture ? metallicRoughnessTexture->GetHWResourceView() : nullptr,
                normalTexture ? normalTexture->GetHWResourceView() : nullptr,
                radianceMap ? radianceMap->GetHWResourceView() : nullptr,
                irradianceMap ? irradianceMap->GetHWResourceView() : nullptr,
                envBrdf ? envBrdf->GetHWResourceView() : nullptr,
            };

            rendererAPI->SetShaderResourcesPS(ALBEDO_TEXTURE2D_SLOT, textureResources, ARRAYSIZE(textureResources));
        }

        if (bindGeometry) {
            rendererAPI->SetVertexBuffers(submesh->vertexBuffers, VertexBuffers::VERTEX_BUFFER_COUNT, submesh->vertexStrides, submesh->vertexOffsets);
            rendererAPI->SetIndexBuffer(submesh->indexBuffer, submesh->indexBufferStride, submesh->indexBufferOffset);
        }

        rendererAPI->DrawIndexed(submesh->indexCount, submesh->indexStart, 0);

        lastMesh = submesh;
        lastMaterial = material;
        lastPipelineIndex = pipelineIndex;
    }
}

