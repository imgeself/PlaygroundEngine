#pragma once

#include "Mesh.h"
#include "Scene.h"
#include "Renderer/Material.h"

#include "Core.h"
#include "PGLog.h"

#include <unordered_map>
#include <memory>

#include "PGResouceManager.h"

PG_API void LoadMeshFromGLTFFile(HWRendererAPI* rendererAPI, PGScene* scene, Material* defaultMaterial, const std::string& filename);
