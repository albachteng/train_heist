#include "../include/SFMLResourceManager.hpp"

namespace ECS {

SFMLResourceManager::SFMLResourceManager() : nextHandle(1) {
    // Start with handle 1 (0 reserved for INVALID_TEXTURE)
}

TextureHandle SFMLResourceManager::loadTexture(const std::string& filePath) {
    // Create new texture
    auto texture = std::make_unique<sf::Texture>();
    
    if (!texture->loadFromFile(filePath)) {
        // Failed to load texture
        return INVALID_TEXTURE;
    }
    
    // Generate handle and store texture
    TextureHandle handle = generateHandle();
    TextureEntry entry;
    entry.texture = std::move(texture);
    entry.filePath = filePath;
    
    textures[handle] = std::move(entry);
    return handle;
}

bool SFMLResourceManager::isTextureValid(TextureHandle handle) const {
    return handle != INVALID_TEXTURE && textures.find(handle) != textures.end();
}

std::string SFMLResourceManager::getTexturePath(TextureHandle handle) const {
    auto it = textures.find(handle);
    if (it != textures.end()) {
        return it->second.filePath;
    }
    return "";
}

bool SFMLResourceManager::unloadTexture(TextureHandle handle) {
    auto it = textures.find(handle);
    if (it != textures.end()) {
        textures.erase(it);
        return true;
    }
    return false;
}

size_t SFMLResourceManager::getLoadedTextureCount() const {
    return textures.size();
}

void SFMLResourceManager::clearAllTextures() {
    textures.clear();
}

const sf::Texture* SFMLResourceManager::getSFMLTexture(TextureHandle handle) const {
    auto it = textures.find(handle);
    if (it != textures.end()) {
        return it->second.texture.get();
    }
    return nullptr;
}

TextureHandle SFMLResourceManager::generateHandle() {
    return nextHandle++;
}

} // namespace ECS