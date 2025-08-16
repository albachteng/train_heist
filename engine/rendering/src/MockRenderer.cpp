#include "../include/MockRenderer.hpp"

namespace ECS {

// Stub implementations that will cause tests to fail (Red phase)

void MockRenderer::beginFrame() {
    // TODO: Record method call
}

void MockRenderer::endFrame() {
    // TODO: Record method call
}

void MockRenderer::clear() {
    // TODO: Record method call
}

void MockRenderer::renderSprite(float x, float y, float z, float width, float height, int textureId) {
    // TODO: Record sprite call
    (void)x; (void)y; (void)z; (void)width; (void)height; (void)textureId; // Suppress warnings
}

void MockRenderer::renderRect(float x, float y, float width, float height,
                             float red, float green, float blue, float alpha) {
    // TODO: Record rect call
    (void)x; (void)y; (void)width; (void)height; // Suppress warnings
    (void)red; (void)green; (void)blue; (void)alpha;
}

void MockRenderer::getScreenSize(int& width, int& height) const {
    // TODO: Return mock screen size
    width = 0;
    height = 0;
}

void MockRenderer::reset() {
    // TODO: Clear all recorded calls
}

size_t MockRenderer::getCallCount(const std::string& methodName) const {
    // TODO: Count method calls
    (void)methodName; // Suppress warning
    return 0;
}

bool MockRenderer::wasMethodCalled(const std::string& methodName) const {
    // TODO: Check if method was called
    (void)methodName; // Suppress warning
    return false;
}

void MockRenderer::setScreenSize(int width, int height) {
    // TODO: Set mock screen size
    (void)width; (void)height; // Suppress warnings
}

} // namespace ECS