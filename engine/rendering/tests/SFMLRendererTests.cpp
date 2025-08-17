#include "../include/SFMLRenderer.hpp"
#include "../include/MockResourceManager.hpp"
#include "../include/MockWindowManager.hpp"
#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>

using namespace ECS;

/**
 * SFMLRenderer Tests
 * 
 * These tests define the expected behavior of SFMLRenderer for SFML-based rendering.
 * SFMLRenderer should integrate ResourceManager and WindowManager to provide hardware-accelerated rendering.
 * 
 * Key test areas:
 * - Dependency injection of ResourceManager and WindowManager
 * - Frame lifecycle management (beginFrame/endFrame)
 * - Sprite rendering with texture handle resolution
 * - Rectangle rendering with color specification
 * - Render target management from WindowManager
 * - Screen size queries through WindowManager
 * - Render statistics tracking for performance monitoring
 * - SFML integration and object lifecycle
 * - Error handling for invalid states and parameters
 */

class SFMLRendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockResourceManager = std::make_unique<MockResourceManager>();
        mockWindowManager = std::make_unique<MockWindowManager>();
        
        // Create window for rendering tests
        mockWindowManager->createWindow(1024, 768, "Test Window");
        
        sfmlRenderer = std::make_unique<SFMLRenderer>(
            mockResourceManager.get(), 
            mockWindowManager.get()
        );
    }
    
    void TearDown() override {
        sfmlRenderer.reset();
        mockWindowManager.reset();
        mockResourceManager.reset();
    }
    
    std::unique_ptr<MockResourceManager> mockResourceManager;
    std::unique_ptr<MockWindowManager> mockWindowManager;
    std::unique_ptr<SFMLRenderer> sfmlRenderer;
};

// Test basic interface compliance
TEST_F(SFMLRendererTest, InterfaceCompliance) {
    // Should implement all IRenderer methods without crashing
    EXPECT_NO_THROW(sfmlRenderer->beginFrame());
    EXPECT_NO_THROW(sfmlRenderer->clear());
    EXPECT_NO_THROW(sfmlRenderer->endFrame());
    EXPECT_NO_THROW(sfmlRenderer->renderSprite(0, 0, 0, 32, 32, 1));
    EXPECT_NO_THROW(sfmlRenderer->renderRect(0, 0, 100, 100, 1.0f, 0.0f, 0.0f, 1.0f));
    
    int width, height;
    EXPECT_NO_THROW(sfmlRenderer->getScreenSize(width, height));
}

// Test dependency injection
TEST_F(SFMLRendererTest, DependencyInjection) {
    // Should store injected dependencies
    EXPECT_EQ(sfmlRenderer->getResourceManager(), mockResourceManager.get());
    EXPECT_EQ(sfmlRenderer->getWindowManager(), mockWindowManager.get());
    
    // Should not be null pointers
    EXPECT_NE(sfmlRenderer->getResourceManager(), nullptr);
    EXPECT_NE(sfmlRenderer->getWindowManager(), nullptr);
}

// Test frame lifecycle management
TEST_F(SFMLRendererTest, FrameLifecycle) {
    // Initially should not be in frame
    EXPECT_FALSE(sfmlRenderer->isInFrame());
    
    // Begin frame should set frame active
    sfmlRenderer->beginFrame();
    EXPECT_TRUE(sfmlRenderer->isInFrame());
    
    // End frame should clear frame active
    sfmlRenderer->endFrame();
    EXPECT_FALSE(sfmlRenderer->isInFrame());
}

// Test render target management
TEST_F(SFMLRendererTest, RenderTargetManagement) {
    // Should get render target from window manager
    sfmlRenderer->beginFrame();
    
    sf::RenderTarget* renderTarget = sfmlRenderer->getCurrentRenderTarget();
    
    // Should have valid render target when window is open
    EXPECT_NE(renderTarget, nullptr);
    
    sfmlRenderer->endFrame();
}

// Test render target with closed window
TEST_F(SFMLRendererTest, RenderTargetWithClosedWindow) {
    // Close the window
    mockWindowManager->closeWindow();
    
    sfmlRenderer->beginFrame();
    
    // Should handle closed window gracefully
    sf::RenderTarget* renderTarget = sfmlRenderer->getCurrentRenderTarget();
    EXPECT_EQ(renderTarget, nullptr);
    
    sfmlRenderer->endFrame();
}

// Test screen size queries
TEST_F(SFMLRendererTest, ScreenSizeQueries) {
    int width, height;
    sfmlRenderer->getScreenSize(width, height);
    
    // Should get size from window manager
    EXPECT_EQ(width, 1024);
    EXPECT_EQ(height, 768);
}

// Test screen size with different window size
TEST_F(SFMLRendererTest, ScreenSizeWithDifferentWindow) {
    // Change window size
    mockWindowManager->setWindowSize(1920, 1080);
    
    int width, height;
    sfmlRenderer->getScreenSize(width, height);
    
    EXPECT_EQ(width, 1920);
    EXPECT_EQ(height, 1080);
}

// Test sprite rendering
TEST_F(SFMLRendererTest, SpriteRendering) {
    // Load a texture first
    int textureHandle = mockResourceManager->loadTexture("test_sprite.png");
    EXPECT_NE(textureHandle, INVALID_TEXTURE);
    
    sfmlRenderer->beginFrame();
    
    // Initially should have no sprites rendered
    EXPECT_EQ(sfmlRenderer->getSpriteRenderCount(), 0);
    
    // Render a sprite
    sfmlRenderer->renderSprite(100, 200, 0, 64, 64, textureHandle);
    
    // Should increment sprite count
    EXPECT_EQ(sfmlRenderer->getSpriteRenderCount(), 1);
    
    sfmlRenderer->endFrame();
}

// Test multiple sprite rendering
TEST_F(SFMLRendererTest, MultipleSpriteRendering) {
    int textureHandle1 = mockResourceManager->loadTexture("sprite1.png");
    int textureHandle2 = mockResourceManager->loadTexture("sprite2.png");
    
    sfmlRenderer->beginFrame();
    
    // Render multiple sprites
    sfmlRenderer->renderSprite(0, 0, 0, 32, 32, textureHandle1);
    sfmlRenderer->renderSprite(100, 100, 1, 64, 64, textureHandle2);
    sfmlRenderer->renderSprite(200, 200, 2, 48, 48, textureHandle1);
    
    // Should count all sprites
    EXPECT_EQ(sfmlRenderer->getSpriteRenderCount(), 3);
    
    sfmlRenderer->endFrame();
}

// Test sprite rendering with invalid texture
TEST_F(SFMLRendererTest, SpriteRenderingWithInvalidTexture) {
    sfmlRenderer->beginFrame();
    
    // Render sprite with invalid texture handle
    EXPECT_NO_THROW(sfmlRenderer->renderSprite(0, 0, 0, 32, 32, INVALID_TEXTURE));
    
    // Should still count the render attempt
    EXPECT_EQ(sfmlRenderer->getSpriteRenderCount(), 1);
    
    sfmlRenderer->endFrame();
}

// Test rectangle rendering
TEST_F(SFMLRendererTest, RectangleRendering) {
    sfmlRenderer->beginFrame();
    
    // Initially should have no rectangles rendered
    EXPECT_EQ(sfmlRenderer->getRectRenderCount(), 0);
    
    // Render a red rectangle
    sfmlRenderer->renderRect(50, 75, 200, 150, 1.0f, 0.0f, 0.0f, 1.0f);
    
    // Should increment rectangle count
    EXPECT_EQ(sfmlRenderer->getRectRenderCount(), 1);
    
    sfmlRenderer->endFrame();
}

// Test multiple rectangle rendering
TEST_F(SFMLRendererTest, MultipleRectangleRendering) {
    sfmlRenderer->beginFrame();
    
    // Render multiple rectangles with different colors
    sfmlRenderer->renderRect(0, 0, 100, 100, 1.0f, 0.0f, 0.0f, 1.0f);     // Red
    sfmlRenderer->renderRect(100, 0, 100, 100, 0.0f, 1.0f, 0.0f, 0.8f);   // Green
    sfmlRenderer->renderRect(200, 0, 100, 100, 0.0f, 0.0f, 1.0f, 0.6f);   // Blue
    sfmlRenderer->renderRect(300, 0, 100, 100, 1.0f, 1.0f, 0.0f, 0.4f);   // Yellow
    
    // Should count all rectangles
    EXPECT_EQ(sfmlRenderer->getRectRenderCount(), 4);
    
    sfmlRenderer->endFrame();
}

// Test mixed rendering
TEST_F(SFMLRendererTest, MixedRendering) {
    int textureHandle = mockResourceManager->loadTexture("mixed_test.png");
    
    sfmlRenderer->beginFrame();
    
    // Render mix of sprites and rectangles
    sfmlRenderer->renderSprite(0, 0, 0, 64, 64, textureHandle);
    sfmlRenderer->renderRect(100, 100, 50, 50, 0.5f, 0.5f, 0.5f, 1.0f);
    sfmlRenderer->renderSprite(200, 200, 1, 32, 32, textureHandle);
    sfmlRenderer->renderRect(300, 300, 75, 75, 1.0f, 0.0f, 1.0f, 0.7f);
    sfmlRenderer->renderSprite(400, 400, 2, 48, 48, textureHandle);
    
    // Should count each type separately
    EXPECT_EQ(sfmlRenderer->getSpriteRenderCount(), 3);
    EXPECT_EQ(sfmlRenderer->getRectRenderCount(), 2);
    
    sfmlRenderer->endFrame();
}

// Test render count reset between frames
TEST_F(SFMLRendererTest, RenderCountResetBetweenFrames) {
    int textureHandle = mockResourceManager->loadTexture("frame_test.png");
    
    // First frame
    sfmlRenderer->beginFrame();
    sfmlRenderer->renderSprite(0, 0, 0, 32, 32, textureHandle);
    sfmlRenderer->renderRect(0, 0, 100, 100, 1.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(sfmlRenderer->getSpriteRenderCount(), 1);
    EXPECT_EQ(sfmlRenderer->getRectRenderCount(), 1);
    sfmlRenderer->endFrame();
    
    // Second frame - counts should reset
    sfmlRenderer->beginFrame();
    EXPECT_EQ(sfmlRenderer->getSpriteRenderCount(), 0);
    EXPECT_EQ(sfmlRenderer->getRectRenderCount(), 0);
    
    sfmlRenderer->renderSprite(100, 100, 0, 64, 64, textureHandle);
    sfmlRenderer->renderSprite(200, 200, 1, 64, 64, textureHandle);
    EXPECT_EQ(sfmlRenderer->getSpriteRenderCount(), 2);
    EXPECT_EQ(sfmlRenderer->getRectRenderCount(), 0);
    sfmlRenderer->endFrame();
}

// Test rendering outside of frame
TEST_F(SFMLRendererTest, RenderingOutsideOfFrame) {
    int textureHandle = mockResourceManager->loadTexture("outside_frame.png");
    
    // Not in frame initially
    EXPECT_FALSE(sfmlRenderer->isInFrame());
    
    // Should handle rendering calls gracefully even outside frame
    EXPECT_NO_THROW(sfmlRenderer->renderSprite(0, 0, 0, 32, 32, textureHandle));
    EXPECT_NO_THROW(sfmlRenderer->renderRect(0, 0, 100, 100, 1.0f, 0.0f, 0.0f, 1.0f));
    
    // Counts might still increment (implementation dependent)
    // But calls should not crash
}

// Test multiple frame cycles
TEST_F(SFMLRendererTest, MultipleFrameCycles) {
    int textureHandle = mockResourceManager->loadTexture("cycle_test.png");
    
    for (int frame = 0; frame < 5; ++frame) {
        sfmlRenderer->beginFrame();
        EXPECT_TRUE(sfmlRenderer->isInFrame());
        
        // Render different amounts each frame
        for (int i = 0; i <= frame; ++i) {
            sfmlRenderer->renderSprite(i * 32, i * 32, i, 32, 32, textureHandle);
            sfmlRenderer->renderRect(i * 50, i * 50, 25, 25, 1.0f, 0.0f, 0.0f, 1.0f);
        }
        
        EXPECT_EQ(sfmlRenderer->getSpriteRenderCount(), frame + 1);
        EXPECT_EQ(sfmlRenderer->getRectRenderCount(), frame + 1);
        
        sfmlRenderer->endFrame();
        EXPECT_FALSE(sfmlRenderer->isInFrame());
    }
}

// Test polymorphic usage through IRenderer interface
TEST_F(SFMLRendererTest, PolymorphicUsage) {
    IRenderer* renderer = sfmlRenderer.get();
    int textureHandle = mockResourceManager->loadTexture("polymorphic_test.png");
    
    // Should work through base interface
    renderer->beginFrame();
    renderer->renderSprite(0, 0, 0, 64, 64, textureHandle);
    renderer->renderRect(100, 100, 80, 80, 0.0f, 1.0f, 0.0f, 1.0f);
    
    int width, height;
    renderer->getScreenSize(width, height);
    EXPECT_EQ(width, 1024);
    EXPECT_EQ(height, 768);
    
    renderer->endFrame();
}

// Test dependency null checking
TEST_F(SFMLRendererTest, DependencyNullChecking) {
    // Test creation with null dependencies
    EXPECT_NO_THROW({
        SFMLRenderer nullResourceRenderer(nullptr, mockWindowManager.get());
        SFMLRenderer nullWindowRenderer(mockResourceManager.get(), nullptr);
        SFMLRenderer nullBothRenderer(nullptr, nullptr);
    });
}

// Test resource manager integration
TEST_F(SFMLRendererTest, ResourceManagerIntegration) {
    // Load multiple textures
    int texture1 = mockResourceManager->loadTexture("integration1.png");
    int texture2 = mockResourceManager->loadTexture("integration2.png");
    int texture3 = mockResourceManager->loadTexture("integration3.png");
    
    EXPECT_NE(texture1, INVALID_TEXTURE);
    EXPECT_NE(texture2, INVALID_TEXTURE);
    EXPECT_NE(texture3, INVALID_TEXTURE);
    
    sfmlRenderer->beginFrame();
    
    // Render with different textures
    sfmlRenderer->renderSprite(0, 0, 0, 32, 32, texture1);
    sfmlRenderer->renderSprite(50, 50, 1, 32, 32, texture2);
    sfmlRenderer->renderSprite(100, 100, 2, 32, 32, texture3);
    
    EXPECT_EQ(sfmlRenderer->getSpriteRenderCount(), 3);
    
    sfmlRenderer->endFrame();
    
    // Verify texture usage was recorded in resource manager
    EXPECT_TRUE(mockResourceManager->isTextureValid(texture1));
    EXPECT_TRUE(mockResourceManager->isTextureValid(texture2));
    EXPECT_TRUE(mockResourceManager->isTextureValid(texture3));
}

// Test window manager integration
TEST_F(SFMLRendererTest, WindowManagerIntegration) {
    // Test with different window configurations
    mockWindowManager->setWindowSize(800, 600);
    
    int width, height;
    sfmlRenderer->getScreenSize(width, height);
    EXPECT_EQ(width, 800);
    EXPECT_EQ(height, 600);
    
    // Test rendering with updated window
    sfmlRenderer->beginFrame();
    EXPECT_NE(sfmlRenderer->getCurrentRenderTarget(), nullptr);
    sfmlRenderer->endFrame();
    
    // Test with closed window
    mockWindowManager->closeWindow();
    sfmlRenderer->beginFrame();
    EXPECT_EQ(sfmlRenderer->getCurrentRenderTarget(), nullptr);
    sfmlRenderer->endFrame();
}

// Test edge cases and robustness
TEST_F(SFMLRendererTest, EdgeCases) {
    int textureHandle = mockResourceManager->loadTexture("edge_test.png");
    
    sfmlRenderer->beginFrame();
    
    // Zero-size sprites and rectangles
    EXPECT_NO_THROW(sfmlRenderer->renderSprite(0, 0, 0, 0, 0, textureHandle));
    EXPECT_NO_THROW(sfmlRenderer->renderRect(0, 0, 0, 0, 1.0f, 1.0f, 1.0f, 1.0f));
    
    // Negative positions and sizes
    EXPECT_NO_THROW(sfmlRenderer->renderSprite(-100, -100, -1, 32, 32, textureHandle));
    EXPECT_NO_THROW(sfmlRenderer->renderRect(-50, -50, 100, 100, 1.0f, 0.0f, 0.0f, 1.0f));
    
    // Very large positions and sizes
    EXPECT_NO_THROW(sfmlRenderer->renderSprite(10000, 10000, 1000, 5000, 5000, textureHandle));
    EXPECT_NO_THROW(sfmlRenderer->renderRect(20000, 20000, 10000, 10000, 0.0f, 1.0f, 0.0f, 1.0f));
    
    // Color values outside normal range
    EXPECT_NO_THROW(sfmlRenderer->renderRect(0, 0, 100, 100, -1.0f, 2.0f, 0.5f, 10.0f));
    
    // Invalid texture handles
    EXPECT_NO_THROW(sfmlRenderer->renderSprite(0, 0, 0, 32, 32, -999));
    EXPECT_NO_THROW(sfmlRenderer->renderSprite(0, 0, 0, 32, 32, 999999));
    
    sfmlRenderer->endFrame();
}

// Test typical usage pattern
TEST_F(SFMLRendererTest, TypicalUsagePattern) {
    // Load game assets
    int playerTexture = mockResourceManager->loadTexture("player.png");
    int enemyTexture = mockResourceManager->loadTexture("enemy.png");
    int backgroundTexture = mockResourceManager->loadTexture("background.png");
    
    // Simulate game loop
    for (int frame = 0; frame < 3; ++frame) {
        sfmlRenderer->beginFrame();
        
        // Render background
        sfmlRenderer->renderSprite(0, 0, -1, 1024, 768, backgroundTexture);
        
        // Render UI rectangles
        sfmlRenderer->renderRect(10, 10, 200, 50, 0.0f, 0.0f, 0.0f, 0.8f); // Health bar background
        sfmlRenderer->renderRect(15, 15, 150, 40, 1.0f, 0.0f, 0.0f, 1.0f); // Health bar
        
        // Render game entities
        sfmlRenderer->renderSprite(100 + frame * 10, 200, 1, 64, 64, playerTexture);
        sfmlRenderer->renderSprite(300, 150, 2, 48, 48, enemyTexture);
        sfmlRenderer->renderSprite(400, 250, 2, 48, 48, enemyTexture);
        
        // Verify frame stats
        EXPECT_EQ(sfmlRenderer->getSpriteRenderCount(), 4); // background + player + 2 enemies
        EXPECT_EQ(sfmlRenderer->getRectRenderCount(), 2);   // 2 UI rectangles
        EXPECT_TRUE(sfmlRenderer->isInFrame());
        
        sfmlRenderer->endFrame();
        EXPECT_FALSE(sfmlRenderer->isInFrame());
    }
    
    // Verify resource usage
    EXPECT_TRUE(mockResourceManager->isTextureValid(playerTexture));
    EXPECT_TRUE(mockResourceManager->isTextureValid(enemyTexture));
    EXPECT_TRUE(mockResourceManager->isTextureValid(backgroundTexture));
    
    // Check final screen size
    int width, height;
    sfmlRenderer->getScreenSize(width, height);
    EXPECT_EQ(width, 1024);
    EXPECT_EQ(height, 768);
}