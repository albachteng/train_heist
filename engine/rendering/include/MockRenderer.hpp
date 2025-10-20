#pragma once

#include "IRenderer.hpp"
#include <vector>
#include <string>

namespace ECS {

/**
 * MockRenderer - Test implementation of IRenderer interface
 * 
 * Records all rendering calls for verification in tests.
 * Allows testing of rendering systems without requiring actual graphics.
 * 
 * This is a stub for Red phase - implementation will make tests pass.
 */
class MockRenderer : public IRenderer {
public:
    // Constructor to ensure proper initialization
    MockRenderer() : screenWidth(800), screenHeight(600) {}
    
    // Call recording structures (stubs - will be implemented)
    struct RectCall {
        float x, y, width, height;
        float red, green, blue, alpha;
    };
    
    struct SpriteCall {
        float x, y, z, width, height;
        int textureId;
    };
    
    // Recorded calls (stubs)
    std::vector<RectCall> rectCalls;
    std::vector<SpriteCall> spriteCalls;
    std::vector<std::string> methodCalls;
    
    // IRenderer interface (stubs - will cause test failures)
    void beginFrame() override;
    void endFrame() override;
    void clear() override;
    void renderSprite(float x, float y, float z, float width, float height, int textureId) override;
    void renderRect(float x, float y, float width, float height,
                   float red, float green, float blue, float alpha = 1.0f) override;
    void getScreenSize(int& width, int& height) const override;
    
    // Test utility methods
    void reset();
    size_t getCallCount(const std::string& methodName) const;
    bool wasMethodCalled(const std::string& methodName) const;
    void setScreenSize(int width, int height);

    // Call order verification methods
    std::vector<std::string> getCallSequence() const;
    bool verifyCallSequence(const std::vector<std::string>& expectedSequence) const;

    // Specific call data access for detailed verification
    size_t getSpriteCallCount() const;
    const SpriteCall& getSpriteCall(size_t index) const;
    size_t getRectCallCount() const;
    const RectCall& getRectCall(size_t index) const;

private:
    // Mock screen size
    int screenWidth = 800;
    int screenHeight = 600;
};

} // namespace ECS