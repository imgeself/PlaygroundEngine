#pragma once

#include "Mesh.h"
#include "Scene.h"
#include "Renderer/Material.h"

#include "Core.h"
#include "PGLog.h"

#include "PGResouceManager.h"

PG_API void LoadSceneFromGLTFFile(HWRendererAPI* rendererAPI, PGScene* scene, Material* defaultMaterial, const std::string& filename, bool leftHandedNormalMap);
PG_API void CreateQuad(HWRendererAPI* rendererAPI, PGSceneObject* outSceneObject, Material* defaultMaterial, Vector2 leftBottom, Vector2 rightTop, float zVal);
