#pragma once

#include "../../ecs/systems/include/IRenderer.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

namespace ECS {

/**
 * MockRenderer - Test implementation of IRenderer interface
 * 
 * Records all rendering calls for verification in tests.
 * Allows testing of rendering systems without requiring actual graphics.
 */
class MockRenderer : public IRenderer {
public:
    // Recorded call data structures
    struct RectCall {
        float x, y, width, height;
        float red, green, blue, alpha;
        
        bool operator==(const RectCall& other) const {
            const float epsilon = 0.001f;
            return std::abs(x - other.x) < epsilon &&
                   std::abs(y - other.y) < epsilon &&
                   std::abs(width - other.width) < epsilon &&
                   std::abs(height - other.height) < epsilon &&
                   std::abs(red - other.red) < epsilon &&
                   std::abs(green - other.green) < epsilon &&
                   std::abs(blue - other.blue) < epsilon &&
                   std::abs(alpha - other.alpha) < epsilon;
        }
    };
    
    struct SpriteCall {
        float x, y, z, width, height;
        int textureId;
        
        bool operator==(const SpriteCall& other) const {
            const float epsilon = 0.001f;
            return std::abs(x - other.x) < epsilon &&
                   std::abs(y - other.y) < epsilon &&
                   std::abs(z - other.z) < epsilon &&
                   std::abs(width - other.width) < epsilon &&
                   std::abs(height - other.height) < epsilon &&
                   textureId == other.textureId;
        }
    };
    
    // Recorded calls
    std::vector<RectCall> rectCalls;
    std::vector<SpriteCall> spriteCalls;
    std::vector<std::string> methodCalls;
    
    // Mock screen size
    int screenWidth = 800;
    int screenHeight = 600;
    
    // IRenderer interface implementation
    void beginFrame() override {
        methodCalls.push_back("beginFrame");
    }
    
    void endFrame() override {
        methodCalls.push_back("endFrame");
    }
    
    void clear() override {
        methodCalls.push_back("clear");
    }
    
    void renderSprite(float x, float y, float z, 
                     float width, float height, 
                     int textureId) override {
        spriteCalls.push_back({x, y, z, width, height, textureId});
        methodCalls.push_back("renderSprite");
    }
    
    void renderRect(float x, float y, float width, float height,
                   float red, float green, float blue, float alpha = 1.0f) override {
        rectCalls.push_back({x, y, width, height, red, green, blue, alpha});
        methodCalls.push_back("renderRect");
    }
    
    void getScreenSize(int& width, int& height) const override {
        width = screenWidth;
        height = screenHeight;
        // Note: Can't easily record const method calls
    }
    
    // Test utility methods
    void reset() {
        rectCalls.clear();
        spriteCalls.clear();
        methodCalls.clear();
    }
    
    size_t getCallCount(const std::string& methodName) const {
        return std::count(methodCalls.begin(), methodCalls.end(), methodName);
    }
    
    bool wasMethodCalled(const std::string& methodName) const {
        return std::find(methodCalls.begin(), methodCalls.end(), methodName) != methodCalls.end();
    }
};

} // namespace ECS