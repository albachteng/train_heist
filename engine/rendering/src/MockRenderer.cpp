#include "../include/MockRenderer.hpp"
#include <algorithm>

namespace ECS {

void MockRenderer::beginFrame() {
    methodCalls.push_back("beginFrame");
}

void MockRenderer::endFrame() {
    methodCalls.push_back("endFrame");
}

void MockRenderer::clear() {
    methodCalls.push_back("clear");
}

void MockRenderer::renderSprite(float x, float y, float z, float width, float height, int textureId) {
    spriteCalls.push_back({x, y, z, width, height, textureId});
    methodCalls.push_back("renderSprite");
}

void MockRenderer::renderRect(float x, float y, float width, float height,
                             float red, float green, float blue, float alpha) {
    rectCalls.push_back({x, y, width, height, red, green, blue, alpha});
    methodCalls.push_back("renderRect");
}

void MockRenderer::getScreenSize(int& width, int& height) const {
    width = screenWidth;
    height = screenHeight;
}

void MockRenderer::reset() {
    rectCalls.clear();
    spriteCalls.clear();
    methodCalls.clear();
}

size_t MockRenderer::getCallCount(const std::string& methodName) const {
    return std::count(methodCalls.begin(), methodCalls.end(), methodName);
}

bool MockRenderer::wasMethodCalled(const std::string& methodName) const {
    return std::find(methodCalls.begin(), methodCalls.end(), methodName) != methodCalls.end();
}

void MockRenderer::setScreenSize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
}

std::vector<std::string> MockRenderer::getCallSequence() const {
    return methodCalls;
}

bool MockRenderer::verifyCallSequence(const std::vector<std::string>& expectedSequence) const {
    if (methodCalls.size() != expectedSequence.size()) {
        return false;
    }
    
    for (size_t i = 0; i < methodCalls.size(); ++i) {
        if (methodCalls[i] != expectedSequence[i]) {
            return false;
        }
    }
    
    return true;
}

} // namespace ECS