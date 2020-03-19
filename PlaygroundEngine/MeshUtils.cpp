#include "MeshUtils.h"

#include "Utility/tiny_obj_loader.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "Utility/tiny_gltf.h"

static void LoadNode(const tinygltf::Model& model, const tinygltf::Node& node, Transform& parentTransform, 
                     const std::vector<MeshRef>& meshes, PGScene* outScene) {
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

    PGSceneObject sceneObject = { meshes[node.mesh], parentTransform };
    outScene->sceneObjects.push_back(sceneObject);

    for (int childrenNodeIndex : node.children) {
        const tinygltf::Node& children = model.nodes[childrenNodeIndex];
        LoadNode(model, children, parentTransform, meshes, outScene);
    }
}

void CreateTangentsGLTF(HWRendererAPI* rendererAPI, SubMesh* submesh, bool leftHandedNormalMap, const tinygltf::Model& model, const tinygltf::Primitive& prim, 
                        const tinygltf::BufferView& positionBufferView, const tinygltf::BufferView& normalBufferView, const tinygltf::BufferView& texCoordBufferView) {
    if (prim.indices >= 0) {
        const tinygltf::Accessor& indexBufferAccessor = model.accessors[prim.indices];
        const tinygltf::BufferView& indexBufferView = model.bufferViews[indexBufferAccessor.bufferView];
        const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

        const tinygltf::Buffer& positionBuffer = model.buffers[positionBufferView.buffer];
        const tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];
        const tinygltf::Buffer& texCoordBuffer = model.buffers[texCoordBufferView.buffer];

        const Vector3* positions = (Vector3*) (positionBuffer.data.data() + positionBufferView.byteOffset);
        const Vector3* normals = (Vector3*) (normalBuffer.data.data() + normalBufferView.byteOffset);
        const Vector2* texCoords = (Vector2*) (texCoordBuffer.data.data() + texCoordBufferView.byteOffset);

        uint32_t stride = indexBufferAccessor.ByteStride(indexBufferView);

        size_t vertexCount = positionBufferView.byteLength / sizeof(Vector3);
        size_t tangentBufferSize = sizeof(Vector4) * vertexCount;
        Vector4* tangents = (Vector4*) alloca(tangentBufferSize);

        for (size_t i = 0; i < indexBufferView.byteLength / stride; i += 3) {
            uint32_t i0;
            uint32_t i1;
            uint32_t i2;

            if (stride == 2) {
                const uint16_t* data = (uint16_t*)(indexBuffer.data.data() + indexBufferView.byteOffset);
                i0 = *(data + i);
                i1 = *(data + i + 1);
                i2 = *(data + i + 2);
            } else {
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

void LoadMeshFromGLTFFile(HWRendererAPI* rendererAPI, PGScene* scene, Material* defaultMaterial, const std::string& filename, bool leftHandedNormal) {
    tinygltf::TinyGLTF loader;
    std::string errors;
    std::string warnings;

    loader.SetImageLoader(tinygltf::LoadImageData, nullptr);

    std::filesystem::path filenamePath(filename);
    std::string directory = filenamePath.parent_path().string() + "/";

    tinygltf::Model model;
    bool success = loader.LoadASCIIFromFile(&model, &errors, &warnings, filename);
    if (!warnings.empty()) {
        PG_LOG_WARNING("%s Obj file load warnings: %s", filename.c_str(), warnings.c_str());
    }
    if (!errors.empty()) {
        PG_LOG_ERROR("%s Obj file load errors: %s", filename.c_str(), errors.c_str());
    }
    if (!success) {
        return;
    }

    std::vector<Material*> materials;
    for (tinygltf::Material mtl : model.materials) {
        Material* material = new Material;
        memset(material, 0, sizeof(Material));

        tinygltf::PbrMetallicRoughness pbr = mtl.pbrMetallicRoughness;

        material->diffuseColor = Vector4((float) pbr.baseColorFactor[0], (float) pbr.baseColorFactor[1], (float) pbr.baseColorFactor[2], (float) pbr.baseColorFactor[3]);
        material->roughness = (float) pbr.roughnessFactor;
        material->metallic = (float) pbr.metallicFactor;

        tinygltf::TextureInfo baseColor = pbr.baseColorTexture;
        if (baseColor.index >= 0) {
            tinygltf::Texture baseColorTexture = model.textures[baseColor.index];
            tinygltf::Image baseColorImage = model.images[baseColorTexture.source];
            material->hasAlbedoTexture = true;
            material->albedoTexture = (PGTexture*) PGResourceManager::CreateResource(directory + baseColorImage.uri);
        }

        tinygltf::TextureInfo metallicRoughness = pbr.metallicRoughnessTexture;
        if (metallicRoughness.index >= 0) {
            tinygltf::Texture metallicRoughnessTexture = model.textures[metallicRoughness.index];
            tinygltf::Image metallicRoughnessImage = model.images[metallicRoughnessTexture.source];
            material->hasMetallicRoughnessTexture = true;
            material->metallicRoughnessTexture = (PGTexture*) PGResourceManager::CreateResource(directory + metallicRoughnessImage.uri);
        }

        tinygltf::OcclusionTextureInfo occlusion = mtl.occlusionTexture;
        if (occlusion.index >= 0) {
            tinygltf::Texture occlusionTexture = model.textures[occlusion.index];
            tinygltf::Image occlusionImage = model.images[occlusionTexture.source];
            material->hasAOTexture = true;
            material->aoTexture = (PGTexture*) PGResourceManager::CreateResource(directory + occlusionImage.uri);
        }

        tinygltf::NormalTextureInfo normals = mtl.normalTexture;
        if (normals.index >= 0) {
            tinygltf::Texture normalTexture = model.textures[normals.index];
            tinygltf::Image normalImage = model.images[normalTexture.source];
            material->normalMappingEnabled = true;
            material->normalTexture = (PGTexture*) PGResourceManager::CreateResource(directory + normalImage.uri);
        }

        if (!mtl.alphaMode.compare("MASK")) {
            material->alphaMode = AlphaMode_ALPHA_TEST;
        } else {
            material->alphaMode = AlphaMode_ALWAYS_PASS;
        }

        material->doubleSided = mtl.doubleSided;

        material->radianceMap = defaultMaterial->radianceMap;
        material->irradianceMap = defaultMaterial->irradianceMap;
        material->envBrdf = defaultMaterial->envBrdf;
        material->shader = defaultMaterial->shader;
        materials.push_back(material);
    }

    // Create all buffers (vertex + index)
    std::vector<HWBuffer*> buffers;
    for (const tinygltf::BufferView& bufferView : model.bufferViews) {
        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        uint32_t bufferFlags = HWResourceFlags::USAGE_IMMUTABLE;
        switch (bufferView.target) {
            case 0x8892: //GL_ARRAY_BUFFER
            {
                bufferFlags |= HWResourceFlags::BIND_VERTEX_BUFFER;
            } break;
            case 0x8893: //GL_ELEMENT_ARRAY_BUFFER
            {
                bufferFlags |= HWResourceFlags::BIND_INDEX_BUFFER;
            } break;
            default:
            {
                PG_ASSERT(false, "Unsupported bufferView target");
            } break;
        }

        SubresourceData bufferSubresource = {};
        bufferSubresource.data = buffer.data.data() + bufferView.byteOffset;
        HWBuffer* hwBuffer = rendererAPI->CreateBuffer(&bufferSubresource, bufferView.byteLength, bufferFlags);

        buffers.push_back(hwBuffer);
    }

    std::vector<MeshRef> meshes;
    for (const tinygltf::Mesh& mesh : model.meshes) {
        MeshRef newMesh = new Mesh;
        newMesh->name = mesh.name;

        for (const tinygltf::Primitive& prim : mesh.primitives) {
            const tinygltf::Accessor& accessor = model.accessors[prim.indices];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            SubMesh* submesh = new SubMesh;
            memset(submesh, 0, sizeof(SubMesh));
            submesh->material = materials[prim.material];

            if (prim.indices >= 0) {
                const tinygltf::Accessor& indexBufferAccessor = model.accessors[prim.indices];
                const tinygltf::BufferView& indexBufferView = model.bufferViews[indexBufferAccessor.bufferView];

                submesh->indexBuffer = buffers[indexBufferAccessor.bufferView];
                submesh->indexBufferStride = (uint32_t) indexBufferAccessor.ByteStride(indexBufferView);
                submesh->indexStart = (uint32_t) indexBufferAccessor.byteOffset / indexBufferAccessor.ByteStride(indexBufferView);
                submesh->indexCount = (uint32_t) indexBufferAccessor.count;
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
                    PG_ASSERT(stride == 12, "Unsupported vertex stride");
                    submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_POSITIONS] = buffers[accessor.bufferView];
                    submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_POSITIONS] = stride;
                    submesh->vertexOffsets[VertexBuffers::VERTEX_BUFFER_POSITIONS] = (uint32_t) accessor.byteOffset;
                    positionBufferView = bufferView;
                } else if (!attrName.compare("NORMAL")) {
                    PG_ASSERT(stride == 12, "Unsupported vertex stride");
                    submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_NORMAL] = buffers[accessor.bufferView];
                    submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_NORMAL] = stride;
                    submesh->vertexOffsets[VertexBuffers::VERTEX_BUFFER_NORMAL] = (uint32_t) accessor.byteOffset;
                    normalBufferView = bufferView;
                } else if (!attrName.compare("TEXCOORD_0")) {
                    PG_ASSERT(stride == 8, "Unsupported vertex stride");
                    submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_TEXCOORD] = buffers[accessor.bufferView];
                    submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_TEXCOORD] = stride;
                    submesh->vertexOffsets[VertexBuffers::VERTEX_BUFFER_TEXCOORD] = (uint32_t) accessor.byteOffset;
                    texCoordBufferView = bufferView;
                } else if (!attrName.compare("TANGENT")) {
                    PG_ASSERT(stride == 16, "Unsupported vertex stride");
                    submesh->vertexBuffers[VertexBuffers::VERTEX_BUFFER_TANGENT] = buffers[accessor.bufferView];
                    submesh->vertexStrides[VertexBuffers::VERTEX_BUFFER_TANGENT] = stride;
                    submesh->vertexOffsets[VertexBuffers::VERTEX_BUFFER_TANGENT] = (uint32_t)accessor.byteOffset;
                    hasTangents = true;
                }
            }

            if (submesh->material->normalMappingEnabled && !hasTangents) {
                CreateTangentsGLTF(rendererAPI, submesh, leftHandedNormal, model, prim, positionBufferView, normalBufferView, texCoordBufferView);
            }

            newMesh->submeshes.push_back(submesh);
        }

        meshes.push_back(newMesh);
    }

    const tinygltf::Scene& gltfScene = model.scenes[model.defaultScene];
    for (int i : gltfScene.nodes) {
        Transform rootTransform;
        rootTransform.RotateYAxis(PI);
        const tinygltf::Node& node = model.nodes[i];
        LoadNode(model, node, rootTransform, meshes, scene);
    }
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

    mesh->submeshes.push_back(submesh);

    outSceneObject->mesh = mesh;
}


