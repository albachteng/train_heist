#include <gtest/gtest.h>
#include "../include/SFMLRenderer.hpp"
#include "../include/MockResourceManager.hpp"
#include "../include/MockWindowManager.hpp"
#include "../include/SFMLResourceManager.hpp"
#include "../include/SFMLWindowManager.hpp"
#include <memory>

using namespace ECS;

/**
 * Integration tests for SFML components working together
 * These tests focus on cross-component integration and dependency injection patterns
 */
class SFMLIntegrationTests : public ::testing::Test {
protected:
    void SetUp() override {
        mockResourceManager = std::make_unique<MockResourceManager>();
        mockWindowManager = std::make_unique<MockWindowManager>();
        
        // Create renderer with mock dependencies
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
};

/**
 * Test resource manager integration and dynamic casting
 */
TEST_F(SFMLIntegrationTests, ResourceManagerDynamicCasting) {
    // Test with MockResourceManager (should not cast to SFMLResourceManager)
    renderer->beginFrame();
    
    // This should work but not render actual sprites (no SFML textures available)
    renderer->renderSprite(10.0f, 20.0f, 0.0f, 100.0f, 50.0f, 1);
    
    // Render counting should still work
    EXPECT_EQ(renderer->getSpriteRenderCount(), 1u);
    
    renderer->endFrame();
}

TEST_F(SFMLIntegrationTests, ResourceManagerIntegrationWithSFMLResourceManager) {
    // Create SFMLResourceManager for real integration test
    auto sfmlResourceManager = std::make_unique<SFMLResourceManager>();
    auto sfmlRenderer = std::make_unique<SFMLRenderer>(sfmlResourceManager.get(), mockWindowManager.get());
    
    sfmlRenderer->beginFrame();
    
    // Test with invalid texture handle
    sfmlRenderer->renderSprite(10.0f, 20.0f, 0.0f, 100.0f, 50.0f, 999);
    EXPECT_EQ(sfmlRenderer->getSpriteRenderCount(), 1u); // Still counts render attempt
    
    // Test with valid handle (if resource manager has textures)
    sfmlRenderer->renderSprite(10.0f, 20.0f, 0.0f, 100.0f, 50.0f, 1);
    EXPECT_EQ(sfmlRenderer->getSpriteRenderCount(), 2u);
    
    sfmlRenderer->endFrame();
}

/**
 * Test color conversion logic (0.0-1.0 to 0-255) with actual color verification
 */
TEST_F(SFMLIntegrationTests, ColorConversionClamping) {
    renderer->beginFrame();
    
    // Test normal color values (0.5, 0.75, 1.0, 0.8) -> (127, 191, 255, 204)
    renderer->renderRect(0.0f, 0.0f, 100.0f, 100.0f, 0.5f, 0.75f, 1.0f, 0.8f);
    sf::Color color1 = renderer->getLastRectangleColor();
    EXPECT_EQ(color1.r, 127u);   // 0.5 * 255 = 127.5 -> 127
    EXPECT_EQ(color1.g, 191u);   // 0.75 * 255 = 191.25 -> 191
    EXPECT_EQ(color1.b, 255u);   // 1.0 * 255 = 255
    EXPECT_EQ(color1.a, 204u);   // 0.8 * 255 = 204
    EXPECT_EQ(renderer->getRectRenderCount(), 1u);
    
    // Test edge case values - negative should clamp to 0, >1.0 should clamp to 255
    renderer->renderRect(10.0f, 10.0f, 100.0f, 100.0f, -0.5f, 1.5f, 0.0f, 1.0f);
    sf::Color color2 = renderer->getLastRectangleColor();
    EXPECT_EQ(color2.r, 0u);     // -0.5 should clamp to 0
    EXPECT_EQ(color2.g, 255u);   // 1.5 should clamp to 255
    EXPECT_EQ(color2.b, 0u);     // 0.0 -> 0
    EXPECT_EQ(color2.a, 255u);   // 1.0 -> 255
    EXPECT_EQ(renderer->getRectRenderCount(), 2u);
    
    // Test extreme values - should clamp properly
    renderer->renderRect(20.0f, 20.0f, 100.0f, 100.0f, -100.0f, 255.0f, 0.5f, 2.0f);
    sf::Color color3 = renderer->getLastRectangleColor();
    EXPECT_EQ(color3.r, 0u);     // -100.0 should clamp to 0
    EXPECT_EQ(color3.g, 255u);   // 255.0 should clamp to 255
    EXPECT_EQ(color3.b, 127u);   // 0.5 * 255 = 127.5 -> 127
    EXPECT_EQ(color3.a, 255u);   // 2.0 should clamp to 255
    EXPECT_EQ(renderer->getRectRenderCount(), 3u);
    
    // Test exact boundary values
    renderer->renderRect(30.0f, 30.0f, 100.0f, 100.0f, 0.0f, 1.0f, 0.0039f, 0.9961f);
    sf::Color color4 = renderer->getLastRectangleColor();
    EXPECT_EQ(color4.r, 0u);     // 0.0 -> 0
    EXPECT_EQ(color4.g, 255u);   // 1.0 -> 255
    EXPECT_EQ(color4.b, 0u);     // 0.0039 * 255 = 0.9945 -> 0 (rounds down)
    EXPECT_EQ(color4.a, 254u);   // 0.9961 * 255 = 254.0055 -> 254
    EXPECT_EQ(renderer->getRectRenderCount(), 4u);
    
    renderer->endFrame();
}

/**
 * Test render target management with different window manager states
 */
TEST_F(SFMLIntegrationTests, RenderTargetManagement) {
    // Test with mock window manager that returns null
    renderer->beginFrame();
    
    // Should handle null render target gracefully
    renderer->renderRect(0.0f, 0.0f, 50.0f, 50.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(renderer->getRectRenderCount(), 1u); // Still counts render attempts
    
    renderer->endFrame();
}

/**
 * Test frame state management
 */
TEST_F(SFMLIntegrationTests, FrameStateManagement) {
    // Test initial state
    EXPECT_FALSE(renderer->isInFrame());
    EXPECT_EQ(renderer->getSpriteRenderCount(), 0u);
    EXPECT_EQ(renderer->getRectRenderCount(), 0u);
    
    // Test begin frame
    renderer->beginFrame();
    EXPECT_TRUE(renderer->isInFrame());
    
    // Test render counting during frame
    renderer->renderRect(0.0f, 0.0f, 100.0f, 100.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    renderer->renderSprite(10.0f, 10.0f, 0.0f, 50.0f, 50.0f, 1);
    
    EXPECT_EQ(renderer->getRectRenderCount(), 1u);
    EXPECT_EQ(renderer->getSpriteRenderCount(), 1u);
    
    // Test end frame
    renderer->endFrame();
    EXPECT_FALSE(renderer->isInFrame());
    
    // Test that new frame resets counters
    renderer->beginFrame();
    EXPECT_EQ(renderer->getRectRenderCount(), 0u);
    EXPECT_EQ(renderer->getSpriteRenderCount(), 0u);
    
    renderer->endFrame();
}

/**
 * Test dependency injection validation
 */
TEST_F(SFMLIntegrationTests, DependencyInjectionValidation) {
    // Test getters
    EXPECT_EQ(renderer->getResourceManager(), mockResourceManager.get());
    EXPECT_EQ(renderer->getWindowManager(), mockWindowManager.get());
    
    // Test with null dependencies
    auto nullRenderer = std::make_unique<SFMLRenderer>(nullptr, nullptr);
    
    nullRenderer->beginFrame();
    
    // Should handle null dependencies gracefully
    nullRenderer->clear(); // Should not crash
    nullRenderer->renderRect(0.0f, 0.0f, 100.0f, 100.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    nullRenderer->renderSprite(0.0f, 0.0f, 0.0f, 100.0f, 100.0f, 1);
    
    // Render counting should still work
    EXPECT_EQ(nullRenderer->getRectRenderCount(), 1u);
    EXPECT_EQ(nullRenderer->getSpriteRenderCount(), 1u);
    
    int width, height;
    nullRenderer->getScreenSize(width, height);
    EXPECT_EQ(width, 0);
    EXPECT_EQ(height, 0);
    
    nullRenderer->endFrame();
}

/**
 * Test SFML-specific render target casting
 */
TEST_F(SFMLIntegrationTests, RenderTargetCasting) {
    // Test getCurrentRenderTarget method
    renderer->beginFrame();
    
    // With MockWindowManager, this might return nullptr or a mock object
    sf::RenderTarget* target = renderer->getCurrentRenderTarget();
    (void)target; // Suppress unused variable warning
    
    // Should not crash regardless of what mock returns
    renderer->renderRect(0.0f, 0.0f, 100.0f, 100.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    
    renderer->endFrame();
}

/**
 * Test edge cases and error conditions
 */
TEST_F(SFMLIntegrationTests, EdgeCasesAndErrorConditions) {
    renderer->beginFrame();
    
    // Test with extreme coordinates
    renderer->renderRect(-1000.0f, -1000.0f, 0.1f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f);
    renderer->renderSprite(10000.0f, 10000.0f, 0.0f, 1.0f, 1.0f, -1);
    
    // Test with zero/negative dimensions
    renderer->renderRect(100.0f, 100.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    renderer->renderSprite(100.0f, 100.0f, 0.0f, -50.0f, -50.0f, 1);
    
    // All should be counted as render attempts
    EXPECT_EQ(renderer->getRectRenderCount(), 2u);
    EXPECT_EQ(renderer->getSpriteRenderCount(), 2u);
    
    renderer->endFrame();
}

/**
 * Test multiple frame cycles
 */
TEST_F(SFMLIntegrationTests, MultipleFrameCycles) {
    for (int frame = 0; frame < 5; ++frame) {
        renderer->beginFrame();
        EXPECT_TRUE(renderer->isInFrame());
        
        // Each frame should start with zero counts
        EXPECT_EQ(renderer->getRectRenderCount(), 0u);
        EXPECT_EQ(renderer->getSpriteRenderCount(), 0u);
        
        // Render some objects
        for (int i = 0; i < 3; ++i) {
            renderer->renderRect(i * 10.0f, i * 10.0f, 50.0f, 50.0f, 1.0f, 0.0f, 0.0f, 1.0f);
        }
        
        EXPECT_EQ(renderer->getRectRenderCount(), 3u);
        
        renderer->endFrame();
        EXPECT_FALSE(renderer->isInFrame());
    }
}