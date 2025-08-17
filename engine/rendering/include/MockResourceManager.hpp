#pragma once

#include "IResourceManager.hpp"
#include <vector>
#include <string>
#include <map>

namespace ECS {

/**
 * MockResourceManager - Test implementation of IResourceManager interface
 * 
 * Records all resource management calls for verification in tests.
 * Simulates resource loading/unloading without requiring actual files.
 * 
 * Features:
 * - Records all method calls for test verification
 * - Simulates texture loading with fake handles
 * - Configurable success/failure behavior for testing error cases
 * - Full texture lifecycle management (load, validate, unload)
 */
class MockResourceManager : public IResourceManager {
public:
    MockResourceManager();
    
    // Recorded calls for test verification
    struct LoadTextureCall {
        std::string filePath;
        TextureHandle returnedHandle;
    };
    
    struct UnloadTextureCall {
        TextureHandle handle;
        bool returnedSuccess;
    };
    
    std::vector<LoadTextureCall> loadTextureCalls;
    std::vector<UnloadTextureCall> unloadTextureCalls;
    std::vector<std::string> methodCalls;
    
    // IResourceManager interface (will cause test failures in RED phase)
    TextureHandle loadTexture(const std::string& filePath) override;
    bool isTextureValid(TextureHandle handle) const override;
    std::string getTexturePath(TextureHandle handle) const override;
    bool unloadTexture(TextureHandle handle) override;
    size_t getLoadedTextureCount() const override;
    void clearAllTextures() override;
    
    // Test utility methods
    void reset();
    size_t getCallCount(const std::string& methodName) const;
    bool wasMethodCalled(const std::string& methodName) const;
    
    // Test configuration methods
    void setNextLoadResult(TextureHandle handle);
    void setNextUnloadResult(bool success);
    void setLoadFailureMode(bool shouldFail);
    
private:
    // Mock state
    std::map<TextureHandle, std::string> loadedTextures;
    TextureHandle nextHandle;
    
    // Test configuration
    TextureHandle nextLoadResult;
    bool nextUnloadResult;
    bool loadFailureMode;
};

} // namespace ECS