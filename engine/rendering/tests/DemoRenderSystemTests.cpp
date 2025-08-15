#include "../../../game/systems/DemoRenderSystem.hpp"
#include "MockRenderer.hpp"
#include "../../ecs/include/EntityManager.hpp"
#include <gtest/gtest.h>
#include <memory>

class DemoRenderSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockRenderer = std::make_unique<ECS::MockRenderer>();
        demoSystem = std::make_unique<DemoRenderSystem>(mockRenderer.get());
        entityManager = std::make_unique<ECS::EntityManager>();
    }
    
    void TearDown() override {
        demoSystem.reset();
        mockRenderer.reset();
        entityManager.reset();
    }
    
    std::unique_ptr<ECS::MockRenderer> mockRenderer;
    std::unique_ptr<DemoRenderSystem> demoSystem;
    std::unique_ptr<ECS::EntityManager> entityManager;
};

// Test basic system properties
TEST_F(DemoRenderSystemTest, SystemProperties) {
    // Test ISystem interface implementation
    EXPECT_EQ(demoSystem->getPriority(), 100);
    EXPECT_EQ(demoSystem->getRequiredComponents(), 0); // No components required
    EXPECT_TRUE(demoSystem->shouldUpdate(0.016f)); // Should always update
}

// Test that system calls renderer methods in correct sequence
TEST_F(DemoRenderSystemTest, RenderingSequence) {
    // Execute one frame
    demoSystem->update(0.016f, *entityManager);
    
    // Verify frame sequence
    EXPECT_TRUE(mockRenderer->wasMethodCalled("beginFrame"));
    EXPECT_TRUE(mockRenderer->wasMethodCalled("clear"));
    EXPECT_TRUE(mockRenderer->wasMethodCalled("endFrame"));
    
    // Verify sequence order (beginFrame -> clear -> renders -> endFrame)
    const auto& calls = mockRenderer->methodCalls;
    ASSERT_GE(calls.size(), 3);
    EXPECT_EQ(calls[0], "beginFrame");
    EXPECT_EQ(calls[1], "clear");
    EXPECT_EQ(calls.back(), "endFrame");
}

// Test that system renders expected number of objects
TEST_F(DemoRenderSystemTest, RendersExpectedObjects) {
    demoSystem->update(0.016f, *entityManager);
    
    // DemoRenderSystem should render:
    // - 3 rectangles (center, animated, color-changing)
    // - 1 sprite (demo sprite)
    EXPECT_EQ(mockRenderer->rectCalls.size(), 3);
    EXPECT_EQ(mockRenderer->spriteCalls.size(), 1);
}

// Test center rectangle properties
TEST_F(DemoRenderSystemTest, CenterRectangleProperties) {
    demoSystem->update(0.016f, *entityManager);
    
    ASSERT_EQ(mockRenderer->rectCalls.size(), 3);
    
    // Center rectangle should be green (0, 1, 0) and positioned at screen center
    const auto& centerRect = mockRenderer->rectCalls[0];
    EXPECT_FLOAT_EQ(centerRect.x, 350.0f); // (800/2) - 50
    EXPECT_FLOAT_EQ(centerRect.y, 250.0f); // (600/2) - 50
    EXPECT_FLOAT_EQ(centerRect.width, 100.0f);
    EXPECT_FLOAT_EQ(centerRect.height, 100.0f);
    EXPECT_FLOAT_EQ(centerRect.red, 0.0f);
    EXPECT_FLOAT_EQ(centerRect.green, 1.0f);
    EXPECT_FLOAT_EQ(centerRect.blue, 0.0f);
    EXPECT_FLOAT_EQ(centerRect.alpha, 1.0f);
}

// Test animated rectangle moves over time
TEST_F(DemoRenderSystemTest, AnimatedRectangleMovement) {
    // Capture position at time 0
    demoSystem->update(0.0f, *entityManager);
    ASSERT_EQ(mockRenderer->rectCalls.size(), 3);
    float x1 = mockRenderer->rectCalls[1].x;
    float y1 = mockRenderer->rectCalls[1].y;
    
    mockRenderer->reset();
    
    // Capture position after some time
    demoSystem->update(1.0f, *entityManager); // 1 second delta
    ASSERT_EQ(mockRenderer->rectCalls.size(), 3);
    float x2 = mockRenderer->rectCalls[1].x;
    float y2 = mockRenderer->rectCalls[1].y;
    
    // Position should have changed (animated)
    EXPECT_NE(x1, x2);
    EXPECT_NE(y1, y2);
    
    // Should be red (1, 0, 0)
    const auto& animRect = mockRenderer->rectCalls[1];
    EXPECT_FLOAT_EQ(animRect.red, 1.0f);
    EXPECT_FLOAT_EQ(animRect.green, 0.0f);
    EXPECT_FLOAT_EQ(animRect.blue, 0.0f);
}

// Test color-changing rectangle
TEST_F(DemoRenderSystemTest, ColorChangingRectangle) {
    demoSystem->update(0.0f, *entityManager);
    ASSERT_EQ(mockRenderer->rectCalls.size(), 3);
    
    const auto& colorRect = mockRenderer->rectCalls[2];
    
    // Should be positioned in corner
    EXPECT_FLOAT_EQ(colorRect.x, 50.0f);
    EXPECT_FLOAT_EQ(colorRect.y, 50.0f);
    EXPECT_FLOAT_EQ(colorRect.width, 80.0f);
    EXPECT_FLOAT_EQ(colorRect.height, 80.0f);
    
    // Color should be some blend of purple-blue
    EXPECT_GE(colorRect.red, 0.0f);
    EXPECT_LE(colorRect.red, 1.0f);
    EXPECT_FLOAT_EQ(colorRect.green, 0.0f);
    EXPECT_GE(colorRect.blue, 0.0f);
    EXPECT_LE(colorRect.blue, 1.0f);
}

// Test sprite rendering
TEST_F(DemoRenderSystemTest, SpriteRendering) {
    demoSystem->update(0.016f, *entityManager);
    
    ASSERT_EQ(mockRenderer->spriteCalls.size(), 1);
    const auto& sprite = mockRenderer->spriteCalls[0];
    
    // Should be positioned in bottom-right
    EXPECT_FLOAT_EQ(sprite.x, 650.0f); // 800 - 150
    EXPECT_FLOAT_EQ(sprite.y, 450.0f); // 600 - 150
    EXPECT_FLOAT_EQ(sprite.z, 0.0f);
    EXPECT_FLOAT_EQ(sprite.width, 100.0f);
    EXPECT_FLOAT_EQ(sprite.height, 100.0f);
    EXPECT_EQ(sprite.textureId, 42);
}

// Test system handles different screen sizes
TEST_F(DemoRenderSystemTest, DifferentScreenSizes) {
    // Change mock screen size
    mockRenderer->screenWidth = 1024;
    mockRenderer->screenHeight = 768;
    
    demoSystem->update(0.016f, *entityManager);
    
    ASSERT_EQ(mockRenderer->rectCalls.size(), 3);
    
    // Center rectangle should adjust to new screen size
    const auto& centerRect = mockRenderer->rectCalls[0];
    EXPECT_FLOAT_EQ(centerRect.x, 462.0f); // (1024/2) - 50
    EXPECT_FLOAT_EQ(centerRect.y, 334.0f); // (768/2) - 50
}

// Test frame rate independence (time-based animation)
TEST_F(DemoRenderSystemTest, TimeBasedAnimation) {
    // Test with different delta times
    float deltaTime1 = 0.016f; // 60 FPS
    float deltaTime2 = 0.033f; // 30 FPS
    
    // Both should produce similar animation progression after same total time
    demoSystem->update(deltaTime1, *entityManager);
    mockRenderer->reset();
    demoSystem->update(deltaTime1, *entityManager); // Total: 0.032s
    float x_60fps = mockRenderer->rectCalls[1].x;
    
    // Reset system by creating new instance
    demoSystem = std::make_unique<DemoRenderSystem>(mockRenderer.get());
    mockRenderer->reset();
    
    demoSystem->update(deltaTime2, *entityManager); // Total: 0.033s
    float x_30fps = mockRenderer->rectCalls[1].x;
    
    // Positions should be very close (time-based, not frame-based)
    EXPECT_NEAR(x_60fps, x_30fps, 5.0f); // Allow small difference due to rounding
}

// Test system doesn't crash with zero delta time
TEST_F(DemoRenderSystemTest, ZeroDeltaTime) {
    EXPECT_NO_THROW(demoSystem->update(0.0f, *entityManager));
    
    // Should still render everything
    EXPECT_EQ(mockRenderer->rectCalls.size(), 3);
    EXPECT_EQ(mockRenderer->spriteCalls.size(), 1);
}

// Test multiple consecutive updates work correctly  
TEST_F(DemoRenderSystemTest, ConsecutiveUpdates) {
    // Multiple updates should all work without issues
    for (int i = 0; i < 5; ++i) {
        mockRenderer->reset();
        demoSystem->update(0.016f, *entityManager);
        
        EXPECT_EQ(mockRenderer->rectCalls.size(), 3);
        EXPECT_EQ(mockRenderer->spriteCalls.size(), 1);
        EXPECT_TRUE(mockRenderer->wasMethodCalled("beginFrame"));
        EXPECT_TRUE(mockRenderer->wasMethodCalled("endFrame"));
    }
}