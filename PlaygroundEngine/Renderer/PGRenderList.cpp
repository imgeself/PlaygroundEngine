#include "PGRenderList.h"
#include "../PGProfiler.h"

void RenderScene(HWRendererAPI* rendererAPI, const RenderList* renderList, SceneRenderPassType scenePassType) {
    PG_PROFILE_FUNCTION();
    // TODO: We need some sort of batching to take advantage of instanced drawing!
    SubMesh* lastMesh = nullptr;
    Material* lastMaterial = nullptr;
    uint64_t lastPipelineIndex = ~0;
    bool depthOnlyPass = scenePassType == SceneRenderPassType::DEPTH_PASS;

    for (size_t elementIndex = 0; elementIndex < renderList->elementCount; ++elementIndex) {
        RenderList::Element element = renderList->elements[elementIndex];

        SubMesh* submesh = element.mesh;
        Material* material = element.mesh->material;

        PerDrawGlobalConstantBuffer perDrawGlobalConstantBuffer = {};
        perDrawGlobalConstantBuffer.g_ModelMatrix = element.transform.GetTransformMatrix();

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
            PGTexture* emissiveTexture = material->emmisiveTexture;
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
                emissiveTexture ? emissiveTexture->GetHWResourceView() : nullptr,
                radianceMap ? radianceMap->GetHWResourceView() : nullptr,
                irradianceMap ? irradianceMap->GetHWResourceView() : nullptr,
                envBrdf ? envBrdf->GetHWResourceView() : nullptr,
            };

            rendererAPI->SetShaderResourcesPS(ALBEDO_TEXTURE2D_SLOT, textureResources, ARRAYSIZE(textureResources));

            PerMaterialGlobalConstantBuffer perMaterialGlobalConstantBuffer = {};
            memcpy(&perMaterialGlobalConstantBuffer.g_Material, element.mesh->material, sizeof(DrawMaterial));

            void* materialData = rendererAPI->Map(PGRendererResources::s_PerMaterialGlobalConstantBuffer);
            memcpy(materialData, &perMaterialGlobalConstantBuffer, sizeof(PerMaterialGlobalConstantBuffer));
            rendererAPI->Unmap(PGRendererResources::s_PerMaterialGlobalConstantBuffer);
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

void RenderSceneDebug(HWRendererAPI* rendererAPI, const RenderList* renderList, HWBuffer* positionBuffer) {
    PG_PROFILE_FUNCTION();

    Vector3 positionData[24];
    for (size_t elementIndex = 0; elementIndex < renderList->elementCount; ++elementIndex) {
        RenderList::Element element = renderList->elements[elementIndex];

        SubMesh* submesh = element.mesh;
        Material* material = element.mesh->material;

        Box boundingBox = element.mesh->boundingBox;

        const float x0 = boundingBox.min.x;
        const float y0 = boundingBox.min.y;
        const float z0 = boundingBox.min.z;
        const float x1 = boundingBox.max.x;
        const float y1 = boundingBox.max.y;
        const float z1 = boundingBox.max.z;

        positionData[0] = Vector3(x0, y0, z0);
        positionData[1] = Vector3(x0, y1, z0);
        positionData[2] = Vector3(x0, y1, z0);
        positionData[3] = Vector3(x1, y1, z0);
        positionData[4] = Vector3(x1, y1, z0);
        positionData[5] = Vector3(x1, y0, z0);
        positionData[6] = Vector3(x1, y0, z0);
        positionData[7] = Vector3(x0, y0, z0);
        positionData[8] = Vector3(x0, y0, z0);
        positionData[9] = Vector3(x0, y0, z1);
        positionData[10] = Vector3(x0, y1, z0);
        positionData[11] = Vector3(x0, y1, z1);
        positionData[12] = Vector3(x1, y1, z0);
        positionData[13] = Vector3(x1, y1, z1);
        positionData[14] = Vector3(x1, y0, z0);
        positionData[15] = Vector3(x1, y0, z1);
        positionData[16] = Vector3(x0, y0, z1);
        positionData[17] = Vector3(x0, y1, z1);
        positionData[18] = Vector3(x0, y1, z1);
        positionData[19] = Vector3(x1, y1, z1);
        positionData[20] = Vector3(x1, y1, z1);
        positionData[21] = Vector3(x1, y0, z1);
        positionData[22] = Vector3(x1, y0, z1);
        positionData[23] = Vector3(x0, y0, z1);


        void* positionBufferData = rendererAPI->Map(positionBuffer);
        memcpy(positionBufferData, positionData, sizeof(positionData));
        rendererAPI->Unmap(positionBuffer);

        rendererAPI->Draw(ARRAYSIZE(positionData), 0);

    }
}

