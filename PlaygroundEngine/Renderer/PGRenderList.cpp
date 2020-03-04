#include "PGRenderList.h"
#include "../PGProfiler.h"

void RenderScene(HWRendererAPI* rendererAPI, const RenderList& renderList, SceneRenderPassType scenePassType) {
    PG_PROFILE_FUNCTION();
    // TODO: We need some sort of batching to take advantage of instanced drawing!
    uint64_t lastMeshIndex = 0;
    uint64_t lastMaterialIndex = 0;
    bool depthOnlyPass = scenePassType == SceneRenderPassType::SHADOW_GEN || scenePassType == SceneRenderPassType::PRE_DEPTH;

    std::hash<std::string> stringHasher;
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
        if (lastMeshIndex != (uint64_t) submesh) {
            bindGeometry = true;
        }
        if (lastMaterialIndex != (uint64_t)material && !depthOnlyPass) {
            bindMaterial = true;
        }

        if (bindMaterial) {
            PGTexture* albedoTexture = material->albedoTexture;
            PGTexture* roughnessTexture = material->roughnessTexture;
            PGTexture* metallicTexture = material->metallicTexture;
            PGTexture* aoTexture = material->aoTexture;
            PGTexture* metallicRoughnessTexture = material->metallicRoughnessTexture;
            PGTexture* radianceMap = material->radianceMap;
            PGTexture* irradianceMap = material->irradianceMap;
            PGTexture* envBrdf = material->envBrdf;
            HWShaderResourceView* textureResources[] = {
                albedoTexture ? albedoTexture->GetHWResourceView() : nullptr,
                roughnessTexture ? roughnessTexture->GetHWResourceView() : nullptr,
                metallicTexture ? metallicTexture->GetHWResourceView() : nullptr,
                aoTexture ? aoTexture->GetHWResourceView() : nullptr,
                metallicRoughnessTexture ? metallicRoughnessTexture->GetHWResourceView() : nullptr,
                radianceMap ? radianceMap->GetHWResourceView() : nullptr,
                irradianceMap ? irradianceMap->GetHWResourceView() : nullptr,
                envBrdf ? envBrdf->GetHWResourceView() : nullptr,
            };

            rendererAPI->SetShaderResourcesPS(ALBEDO_TEXTURE2D_SLOT, textureResources, ARRAYSIZE(textureResources));

            PGShader* shader = material->shader;
            rendererAPI->SetVertexShader(shader->GetHWVertexShader());
            rendererAPI->SetPixelShader(shader->GetHWPixelShader());
        }

        if (bindGeometry) {
            if (depthOnlyPass) {
                uint32_t vertexBufferStride = submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_POSITIONS];
                uint32_t offset = submesh->vertexOffsets[VertexBuffers::VERTEX_BUFFER_POSITIONS];
                rendererAPI->SetVertexBuffers(&submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_POSITIONS], 1, &vertexBufferStride, &offset);
                rendererAPI->SetIndexBuffer(submesh->indexBuffer, submesh->indexBufferStride, submesh->indexBufferOffset);
                rendererAPI->SetInputLayout(PGRendererResources::s_DefaultInputLayouts[InputLayoutType::POS]);
            }
            else {
                rendererAPI->SetVertexBuffers(submesh->vertexBuffers, VertexBuffers::VERTEX_BUFFER_COUNT, submesh->vertexStrides, submesh->vertexOffsets);
                rendererAPI->SetIndexBuffer(submesh->indexBuffer, submesh->indexBufferStride, submesh->indexBufferOffset);
                rendererAPI->SetInputLayout(PGRendererResources::s_DefaultInputLayouts[InputLayoutType::POS_NOR_TC]);
            }
        }

        rendererAPI->DrawIndexed(submesh->indexCount, submesh->indexStart, 0);

        lastMeshIndex = (uint64_t) submesh;
        lastMaterialIndex = (uint64_t)material;
    }
}

