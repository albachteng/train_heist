#include "../include/SFMLRenderer.hpp"
#include <SFML/Graphics.hpp>

namespace ECS {

SFMLRenderer::SFMLRenderer(IResourceManager* resourceManager, IWindowManager* windowManager)
    : resourceManager(resourceManager), windowManager(windowManager), currentRenderTarget(nullptr),
      frameActive(false), currentFrameSpriteCount(0), currentFrameRectCount(0) {
    
    // STUB: Will cause test failures in RED phase
    // Initialize SFML objects (stubbed)
    sprite = nullptr;
    rectangle = nullptr;
}

SFMLRenderer::~SFMLRenderer() {
    // STUB: Will cause test failures in RED phase - no cleanup
}

void SFMLRenderer::beginFrame() {
    // STUB: Will cause test failures in RED phase
    frameActive = false; // Should be true
}

void SFMLRenderer::endFrame() {
    // STUB: Will cause test failures in RED phase
    frameActive = true; // Should be false
}

void SFMLRenderer::clear() {
    // STUB: Will cause test failures in RED phase - doesn't actually clear
}

void SFMLRenderer::renderSprite(float x, float y, float z, float width, float height, int textureHandle) {
    // STUB: Will cause test failures in RED phase
    (void)x; (void)y; (void)z; (void)width; (void)height; (void)textureHandle;
    // Should increment currentFrameSpriteCount but doesn't
}

void SFMLRenderer::renderRect(float x, float y, float width, float height, float r, float g, float b, float a) {
    // STUB: Will cause test failures in RED phase
    (void)x; (void)y; (void)width; (void)height; (void)r; (void)g; (void)b; (void)a;
    // Should increment currentFrameRectCount but doesn't
}

void SFMLRenderer::getScreenSize(int& width, int& height) const {
    // STUB: Will cause test failures in RED phase
    width = 0;
    height = 0;
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
    // STUB: Will cause test failures in RED phase
    currentRenderTarget = nullptr;
}

const sf::Texture* SFMLRenderer::getTexture(int textureHandle) const {
    // STUB: Will cause test failures in RED phase
    (void)textureHandle;
    return nullptr;
}

} // namespace ECS