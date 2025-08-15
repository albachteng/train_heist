#include "../include/SFMLRenderer.hpp"
#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>

/**
 * SFMLRenderer Tests
 * 
 * These tests verify the SFMLRenderer interface compliance and basic functionality.
 * Note: We can't easily test actual rendering without a graphics context,
 * but we can test interface behavior and parameter handling.
 */

class SFMLRendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a minimal SFML window for testing
        // Note: This may fail in headless environments
        try {
            window = std::make_unique<sf::RenderWindow>(
                sf::VideoMode(sf::Vector2u(800, 600)), 
                "Test Window",
                sf::Style::None  // No decorations for testing
            );
            window->setVisible(false); // Hide test window
            renderer = std::make_unique<ECS::SFMLRenderer>(*window);
        } catch (...) {
            // Skip tests if we can't create window (headless environment)
            GTEST_SKIP() << "Cannot create SFML window - skipping SFMLRenderer tests";
        }
    }
    
    void TearDown() override {
        renderer.reset();
        if (window && window->isOpen()) {
            window->close();
        }
        window.reset();
    }
    
    std::unique_ptr<sf::RenderWindow> window;
    std::unique_ptr<ECS::SFMLRenderer> renderer;
};

// Test basic interface compliance
TEST_F(SFMLRendererTest, InterfaceCompliance) {
    if (!renderer) GTEST_SKIP();
    
    // Should be able to call all IRenderer methods without crashing
    EXPECT_NO_THROW(renderer->beginFrame());
    EXPECT_NO_THROW(renderer->clear());
    EXPECT_NO_THROW(renderer->endFrame());
    
    EXPECT_NO_THROW(renderer->renderRect(100.0f, 100.0f, 50.0f, 50.0f, 
                                        1.0f, 0.0f, 0.0f, 1.0f));
    
    EXPECT_NO_THROW(renderer->renderSprite(200.0f, 200.0f, 0.0f, 
                                          64.0f, 64.0f, 1));
}

// Test screen size retrieval
TEST_F(SFMLRendererTest, ScreenSizeRetrieval) {
    if (!renderer) GTEST_SKIP();
    
    int width, height;
    renderer->getScreenSize(width, height);
    
    EXPECT_EQ(width, 800);
    EXPECT_EQ(height, 600);
}

// Test renderRect with various parameters
TEST_F(SFMLRendererTest, RenderRectParameters) {
    if (!renderer) GTEST_SKIP();
    
    renderer->beginFrame();
    renderer->clear();
    
    // Test various rectangle parameters
    EXPECT_NO_THROW(renderer->renderRect(0.0f, 0.0f, 10.0f, 10.0f, 
                                        1.0f, 1.0f, 1.0f, 1.0f)); // White
    
    EXPECT_NO_THROW(renderer->renderRect(100.0f, 100.0f, 50.0f, 50.0f, 
                                        0.0f, 0.0f, 0.0f, 0.5f)); // Semi-transparent black
    
    EXPECT_NO_THROW(renderer->renderRect(-10.0f, -10.0f, 20.0f, 20.0f, 
                                        1.0f, 0.0f, 1.0f, 1.0f)); // Off-screen position
    
    EXPECT_NO_THROW(renderer->renderRect(400.0f, 300.0f, 0.0f, 0.0f, 
                                        0.5f, 0.5f, 0.5f, 1.0f)); // Zero size
    
    renderer->endFrame();
}

// Test renderSprite with various parameters  
TEST_F(SFMLRendererTest, RenderSpriteParameters) {
    if (!renderer) GTEST_SKIP();
    
    renderer->beginFrame();
    renderer->clear();
    
    // Test various sprite parameters
    EXPECT_NO_THROW(renderer->renderSprite(0.0f, 0.0f, 0.0f, 
                                          32.0f, 32.0f, 0)); // Texture ID 0
    
    EXPECT_NO_THROW(renderer->renderSprite(100.0f, 100.0f, 1.0f, 
                                          64.0f, 64.0f, 999)); // High texture ID
    
    EXPECT_NO_THROW(renderer->renderSprite(-50.0f, -50.0f, -1.0f, 
                                          128.0f, 128.0f, -1)); // Negative values
    
    EXPECT_NO_THROW(renderer->renderSprite(400.0f, 300.0f, 0.0f, 
                                          0.0f, 0.0f, 1)); // Zero size
    
    renderer->endFrame();
}

// Test frame sequence
TEST_F(SFMLRendererTest, FrameSequence) {
    if (!renderer) GTEST_SKIP();
    
    // Test proper frame sequence
    EXPECT_NO_THROW({
        renderer->beginFrame();
        renderer->clear();
        renderer->renderRect(100.0f, 100.0f, 50.0f, 50.0f, 1.0f, 0.0f, 0.0f, 1.0f);
        renderer->renderSprite(200.0f, 200.0f, 0.0f, 64.0f, 64.0f, 1);
        renderer->endFrame();
    });
}

// Test multiple frames
TEST_F(SFMLRendererTest, MultipleFrames) {
    if (!renderer) GTEST_SKIP();
    
    // Render multiple frames to ensure stability
    for (int i = 0; i < 3; ++i) {
        EXPECT_NO_THROW({
            renderer->beginFrame();
            renderer->clear();
            
            // Render different content each frame
            renderer->renderRect(i * 10.0f, i * 10.0f, 50.0f, 50.0f, 
                               i / 3.0f, 0.5f, 1.0f - i / 3.0f, 1.0f);
            
            renderer->endFrame();
        });
    }
}

// Test color clamping behavior
TEST_F(SFMLRendererTest, ColorClamping) {
    if (!renderer) GTEST_SKIP();
    
    renderer->beginFrame();
    renderer->clear();
    
    // Test out-of-range color values (should be handled gracefully)
    EXPECT_NO_THROW(renderer->renderRect(100.0f, 100.0f, 50.0f, 50.0f, 
                                        -1.0f, 2.0f, 0.5f, 1.5f)); // Out of [0,1] range
    
    EXPECT_NO_THROW(renderer->renderRect(200.0f, 200.0f, 50.0f, 50.0f, 
                                        std::numeric_limits<float>::infinity(), 
                                        std::numeric_limits<float>::quiet_NaN(), 
                                        0.0f, 1.0f)); // Extreme values
    
    renderer->endFrame();
}

// Test window dependency
TEST_F(SFMLRendererTest, WindowDependency) {
    if (!renderer) GTEST_SKIP();
    
    // Renderer should work as long as window is valid
    EXPECT_TRUE(window->isOpen());
    
    int width, height;
    renderer->getScreenSize(width, height);
    EXPECT_GT(width, 0);
    EXPECT_GT(height, 0);
}

// Test that constructor doesn't crash with valid window
TEST_F(SFMLRendererTest, ConstructorStability) {
    if (!window) GTEST_SKIP();
    
    // Creating multiple renderers with same window should work
    EXPECT_NO_THROW({
        ECS::SFMLRenderer renderer1(*window);
        ECS::SFMLRenderer renderer2(*window);
        // Both should be valid
    });
}

// Performance test - basic rendering throughput
TEST_F(SFMLRendererTest, BasicPerformance) {
    if (!renderer) GTEST_SKIP();
    
    renderer->beginFrame();
    renderer->clear();
    
    // Render many rectangles (should complete without timeout)
    for (int i = 0; i < 100; ++i) {
        renderer->renderRect(i % 800, (i / 800) * 10, 8.0f, 8.0f, 
                           i / 100.0f, 0.5f, 1.0f - i / 100.0f, 1.0f);
    }
    
    renderer->endFrame();
    
    // If we get here without timeout, performance is acceptable
    SUCCEED();
}