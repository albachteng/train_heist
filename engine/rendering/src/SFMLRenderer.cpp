#include "../include/SFMLRenderer.hpp"
#include "../include/SFMLResourceManager.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdint>

namespace ECS {

SFMLRenderer::SFMLRenderer(IResourceManager* resourceManager, IWindowManager* windowManager)
    : resourceManager(resourceManager), windowManager(windowManager), currentRenderTarget(nullptr),
      frameActive(false), currentFrameSpriteCount(0), currentFrameRectCount(0) {
    
    // Initialize SFML objects
    // Note: For testing purposes, we'll create minimal SFML objects
    // In a real implementation, these would be fully functional SFML objects
    rectangle = nullptr; // Will be created when needed
}

SFMLRenderer::~SFMLRenderer() {
    // SFML objects will be automatically cleaned up by unique_ptr
}

void SFMLRenderer::beginFrame() {
    // Set frame as active
    frameActive = true;
    
    // Reset render counts for new frame
    currentFrameSpriteCount = 0;
    currentFrameRectCount = 0;
    
    // Update render target from window manager
    updateRenderTarget();
}

void SFMLRenderer::endFrame() {
    // Present frame if we have a render target
    if (currentRenderTarget) {
        // SFML automatically handles presentation through window manager
        windowManager->display();
    }
    
    // Clear frame active state
    frameActive = false;
}

void SFMLRenderer::clear() {
    // Get window directly (like working tests)
    if (!windowManager) {
        return;
    }
    
    void* nativeTarget = windowManager->getNativeRenderTarget();
    if (!nativeTarget) {
        return;
    }
    
    sf::RenderWindow* window = static_cast<sf::RenderWindow*>(nativeTarget);
    if (!window || !window->isOpen()) {
        return;
    }
    
    // Clear directly (like working tests)
    window->clear(sf::Color::Black);
}

void SFMLRenderer::renderSprite(float x, float y, float z, float width, float height, int textureHandle) {
    // Increment sprite count
    currentFrameSpriteCount++;
    
    // Skip rendering if no render target
    if (!currentRenderTarget) {
        return;
    }
    
    // Get texture from resource manager
    const sf::Texture* texture = getTexture(textureHandle);
    if (!texture) {
        return; // Still count render attempt but skip actual rendering
    }
    
    // Create and configure sprite for real SFML rendering
    sf::Sprite sprite(*texture);
    sprite.setPosition(sf::Vector2f(x, y));
    
    // Set sprite scale to achieve desired width/height
    sf::Vector2u textureSize = texture->getSize();
    if (textureSize.x > 0 && textureSize.y > 0) {
        sprite.setScale(sf::Vector2f(width / textureSize.x, height / textureSize.y));
    }
    
    // Render sprite (z-coordinate ignored for now, could be used for depth sorting later)
    (void)z;
    currentRenderTarget->draw(sprite);
}

void SFMLRenderer::renderRect(float x, float y, float width, float height, float r, float g, float b, float a) {
    // Increment rectangle count
    currentFrameRectCount++;
    
    // Get window directly from window manager (like working tests)
    if (!windowManager) {
        return;
    }
    
    void* nativeTarget = windowManager->getNativeRenderTarget();
    if (!nativeTarget) {
        return;
    }
    
    // Cast to RenderWindow directly (safer than RenderTarget)
    sf::RenderWindow* window = static_cast<sf::RenderWindow*>(nativeTarget);
    if (!window || !window->isOpen()) {
        return;
    }
    
    // Create and configure rectangle (exactly like working tests)
    sf::RectangleShape rectangle(sf::Vector2f(width, height));
    rectangle.setPosition(sf::Vector2f(x, y));
    
    // Convert color from 0.0-1.0 to 0-255 range for SFML
    std::uint8_t red = static_cast<std::uint8_t>(std::max(0.0f, std::min(255.0f, r * 255.0f)));
    std::uint8_t green = static_cast<std::uint8_t>(std::max(0.0f, std::min(255.0f, g * 255.0f)));
    std::uint8_t blue = static_cast<std::uint8_t>(std::max(0.0f, std::min(255.0f, b * 255.0f)));
    std::uint8_t alpha = static_cast<std::uint8_t>(std::max(0.0f, std::min(255.0f, a * 255.0f)));
    
    rectangle.setFillColor(sf::Color(red, green, blue, alpha));
    
    // Draw directly to window (like working tests)
    window->draw(rectangle);
}

void SFMLRenderer::getScreenSize(int& width, int& height) const {
    // Get size from window manager
    if (windowManager) {
        windowManager->getWindowSize(width, height);
    } else {
        width = 0;
        height = 0;
    }
}

IResourceManager* SFMLRenderer::getResourceManager() const {
    return resourceManager;
}

IWindowManager* SFMLRenderer::getWindowManager() const {
    return windowManager;
}

sf::RenderTarget* SFMLRenderer::getCurrentRenderTarget() const {
    return currentRenderTarget;
}

size_t SFMLRenderer::getSpriteRenderCount() const {
    return currentFrameSpriteCount;
}

size_t SFMLRenderer::getRectRenderCount() const {
    return currentFrameRectCount;
}

bool SFMLRenderer::isInFrame() const {
    return frameActive;
}

void SFMLRenderer::updateRenderTarget() {
    // Get render target from window manager
    if (windowManager) {
        void* nativeTarget = windowManager->getNativeRenderTarget();
        currentRenderTarget = static_cast<sf::RenderTarget*>(nativeTarget);
    } else {
        currentRenderTarget = nullptr;
    }
}

const sf::Texture* SFMLRenderer::getTexture(int textureHandle) const {
    if (!resourceManager || !resourceManager->isTextureValid(textureHandle)) {
        return nullptr;
    }
    
    // Try to cast to SFMLResourceManager for real texture access
    // This allows both MockResourceManager (for testing) and SFMLResourceManager (for real rendering)
    auto* sfmlResourceManager = dynamic_cast<const class SFMLResourceManager*>(resourceManager);
    if (sfmlResourceManager) {
        return sfmlResourceManager->getSFMLTexture(textureHandle);
    }
    
    // For MockResourceManager or other implementations, return nullptr
    // (render counting still works for testing)
    return nullptr;
}

} // namespace ECS