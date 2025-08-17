#include "../include/MockResourceManager.hpp"
#include <algorithm>

namespace ECS {

MockResourceManager::MockResourceManager() 
    : nextHandle(1), nextLoadResult(INVALID_TEXTURE), nextUnloadResult(true), loadFailureMode(false) {
    // Constructor for test setup
}

TextureHandle MockResourceManager::loadTexture(const std::string& filePath) {
    methodCalls.push_back("loadTexture");
    
    // STUB: Will cause test failures in RED phase
    (void)filePath;
    return INVALID_TEXTURE;
}

bool MockResourceManager::isTextureValid(TextureHandle handle) const {
    // STUB: Will cause test failures in RED phase  
    (void)handle;
    return false;
}

std::string MockResourceManager::getTexturePath(TextureHandle handle) const {
    // STUB: Will cause test failures in RED phase
    (void)handle;
    return "";
}

bool MockResourceManager::unloadTexture(TextureHandle handle) {
    methodCalls.push_back("unloadTexture");
    
    // STUB: Will cause test failures in RED phase
    (void)handle;
    return false;
}

size_t MockResourceManager::getLoadedTextureCount() const {
    // STUB: Will cause test failures in RED phase
    return 0;
}

void MockResourceManager::clearAllTextures() {
    methodCalls.push_back("clearAllTextures");
    
    // STUB: Will cause test failures in RED phase - doesn't clear anything
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