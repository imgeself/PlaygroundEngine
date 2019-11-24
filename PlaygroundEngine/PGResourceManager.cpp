#include "PGResouceManager.h"

#include <string_view>

ResourcePool PGResourceManager::s_ResourcePool;

PGResourceManager::PGResourceManager() {

}

void* PGResourceManager::CreateResource(const std::string& resourceFilePath) {
    size_t extIndex = resourceFilePath.find_last_of('.') + 1;
    size_t extSize = resourceFilePath.size() - extIndex;
    std::string_view extension(resourceFilePath.c_str() + extIndex, extSize);

    if (!extension.compare("png")) {
        return (void*) new PGTexture(resourceFilePath);
    }
}

