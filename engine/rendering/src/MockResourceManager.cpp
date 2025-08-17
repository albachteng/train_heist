#include "../include/MockResourceManager.hpp"
#include <algorithm>

namespace ECS {

MockResourceManager::MockResourceManager() 
    : nextHandle(1), nextLoadResult(INVALID_TEXTURE), nextUnloadResult(true), loadFailureMode(false) {
    // Constructor for test setup
}

TextureHandle MockResourceManager::loadTexture(const std::string& filePath) {
    methodCalls.push_back("loadTexture");
    
    // Check if configured to fail
    if (loadFailureMode) {
        LoadTextureCall call{filePath, INVALID_TEXTURE};
        loadTextureCalls.push_back(call);
        return INVALID_TEXTURE;
    }
    
    // Use configured result if set
    TextureHandle handle = (nextLoadResult != INVALID_TEXTURE) ? nextLoadResult : nextHandle++;
    
    // Store the loaded texture
    loadedTextures[handle] = filePath;
    
    // Record the call
    LoadTextureCall call{filePath, handle};
    loadTextureCalls.push_back(call);
    
    // Reset configured result
    nextLoadResult = INVALID_TEXTURE;
    
    return handle;
}

bool MockResourceManager::isTextureValid(TextureHandle handle) const {
    return loadedTextures.find(handle) != loadedTextures.end();
}

std::string MockResourceManager::getTexturePath(TextureHandle handle) const {
    auto it = loadedTextures.find(handle);
    return (it != loadedTextures.end()) ? it->second : "";
}

bool MockResourceManager::unloadTexture(TextureHandle handle) {
    methodCalls.push_back("unloadTexture");
    
    // Check if texture exists
    auto it = loadedTextures.find(handle);
    bool exists = (it != loadedTextures.end());
    
    bool success = exists && nextUnloadResult;
    
    if (success) {
        loadedTextures.erase(it);
    }
    
    // Record the call
    UnloadTextureCall call{handle, success};
    unloadTextureCalls.push_back(call);
    
    // Reset configured result for next call
    nextUnloadResult = true;
    
    return success;
}

size_t MockResourceManager::getLoadedTextureCount() const {
    return loadedTextures.size();
}

void MockResourceManager::clearAllTextures() {
    methodCalls.push_back("clearAllTextures");
    
    loadedTextures.clear();
}

void MockResourceManager::reset() {
    loadTextureCalls.clear();
    unloadTextureCalls.clear();
    methodCalls.clear();
    loadedTextures.clear();
    nextHandle = 1;
    nextLoadResult = INVALID_TEXTURE;
    nextUnloadResult = true;
    loadFailureMode = false;
}

size_t MockResourceManager::getCallCount(const std::string& methodName) const {
    return std::count(methodCalls.begin(), methodCalls.end(), methodName);
}

bool MockResourceManager::wasMethodCalled(const std::string& methodName) const {
    return std::find(methodCalls.begin(), methodCalls.end(), methodName) != methodCalls.end();
}

void MockResourceManager::setNextLoadResult(TextureHandle handle) {
    nextLoadResult = handle;
}

void MockResourceManager::setNextUnloadResult(bool success) {
    nextUnloadResult = success;
}

void MockResourceManager::setLoadFailureMode(bool shouldFail) {
    loadFailureMode = shouldFail;
}

} // namespace ECS