#pragma once

#include <string>

namespace ECS {

/**
 * Resource handle type for textures
 * Simple integer ID for now, can be evolved to opaque handle later
 */
using TextureHandle = int;

/**
 * Special values for TextureHandle
 */
constexpr TextureHandle INVALID_TEXTURE = -1;
constexpr TextureHandle DEFAULT_TEXTURE = 0;

/**
 * IResourceManager - Abstract interface for resource loading and management
 * 
 * Handles loading textures from files and provides handles for rendering.
 * Designed for simple initial implementation with future extensibility.
 * 
 * Key Features:
 * - Simple texture loading by file path
 * - Integer-based handles for immediate use
 * - Error handling with invalid handle returns
 * - Clean separation from rendering logic
 * 
 * Future Evolution:
 * - Async loading support
 * - Reference counting and automatic cleanup
 * - Multiple resource types (fonts, shaders, sounds)
 * - Memory management and streaming
 */
class IResourceManager {
public:
    virtual ~IResourceManager() = default;
    
    /**
     * Load a texture from file path
     * @param filePath Path to the texture file
     * @return TextureHandle for the loaded texture, or INVALID_TEXTURE on failure
     */
    virtual TextureHandle loadTexture(const std::string& filePath) = 0;
    
    /**
     * Check if a texture handle is valid
     * @param handle The texture handle to check
     * @return true if the handle refers to a loaded texture
     */
    virtual bool isTextureValid(TextureHandle handle) const = 0;
    
    /**
     * Get the file path associated with a texture handle
     * @param handle The texture handle
     * @return File path string, or empty string if handle is invalid
     */
    virtual std::string getTexturePath(TextureHandle handle) const = 0;
    
    /**
     * Unload a texture and free its resources
     * @param handle The texture handle to unload
     * @return true if texture was successfully unloaded
     */
    virtual bool unloadTexture(TextureHandle handle) = 0;
    
    /**
     * Get count of currently loaded textures
     * @return Number of loaded textures
     */
    virtual size_t getLoadedTextureCount() const = 0;
    
    /**
     * Clear all loaded textures and free resources
     */
    virtual void clearAllTextures() = 0;
};

} // namespace ECS