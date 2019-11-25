#include "PGResouceManager.h"

#include <string_view>

ResourcePool PGResourceManager::s_ResourcePool;

PGResourceManager::PGResourceManager() {

}

void PGResourceManager::ReleaseResourcePool() {
    for (auto& pair : s_ResourcePool) {
        delete pair.second;
    }

    s_ResourcePool.clear();
}


IResource* PGResourceManager::CreateResource(const std::string& resourceFilePath) {
    auto searchIndex = s_ResourcePool.find(resourceFilePath);
    if (searchIndex != s_ResourcePool.end()) {
        return searchIndex->second;
    }

    size_t extIndex = resourceFilePath.find_last_of('.') + 1;
    size_t extSize = resourceFilePath.size() - extIndex;
    std::string_view extension(resourceFilePath.c_str() + extIndex, extSize);

    if (!extension.compare("png")) {
        IResource* pResource = new PGTexture(resourceFilePath);
        s_ResourcePool[resourceFilePath] = pResource;
        return pResource;
    }

    return nullptr;
}

