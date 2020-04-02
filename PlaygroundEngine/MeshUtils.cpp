#include "MeshUtils.h"

#include "Utility/tiny_obj_loader.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "Utility/tiny_gltf.h"

#include "Platform/PGTime.h"

void CreateTangentsGLTF(HWRendererAPI* rendererAPI, SubMesh* submesh, bool leftHandedNormalMap, const tinygltf::Model& model, const tinygltf::Primitive& prim,
    const tinygltf::BufferView& positionBufferView, const tinygltf::BufferView& normalBufferView, const tinygltf::BufferView& texCoordBufferView) {
    if (prim.indices >= 0) {
        const tinygltf::Accessor& indexBufferAccessor = model.accessors[prim.indices];
        const tinygltf::BufferView& indexBufferView = model.bufferViews[indexBufferAccessor.bufferView];
        const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

        const tinygltf::Buffer& positionBuffer = model.buffers[positionBufferView.buffer];
        const tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];
        const tinygltf::Buffer& texCoordBuffer = model.buffers[texCoordBufferView.buffer];

        const Vector3* positions = (Vector3*)(positionBuffer.data.data() + positionBufferView.byteOffset);
        const Vector3* normals = (Vector3*)(normalBuffer.data.data() + normalBufferView.byteOffset);
        const Vector2* texCoords = (Vector2*)(texCoordBuffer.data.data() + texCoordBufferView.byteOffset);

        uint32_t stride = indexBufferAccessor.ByteStride(indexBufferView);

        size_t vertexCount = positionBufferView.byteLength / sizeof(Vector3);
        size_t tangentBufferSize = sizeof(Vector4) * vertexCount;
        Vector4* tangents = (Vector4*)alloca(tangentBufferSize);

        for (size_t i = 0; i < indexBufferView.byteLength / stride; i += 3) {
            uint32_t i0;
            uint32_t i1;
            uint32_t i2;

            if (stride == 2) {
                const uint16_t* data = (uint16_t*)(indexBuffer.data.data() + indexBufferView.byteOffset);
                i0 = *(data + i);
                i1 = *(data + i + 1);
                i2 = *(data + i + 2);
            }
            else {
                const uint32_t* data = (uint32_t*)(indexBuffer.data.data() + indexBufferView.byteOffset);
                i0 = *(data + i);
                i1 = *(data + i + 1);
                i2 = *(data + i + 2);
            }

            Vector3 p0 = *(positions + i0);
            Vector3 p1 = *(positions + i1);
            Vector3 p2 = *(positions + i2);

            Vector2 t0 = *(texCoords + i0);
            Vector2 t1 = *(texCoords + i1);
            Vector2 t2 = *(texCoords + i2);

            Vector3 e1 = p1 - p0;
            Vector3 e2 = p2 - p0;

            Vector2 d1 = t1 - t0;
            Vector2 d2 = t2 - t0;

            float f = 1 / (d1.x * d2.y - d2.x * d1.y);

            float tx = f * (d2.y * e1.x - d1.y * e2.x);
            float ty = f * (d2.y * e1.y - d1.y * e2.y);
            float tz = f * (d2.y * e1.z - d1.y * e2.z);

            Vector3 tangent(tx, ty, tz);
            Normalize(tangent);

            float bx = f * (-d2.x * e1.x + d1.x * e2.x);
            float by = f * (-d2.x * e1.y + d1.x * e2.y);
            float bz = f * (-d2.x * e1.z + d1.x * e2.z);

            Vector3 bitangent(bx, by, bz);
            Normalize(bitangent);

            Vector3 normal = *(normals + i0);

            float handedness = leftHandedNormalMap ? 1.0f : -1.0f;
            if (DotProduct(CrossProduct(normal, tangent), bitangent) < 0.0f) {
                handedness *= -1.0f;
            }

            *(tangents + i0) = Vector4(tangent, handedness);
            *(tangents + i1) = Vector4(tangent, handedness);
            *(tangents + i2) = Vector4(tangent, handedness);
        }

        SubresourceData subresource = {};
        subresource.data = tangents;
        HWBuffer* buffer = rendererAPI->CreateBuffer(&subresource, tangentBufferSize, HWResourceFlags::USAGE_IMMUTABLE | HWResourceFlags::BIND_VERTEX_BUFFER);
        submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_TANGENT] = buffer;
        submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_TANGENT] = 16;
        submesh->vertexOffsets[VertexBuffers::VERTEX_BUFFER_TANGENT] = 0;

    }
}

static void LoadNode(HWRendererAPI* rendererAPI, const tinygltf::Model& model, const tinygltf::Node& node, Transform& parentTransform, 
                     Material* materials, HWBuffer** buffers, PGScene* outScene, bool leftHandedNormalMap) {
    if (!node.scale.empty()) {
        parentTransform.Scale(Vector3((float) node.scale[0], (float) node.scale[1], (float) node.scale[2]));
    }
    if (!node.translation.empty()) {
        parentTransform.Translate(Vector3((float) node.translation[0], (float) node.translation[1], (float) node.translation[2]));
    }
    if (!node.rotation.empty()) {
        Matrix4 rotation;
        Vector4 q = Vector4((float) node.rotation[0], (float) node.rotation[1], (float) node.rotation[2], (float) node.rotation[3]);
        float w, x, y, z,
            xx, yy, zz,
            xy, yz, xz,
            wx, wy, wz, norm, s;

        norm = sqrtf(DotProduct(q, q));
        s = norm > 0.0f ? 2.0f / norm : 0.0f;

        x = q[0];
        y = q[1];
        z = q[2];
        w = q[3];

        xx = s * x * x;   xy = s * x * y;   wx = s * w * x;
        yy = s * y * y;   yz = s * y * z;   wy = s * w * y;
        zz = s * z * z;   xz = s * x * z;   wz = s * w * z;

        rotation[0][0] = 1.0f - yy - zz;
        rotation[1][1] = 1.0f - xx - zz;
        rotation[2][2] = 1.0f - xx - yy;

        rotation[0][1] = xy + wz;
        rotation[1][2] = yz + wx;
        rotation[2][0] = xz + wy;

        rotation[1][0] = xy - wz;
        rotation[2][1] = yz - wx;
        rotation[0][2] = xz - wy;

        rotation[0][3] = 0.0f;
        rotation[1][3] = 0.0f;
        rotation[2][3] = 0.0f;
        rotation[3][0] = 0.0f;
        rotation[3][1] = 0.0f;
        rotation[3][2] = 0.0f;
        rotation[3][3] = 1.0f;

        parentTransform.rotationMatrix = parentTransform.rotationMatrix * rotation;
    }

    if (node.mesh >= 0) {
        const tinygltf::Mesh& gltfMesh = model.meshes[node.mesh];
        MeshRef newMesh = new Mesh;
        newMesh->name = gltfMesh.name;

        for (const tinygltf::Primitive& prim : gltfMesh.primitives) {
            const tinygltf::Accessor& accessor = model.accessors[prim.indices];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            SubMesh* submesh = new SubMesh;
            memset(submesh, 0, sizeof(SubMesh));
            submesh->material = materials + prim.material;

            if (prim.indices >= 0) {
                const tinygltf::Accessor& indexBufferAccessor = model.accessors[prim.indices];
                const tinygltf::BufferView& indexBufferView = model.bufferViews[indexBufferAccessor.bufferView];

                submesh->indexBuffer = buffers[indexBufferAccessor.bufferView];
                submesh->indexBufferStride = (uint32_t)indexBufferAccessor.ByteStride(indexBufferView);
                submesh->indexStart = (uint32_t)indexBufferAccessor.byteOffset / indexBufferAccessor.ByteStride(indexBufferView);
                submesh->indexCount = (uint32_t)indexBufferAccessor.count;
            }

            bool hasTangents = false;
            tinygltf::BufferView positionBufferView;
            tinygltf::BufferView normalBufferView;
            tinygltf::BufferView texCoordBufferView;

            for (auto& attr : prim.attributes) {
                const std::string& attrName = attr.first;
                int attrData = attr.second;

                const tinygltf::Accessor& accessor = model.accessors[attrData];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];

                uint32_t stride = accessor.ByteStride(bufferView);

                if (!attrName.compare("POSITION")) {
                    PG_ASSERT(stride == 12, "Unsupported position stride");
                    submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_POSITIONS] = buffers[accessor.bufferView];
                    submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_POSITIONS] = stride;
                    submesh->vertexOffsets[VertexBuffers::VERTEX_BUFFER_POSITIONS] = (uint32_t)accessor.byteOffset;
                    positionBufferView = bufferView;

                    if (accessor.minValues.size() == 3 && accessor.maxValues.size() == 3) {
                        Vector3 minVector = Vector3((float)accessor.minValues[0], (float)accessor.minValues[1], (float)accessor.minValues[2]);
                        Vector3 maxVector = Vector3((float)accessor.maxValues[0], (float)accessor.maxValues[1], (float)accessor.maxValues[2]);

                        Vector4 transformedMin = parentTransform.GetTransformMatrix() * Vector4(minVector, 1.0f);
                        Vector4 transformedMax = parentTransform.GetTransformMatrix() * Vector4(maxVector, 1.0f);

                        Box meshBoundingBox;
                        meshBoundingBox.max = Vector3(std::max(transformedMax.x, transformedMin.x), std::max(transformedMax.y, transformedMin.y), std::max(transformedMax.z, transformedMin.z));
                        meshBoundingBox.min = Vector3(std::min(transformedMax.x, transformedMin.x), std::min(transformedMax.y, transformedMin.y), std::min(transformedMax.z, transformedMin.z));

                        submesh->boundingBox = meshBoundingBox;

                        if (meshBoundingBox.min.x < outScene->boundingBox.min.x) {
                            outScene->boundingBox.min.x = meshBoundingBox.min.x;
                        }
                        if (meshBoundingBox.min.y < outScene->boundingBox.min.y) {
                            outScene->boundingBox.min.y = meshBoundingBox.min.y;
                        }
                        if (meshBoundingBox.min.z < outScene->boundingBox.min.z) {
                            outScene->boundingBox.min.z = meshBoundingBox.min.z;
                        }

                        if (meshBoundingBox.max.x > outScene->boundingBox.max.x) {
                            outScene->boundingBox.max.x = meshBoundingBox.max.x;
                        }
                        if (meshBoundingBox.max.y > outScene->boundingBox.max.y) {
                            outScene->boundingBox.max.y = meshBoundingBox.max.y;
                        }
                        if (meshBoundingBox.max.z > outScene->boundingBox.max.z) {
                            outScene->boundingBox.max.z = meshBoundingBox.max.z;
                        }
                    }
                }
                else if (!attrName.compare("NORMAL")) {
                    PG_ASSERT(stride == 12, "Unsupported normal stride");
                    submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_NORMAL] = buffers[accessor.bufferView];
                    submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_NORMAL] = stride;
                    submesh->vertexOffsets[VertexBuffers::VERTEX_BUFFER_NORMAL] = (uint32_t)accessor.byteOffset;
                    normalBufferView = bufferView;
                }
                else if (!attrName.compare("TEXCOORD_0")) {
                    PG_ASSERT(stride == 8, "Unsupported texcoord stride");
                    submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_TEXCOORD] = buffers[accessor.bufferView];
                    submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_TEXCOORD] = stride;
                    submesh->vertexOffsets[VertexBuffers::VERTEX_BUFFER_TEXCOORD] = (uint32_t)accessor.byteOffset;
                    texCoordBufferView = bufferView;
                }
                else if (!attrName.compare("TANGENT")) {
                    PG_ASSERT(stride == 16, "Unsupported tangent stride");
                    submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_TANGENT] = buffers[accessor.bufferView];
                    submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_TANGENT] = stride;
                    submesh->vertexOffsets[VertexBuffers::VERTEX_BUFFER_TANGENT] = (uint32_t)accessor.byteOffset;
                    hasTangents = true;
                }
            }

            if (submesh->material->normalMappingEnabled && !hasTangents) {
                CreateTangentsGLTF(rendererAPI, submesh, leftHandedNormalMap, model, prim, positionBufferView, normalBufferView, texCoordBufferView);
            }

            newMesh->submeshes.push_back(submesh);
        }

        PGSceneObject sceneObject = { newMesh, parentTransform };
        outScene->sceneObjects.push_back(sceneObject);
    }

    for (int childrenNodeIndex : node.children) {
        const tinygltf::Node& children = model.nodes[childrenNodeIndex];
        LoadNode(rendererAPI, model, children, parentTransform, materials, buffers, outScene, leftHandedNormalMap);
    }
}

PGTexture* LoadGLTFImage(const tinygltf::Image& gltfImage, bool generateMips = true) {
    // Subresources
    SubresourceData subresource;
    subresource.data = gltfImage.image.data();
    subresource.memPitch = gltfImage.width * (gltfImage.component * (gltfImage.bits / 8));
    subresource.memSlicePitch = 0;

    // TODO: Determine the format using image's pixel informations!
    Texture2DDesc initParams = {};
    initParams.arraySize = 1;
    initParams.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    initParams.width = gltfImage.width;
    initParams.height = gltfImage.height;
    initParams.sampleCount = 1;
    initParams.mipCount = generateMips ? 0 : 1;
    initParams.flags = HWResourceFlags::BIND_SHADER_RESOURCE;
    if (generateMips) {
        initParams.flags |= (HWResourceFlags::MISC_GENERATE_MIPS | HWResourceFlags::BIND_RENDER_TARGET);
    } else {
        initParams.flags |= HWResourceFlags::USAGE_IMMUTABLE;
    }

    PGTexture* texture = new PGTexture(&initParams, &subresource, generateMips);
    return texture;
}

void LoadSceneFromGLTFFile(HWRendererAPI* rendererAPI, PGScene* scene, Material* defaultMaterial, const std::string& filename, bool leftHandedNormalMap) {
    tinygltf::TinyGLTF loader;
    std::string errors;
    std::string warnings;

    uint64_t startTimeMillis = PGTime::GetTimeMilliseconds();

    loader.SetImageLoader(tinygltf::LoadImageData, nullptr);

    std::filesystem::path filenamePath(filename);
    std::string directory = filenamePath.parent_path().string() + "/";

    PG_LOG_DEBUG("Loading scene: %s", filenamePath.filename().string().c_str());

    tinygltf::Model model;
    bool success = false;
    if (filenamePath.extension().string().compare(".gltf") == 0) {
        success = loader.LoadASCIIFromFile(&model, &errors, &warnings, filename);
    } else {
        success = loader.LoadBinaryFromFile(&model, &errors, &warnings, filename);
    }

    if (!warnings.empty()) {
        PG_LOG_WARNING("%s scene load warnings: %s", filename.c_str(), warnings.c_str());
    }
    if (!errors.empty()) {
        PG_LOG_ERROR("%s scene load errors: %s", filename.c_str(), errors.c_str());
    }
    if (!success) {
        return;
    }

    size_t numGltfMaterials = model.materials.size();
    Material* materials = new Material[numGltfMaterials];
    memset(materials, 0, sizeof(Material) * numGltfMaterials);
    for (size_t materialIndex = 0; materialIndex < numGltfMaterials; ++materialIndex) {
        const tinygltf::Material& gltfMaterial = model.materials[materialIndex];
        Material* material = materials + materialIndex;

        tinygltf::PbrMetallicRoughness pbr = gltfMaterial.pbrMetallicRoughness;

        material->diffuseColor = Vector4((float) pbr.baseColorFactor[0], (float) pbr.baseColorFactor[1], (float) pbr.baseColorFactor[2], (float) pbr.baseColorFactor[3]);
        material->emissiveColor = Vector4((float) gltfMaterial.emissiveFactor[0], (float) gltfMaterial.emissiveFactor[1], (float) gltfMaterial.emissiveFactor[2], 1.0f);
        material->roughness = (float) pbr.roughnessFactor;
        material->metallic = (float) pbr.metallicFactor;

        tinygltf::TextureInfo baseColor = pbr.baseColorTexture;
        if (baseColor.index >= 0) {
            const tinygltf::Texture& baseColorTexture = model.textures[baseColor.index];
            const tinygltf::Image& baseColorImage = model.images[baseColorTexture.source];
            material->hasAlbedoTexture = true;
            material->albedoTexture = LoadGLTFImage(baseColorImage);
        }

        tinygltf::TextureInfo metallicRoughness = pbr.metallicRoughnessTexture;
        if (metallicRoughness.index >= 0) {
            const tinygltf::Texture& metallicRoughnessTexture = model.textures[metallicRoughness.index];
            const tinygltf::Image& metallicRoughnessImage = model.images[metallicRoughnessTexture.source];
            material->hasMetallicRoughnessTexture = true;
            material->metallicRoughnessTexture = LoadGLTFImage(metallicRoughnessImage);
        }

        tinygltf::TextureInfo emissive = gltfMaterial.emissiveTexture;
        if (emissive.index >= 0) {
            const tinygltf::Texture& emissiveTexture = model.textures[emissive.index];
            const tinygltf::Image& emissiveImage = model.images[emissiveTexture.source];
            material->hasEmissiveTexture = true;
            material->emmisiveTexture = LoadGLTFImage(emissiveImage);
        }

        tinygltf::OcclusionTextureInfo occlusion = gltfMaterial.occlusionTexture;
        if (occlusion.index >= 0) {
            const tinygltf::Texture& occlusionTexture = model.textures[occlusion.index];
            const tinygltf::Image& occlusionImage = model.images[occlusionTexture.source];
            material->hasAOTexture = true;
            material->aoTexture = LoadGLTFImage(occlusionImage);
        }

        tinygltf::NormalTextureInfo normals = gltfMaterial.normalTexture;
        if (normals.index >= 0) {
            const tinygltf::Texture& normalTexture = model.textures[normals.index];
            const tinygltf::Image& normalImage = model.images[normalTexture.source];
            material->normalMappingEnabled = true;
            material->normalTexture = LoadGLTFImage(normalImage);
        }

        if (!gltfMaterial.alphaMode.compare("MASK")) {
            material->alphaMode = AlphaMode_ALPHA_TEST;
        } else {
            material->alphaMode = AlphaMode_ALWAYS_PASS;
        }

        material->doubleSided = gltfMaterial.doubleSided;

        material->radianceMap = defaultMaterial->radianceMap;
        material->irradianceMap = defaultMaterial->irradianceMap;
        material->envBrdf = defaultMaterial->envBrdf;
        material->shader = defaultMaterial->shader;
    }

    // Create all buffers (vertex + index)
    size_t numBuffers = model.bufferViews.size();
    HWBuffer** buffers = (HWBuffer**) alloca(sizeof(HWBuffer*) * numBuffers);
    for (size_t bufferIndex = 0; bufferIndex < numBuffers; ++bufferIndex) {
        const tinygltf::BufferView& gltfBufferView = model.bufferViews[bufferIndex];
        const tinygltf::Buffer& gltfBuffer = model.buffers[gltfBufferView.buffer];

        uint32_t bufferFlags = HWResourceFlags::USAGE_IMMUTABLE;
        if (gltfBufferView.target == 0) {
            continue;
        }
        switch (gltfBufferView.target) {
            case TINYGLTF_TARGET_ARRAY_BUFFER:
            {
                bufferFlags |= HWResourceFlags::BIND_VERTEX_BUFFER;
            } break;
            case TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER:
            {
                bufferFlags |= HWResourceFlags::BIND_INDEX_BUFFER;
            } break;
            default:
            {
                PG_ASSERT(false, "Unsupported bufferView target");
            } break;
        }

        SubresourceData bufferSubresource = {};
        bufferSubresource.data = gltfBuffer.data.data() + gltfBufferView.byteOffset;
        HWBuffer* hwBuffer = rendererAPI->CreateBuffer(&bufferSubresource, gltfBufferView.byteLength, bufferFlags);

        *(buffers + bufferIndex) = hwBuffer;
    }

    const tinygltf::Scene& gltfScene = model.scenes[model.defaultScene];
    for (int i : gltfScene.nodes) {
        Transform rootTransform;
        rootTransform.RotateYAxis(PI);
        const tinygltf::Node& node = model.nodes[i];
        LoadNode(rendererAPI, model, node, rootTransform, materials, buffers, scene, leftHandedNormalMap);
    }

    uint64_t endTimeMillis = PGTime::GetTimeMilliseconds();
    uint64_t elapsedTime = endTimeMillis - startTimeMillis;

    PG_LOG_DEBUG("Scene loaded in %d milliseconds", elapsedTime);

}

void CreateQuad(HWRendererAPI* rendererAPI, PGSceneObject* outSceneObject, Material* defaultMaterial, Vector2 leftBottom, Vector2 rightTop, float zVal) {
    Mesh* mesh = new Mesh;
    mesh->name = "Quad";

    SubMesh* submesh = new SubMesh;
    submesh->material = defaultMaterial;

    const Vector3 positions[] = {
        { leftBottom.x, leftBottom.y, zVal },
        { leftBottom.x, rightTop.y, zVal},
        { rightTop.x, rightTop.y, zVal },
        { rightTop.x, leftBottom.y, zVal }
    };

    const Vector3 normals[] = {
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f }
    };

    const Vector2 texCoords[] = {
        { 0.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
    };

    const Vector4 tangents[] = {
        { 1.0f, 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f, 1.0f }
    };

    const uint16_t indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    SubresourceData bufferSubresource = {};
    bufferSubresource.data = positions;
    submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_POSITIONS] = rendererAPI->CreateBuffer(&bufferSubresource, sizeof(positions), HWResourceFlags::USAGE_IMMUTABLE | HWResourceFlags::BIND_VERTEX_BUFFER);
    submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_POSITIONS] = sizeof(positions[0]);

    bufferSubresource.data = normals;
    submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_NORMAL] = rendererAPI->CreateBuffer(&bufferSubresource, sizeof(normals), HWResourceFlags::USAGE_IMMUTABLE | HWResourceFlags::BIND_VERTEX_BUFFER);
    submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_NORMAL] = sizeof(normals[0]);

    bufferSubresource.data = texCoords;
    submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_TEXCOORD] = rendererAPI->CreateBuffer(&bufferSubresource, sizeof(texCoords), HWResourceFlags::USAGE_IMMUTABLE | HWResourceFlags::BIND_VERTEX_BUFFER);
    submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_TEXCOORD] = sizeof(texCoords[0]);

    bufferSubresource.data = tangents;
    submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_TANGENT] = rendererAPI->CreateBuffer(&bufferSubresource, sizeof(tangents), HWResourceFlags::USAGE_IMMUTABLE | HWResourceFlags::BIND_VERTEX_BUFFER);
    submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_TANGENT] = sizeof(tangents[0]);

    bufferSubresource.data = indices;
    submesh->indexBuffer = rendererAPI->CreateBuffer(&bufferSubresource, sizeof(indices), HWResourceFlags::USAGE_IMMUTABLE | HWResourceFlags::BIND_INDEX_BUFFER);
    submesh->indexBufferStride = sizeof(indices[0]);

    submesh->indexCount = ARRAYSIZE(indices);
    submesh->indexStart = 0;

    submesh->boundingBox.min = Vector3(leftBottom.x, leftBottom.y, zVal);
    submesh->boundingBox.max = Vector3(rightTop.x, rightTop.y, zVal);

    mesh->submeshes.push_back(submesh);

    outSceneObject->mesh = mesh;
}


