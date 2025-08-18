#pragma once

#include "IResourceManager.hpp"
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory>

namespace ECS {

/**
 * SFMLResourceManager - Real SFML implementation of IResourceManager
 * 
 * Manages actual SFML textures with handle-based access.
 * Provides texture loading, validation, and cleanup for SFMLRenderer.
 * 
 * Features:
 * - Real sf::Texture loading from file paths
 * - Handle-based texture access for SFMLRenderer
 * - Automatic texture cleanup and memory management
 * - Error handling for invalid file paths
 */
class SFMLResourceManager : public IResourceManager {
public:
    SFMLResourceManager();
    ~SFMLResourceManager() override = default;

    // IResourceManager interface implementation
    TextureHandle loadTexture(const std::string& filePath) override;
    bool isTextureValid(TextureHandle handle) const override;
    std::string getTexturePath(TextureHandle handle) const override;
    bool unloadTexture(TextureHandle handle) override;
    size_t getLoadedTextureCount() const override;
    void clearAllTextures() override;

    /**
     * Get actual SFML texture for rendering (used by SFMLRenderer)
     * @param handle Texture handle from loadTexture()
     * @return Pointer to sf::Texture, or nullptr if invalid handle
     */
    const sf::Texture* getSFMLTexture(TextureHandle handle) const;

private:
    struct TextureEntry {
        std::unique_ptr<sf::Texture> texture;
        std::string filePath;
    };
    
    std::unordered_map<TextureHandle, TextureEntry> textures;
    TextureHandle nextHandle;
    
    /**
     * Generate next available texture handle
     * @return Unique texture handle
     */
    TextureHandle generateHandle();
};

} // namespace ECS