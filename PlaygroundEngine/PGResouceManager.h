#pragma once

#include "Core.h"
#include "Renderer/PGTexture.h"

#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, IResource*> ResourcePool;

class PG_API PGResourceManager {
public:
    static void ReleaseResourcePool();
    static IResource* CreateResource(const std::string& resourceFilePath);

private:
    PGResourceManager();

    static ResourcePool s_ResourcePool;
};

