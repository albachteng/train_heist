#include <gtest/gtest.h>
#include "../include/SFMLRenderer.hpp"
#include "../include/MockResourceManager.hpp"
#include "../include/MockWindowManager.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <limits>

using namespace ECS;

/**
 * Tests for SFML-specific mathematical and logical operations
 * These tests focus on color conversion, scaling calculations, and other SFML wrapper logic
 */
class SFMLLogicTests : public ::testing::Test {
protected:
    void SetUp() override {
        mockResourceManager = std::make_unique<MockResourceManager>();
        mockWindowManager = std::make_unique<MockWindowManager>();
        renderer = std::make_unique<SFMLRenderer>(mockResourceManager.get(), mockWindowManager.get());
    }
    
    void TearDown() override {
        renderer.reset();
        mockResourceManager.reset();
        mockWindowManager.reset();
    }
    
    std::unique_ptr<MockResourceManager> mockResourceManager;
    std::unique_ptr<MockWindowManager> mockWindowManager;
    std::unique_ptr<SFMLRenderer> renderer;
    
    // Helper function to test color conversion logic
    // We can't directly test the internal conversion, but we can test edge cases
    void testColorConversion(float r, float g, float b, float a) {
        renderer->beginFrame();
        renderer->renderRect(0.0f, 0.0f, 100.0f, 100.0f, r, g, b, a);
        EXPECT_EQ(renderer->getRectRenderCount(), 1u);
        renderer->endFrame();
    }
};

/**
 * Test color conversion edge cases and clamping logic
 */
TEST_F(SFMLLogicTests, ColorConversionClamping) {
    // Test normal range (0.0 to 1.0)
    testColorConversion(0.0f, 0.5f, 1.0f, 0.75f);
    
    // Test negative values (should clamp to 0)
    testColorConversion(-0.5f, -1.0f, -100.0f, -0.1f);
    
    // Test values above 1.0 (should clamp to 1.0/255)
    testColorConversion(1.5f, 2.0f, 255.0f, 10.0f);
    
    // Test extreme values
    testColorConversion(-1000.0f, 1000.0f, std::numeric_limits<float>::infinity(), 
                       -std::numeric_limits<float>::infinity());
    
    // Test edge values
    testColorConversion(0.0f, 1.0f, 0.0f, 1.0f);
    
    // Test precision edge cases
    testColorConversion(0.0039f, 0.9961f, 0.5f, 0.5f); // Close to boundaries
}

/**
 * Test mathematical calculations for sprite scaling
 * We can't directly access the scaling logic, but we can test scenarios that would use it
 */
TEST_F(SFMLLogicTests, SpriteScalingScenarios) {
    renderer->beginFrame();
    
    // Test various sprite size scenarios
    renderer->renderSprite(0.0f, 0.0f, 0.0f, 100.0f, 100.0f, 1); // Square
    renderer->renderSprite(100.0f, 0.0f, 0.0f, 200.0f, 50.0f, 1); // Wide rectangle
    renderer->renderSprite(0.0f, 100.0f, 0.0f, 50.0f, 200.0f, 1); // Tall rectangle
    
    // Test edge cases
    renderer->renderSprite(300.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1); // Very small
    renderer->renderSprite(0.0f, 300.0f, 0.0f, 1000.0f, 1000.0f, 1); // Very large
    
    // Test zero dimensions (edge case)
    renderer->renderSprite(500.0f, 0.0f, 0.0f, 0.0f, 100.0f, 1); // Zero width
    renderer->renderSprite(0.0f, 500.0f, 0.0f, 100.0f, 0.0f, 1); // Zero height
    renderer->renderSprite(500.0f, 500.0f, 0.0f, 0.0f, 0.0f, 1); // Zero both
    
    EXPECT_EQ(renderer->getSpriteRenderCount(), 8u);
    renderer->endFrame();
}

/**
 * Test coordinate system handling and precision
 */
TEST_F(SFMLLogicTests, CoordinateSystemHandling) {
    renderer->beginFrame();
    
    // Test floating point precision
    renderer->renderRect(0.1f, 0.1f, 10.1f, 10.1f, 1.0f, 0.0f, 0.0f, 1.0f);
    renderer->renderRect(0.01f, 0.01f, 1.01f, 1.01f, 0.0f, 1.0f, 0.0f, 1.0f);
    renderer->renderRect(0.001f, 0.001f, 0.101f, 0.101f, 0.0f, 0.0f, 1.0f, 1.0f);
    
    // Test negative coordinates
    renderer->renderRect(-100.0f, -50.0f, 200.0f, 100.0f, 1.0f, 1.0f, 0.0f, 1.0f);
    
    // Test large coordinates
    renderer->renderRect(10000.0f, 5000.0f, 100.0f, 100.0f, 0.5f, 0.5f, 0.5f, 1.0f);
    
    // Test extreme coordinates
    float maxFloat = std::numeric_limits<float>::max();
    renderer->renderRect(maxFloat, maxFloat, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    
    EXPECT_EQ(renderer->getRectRenderCount(), 6u);
    renderer->endFrame();
}

/**
 * Test Z-coordinate handling (currently ignored but should not cause issues)
 */
TEST_F(SFMLLogicTests, ZCoordinateHandling) {
    renderer->beginFrame();
    
    // Test various Z values
    renderer->renderSprite(0.0f, 0.0f, -100.0f, 50.0f, 50.0f, 1);
    renderer->renderSprite(50.0f, 0.0f, 0.0f, 50.0f, 50.0f, 1);
    renderer->renderSprite(100.0f, 0.0f, 100.0f, 50.0f, 50.0f, 1);
    renderer->renderSprite(150.0f, 0.0f, 1000.0f, 50.0f, 50.0f, 1);
    
    // Extreme Z values
    renderer->renderSprite(200.0f, 0.0f, std::numeric_limits<float>::max(), 50.0f, 50.0f, 1);
    renderer->renderSprite(250.0f, 0.0f, std::numeric_limits<float>::lowest(), 50.0f, 50.0f, 1);
    
    EXPECT_EQ(renderer->getSpriteRenderCount(), 6u);
    renderer->endFrame();
}

/**
 * Test texture handle validation and edge cases
 */
TEST_F(SFMLLogicTests, TextureHandleValidation) {
    renderer->beginFrame();
    
    // Test valid-looking handles
    renderer->renderSprite(0.0f, 0.0f, 0.0f, 100.0f, 100.0f, 1);
    renderer->renderSprite(100.0f, 0.0f, 0.0f, 100.0f, 100.0f, 100);
    
    // Test edge case handles
    renderer->renderSprite(200.0f, 0.0f, 0.0f, 100.0f, 100.0f, 0);
    renderer->renderSprite(300.0f, 0.0f, 0.0f, 100.0f, 100.0f, -1);
    renderer->renderSprite(400.0f, 0.0f, 0.0f, 100.0f, 100.0f, std::numeric_limits<int>::max());
    renderer->renderSprite(500.0f, 0.0f, 0.0f, 100.0f, 100.0f, std::numeric_limits<int>::min());
    
    // All should be counted as render attempts regardless of handle validity
    EXPECT_EQ(renderer->getSpriteRenderCount(), 6u);
    renderer->endFrame();
}

/**
 * Test mathematical edge cases and special values
 */
TEST_F(SFMLLogicTests, MathematicalEdgeCases) {
    renderer->beginFrame();
    
    // Test with NaN values
    float nan = std::numeric_limits<float>::quiet_NaN();
    renderer->renderRect(nan, nan, nan, nan, nan, nan, nan, nan);
    
    // Test with infinity
    float inf = std::numeric_limits<float>::infinity();
    float negInf = -std::numeric_limits<float>::infinity();
    renderer->renderRect(inf, negInf, inf, negInf, inf, negInf, inf, negInf);
    
    // Test with very small values
    float epsilon = std::numeric_limits<float>::epsilon();
    renderer->renderRect(epsilon, epsilon, epsilon, epsilon, epsilon, epsilon, epsilon, epsilon);
    
    // Test with denormalized numbers
    float denorm = std::numeric_limits<float>::denorm_min();
    renderer->renderRect(denorm, denorm, denorm, denorm, denorm, denorm, denorm, denorm);
    
    // Should handle all gracefully
    EXPECT_EQ(renderer->getRectRenderCount(), 4u);
    renderer->endFrame();
}

/**
 * Test render state consistency across operations
 */
TEST_F(SFMLLogicTests, RenderStateConsistency) {
    // Test that render counts are accurate across mixed operations
    renderer->beginFrame();
    
    // Interleave rect and sprite rendering
    renderer->renderRect(0.0f, 0.0f, 50.0f, 50.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(renderer->getRectRenderCount(), 1u);
    EXPECT_EQ(renderer->getSpriteRenderCount(), 0u);
    
    renderer->renderSprite(60.0f, 0.0f, 0.0f, 50.0f, 50.0f, 1);
    EXPECT_EQ(renderer->getRectRenderCount(), 1u);
    EXPECT_EQ(renderer->getSpriteRenderCount(), 1u);
    
    renderer->renderRect(0.0f, 60.0f, 50.0f, 50.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    EXPECT_EQ(renderer->getRectRenderCount(), 2u);
    EXPECT_EQ(renderer->getSpriteRenderCount(), 1u);
    
    renderer->renderSprite(60.0f, 60.0f, 0.0f, 50.0f, 50.0f, 2);
    EXPECT_EQ(renderer->getRectRenderCount(), 2u);
    EXPECT_EQ(renderer->getSpriteRenderCount(), 2u);
    
    renderer->endFrame();
}

/**
 * Test alpha blending edge cases
 */
TEST_F(SFMLLogicTests, AlphaBlendingEdgeCases) {
    renderer->beginFrame();
    
    // Test fully transparent
    renderer->renderRect(0.0f, 0.0f, 100.0f, 100.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    
    // Test fully opaque
    renderer->renderRect(100.0f, 0.0f, 100.0f, 100.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    
    // Test partial transparency
    renderer->renderRect(200.0f, 0.0f, 100.0f, 100.0f, 0.0f, 0.0f, 1.0f, 0.5f);
    
    // Test very low alpha (should still clamp properly)
    renderer->renderRect(300.0f, 0.0f, 100.0f, 100.0f, 1.0f, 1.0f, 0.0f, 0.001f);
    
    EXPECT_EQ(renderer->getRectRenderCount(), 4u);
    renderer->endFrame();
}