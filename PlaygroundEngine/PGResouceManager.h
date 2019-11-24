#pragma once

#include "Core.h"
#include "Renderer/PGTexture.h"

#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, void*> ResourcePool;

class PG_API PGResourceManager {
public:
    static void* CreateResource(const std::string& resourceFilePath);

private:
    PGResourceManager();

    static ResourcePool s_ResourcePool;
};

