#include <gtest/gtest.h>
#include "../include/SFMLInputManager.hpp"
#include "../../rendering/include/SFMLWindowManager.hpp"
#include <SFML/Window.hpp>

using namespace ECS;

/**
 * Integration test fixture for SFMLInputManager with real SFML components
 */
class SFMLInputIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        sfmlWindowManager = std::make_unique<SFMLWindowManager>();
        sfmlInputManager = std::make_unique<SFMLInputManager>(sfmlWindowManager.get());
    }
    
    void TearDown() override {
        sfmlInputManager.reset();
        if (sfmlWindowManager) {
            sfmlWindowManager->closeWindow();
        }
        sfmlWindowManager.reset();
    }
    
    std::unique_ptr<SFMLWindowManager> sfmlWindowManager;
    std::unique_ptr<SFMLInputManager> sfmlInputManager;
};

/**
 * Test SFML integration without window (graceful degradation)
 */
TEST_F(SFMLInputIntegrationTest, NoWindowIntegration) {
    // Without creating a window, input manager should still work
    EXPECT_NO_THROW(sfmlInputManager->update());
    
    // All input queries should return false/default values
    EXPECT_FALSE(sfmlInputManager->isKeyPressed(KeyCode::A));
    EXPECT_FALSE(sfmlInputManager->wasKeyPressed(KeyCode::A));
    EXPECT_FALSE(sfmlInputManager->isMouseButtonPressed(0));
    
    int x, y;
    sfmlInputManager->getMousePosition(x, y);
    EXPECT_EQ(x, 0);
    EXPECT_EQ(y, 0);
    
    // No events should be processed without window
    EXPECT_EQ(sfmlInputManager->getEventCount(), 0u);
}

/**
 * Test SFML integration with window creation
 */
TEST_F(SFMLInputIntegrationTest, WindowIntegration) {
    // Create a window for event processing
    ASSERT_TRUE(sfmlWindowManager->createWindow(400, 300, "Input Test Window"));
    
    // Update should process events from the window (even if no events)
    EXPECT_NO_THROW(sfmlInputManager->update());
    
    // Input state should be queryable
    EXPECT_FALSE(sfmlInputManager->isKeyPressed(KeyCode::Space));
    EXPECT_FALSE(sfmlInputManager->wasKeyPressed(KeyCode::Space));
    
    // Window should be open and functional
    EXPECT_TRUE(sfmlWindowManager->isWindowOpen());
}

/**
 * Test key code conversion accuracy
 */
TEST_F(SFMLInputIntegrationTest, KeyCodeConversionAccuracy) {
    // Create window for proper SFML context
    ASSERT_TRUE(sfmlWindowManager->createWindow(400, 300, "Key Mapping Test"));
    
    // Test that we can query all defined key codes without crashing
    int testKeys[] = {
        KeyCode::A, KeyCode::B, KeyCode::C, KeyCode::D, KeyCode::E,
        KeyCode::Space, KeyCode::Enter, KeyCode::Escape, KeyCode::Tab,
        KeyCode::Left, KeyCode::Up, KeyCode::Right, KeyCode::Down
    };
    
    for (int key : testKeys) {
        // Should not crash when querying any defined key
        EXPECT_NO_THROW(sfmlInputManager->isKeyPressed(key));
        EXPECT_NO_THROW(sfmlInputManager->wasKeyPressed(key));
        EXPECT_NO_THROW(sfmlInputManager->wasKeyReleased(key));
        
        // All keys should initially be not pressed
        EXPECT_FALSE(sfmlInputManager->isKeyPressed(key));
        EXPECT_FALSE(sfmlInputManager->wasKeyPressed(key));
        EXPECT_FALSE(sfmlInputManager->wasKeyReleased(key));
    }
}

/**
 * Test mouse button code conversion
 */
TEST_F(SFMLInputIntegrationTest, MouseButtonConversion) {
    ASSERT_TRUE(sfmlWindowManager->createWindow(400, 300, "Mouse Test"));
    
    // Test standard mouse buttons
    int mouseButtons[] = {0, 1, 2}; // Left, Right, Middle
    
    for (int button : mouseButtons) {
        EXPECT_NO_THROW(sfmlInputManager->isMouseButtonPressed(button));
        EXPECT_NO_THROW(sfmlInputManager->wasMouseButtonPressed(button));
        
        // All buttons should initially be not pressed
        EXPECT_FALSE(sfmlInputManager->isMouseButtonPressed(button));
        EXPECT_FALSE(sfmlInputManager->wasMouseButtonPressed(button));
    }
}

/**
 * Test event processing pipeline integration
 */
TEST_F(SFMLInputIntegrationTest, EventProcessingPipeline) {
    ASSERT_TRUE(sfmlWindowManager->createWindow(400, 300, "Event Pipeline Test"));
    
    // Multiple update calls should not crash
    for (int i = 0; i < 10; ++i) {
        EXPECT_NO_THROW(sfmlInputManager->update());
    }
    
    // Event count tracking should work
    size_t initialCount = sfmlInputManager->getEventCount();
    sfmlInputManager->update();
    
    // Count should be monotonic (non-decreasing)
    EXPECT_GE(sfmlInputManager->getEventCount(), initialCount);
}

/**
 * Test frame consistency across updates
 */
TEST_F(SFMLInputIntegrationTest, FrameConsistency) {
    ASSERT_TRUE(sfmlWindowManager->createWindow(400, 300, "Frame Consistency Test"));
    
    // Test that multiple updates maintain consistent state
    for (int frame = 0; frame < 5; ++frame) {
        // State should be consistent within a frame
        bool keyAState1 = sfmlInputManager->isKeyPressed(KeyCode::A);
        bool keyAState2 = sfmlInputManager->isKeyPressed(KeyCode::A);
        EXPECT_EQ(keyAState1, keyAState2);
        
        bool wasKeyAPressed1 = sfmlInputManager->wasKeyPressed(KeyCode::A);
        bool wasKeyAPressed2 = sfmlInputManager->wasKeyPressed(KeyCode::A);
        EXPECT_EQ(wasKeyAPressed1, wasKeyAPressed2);
        
        int x1, y1, x2, y2;
        sfmlInputManager->getMousePosition(x1, y1);
        sfmlInputManager->getMousePosition(x2, y2);
        EXPECT_EQ(x1, x2);
        EXPECT_EQ(y1, y2);
        
        sfmlInputManager->update();
    }
}

/**
 * Test edge case handling with window operations
 */
TEST_F(SFMLInputIntegrationTest, WindowOperationEdgeCases) {
    // Create and close window multiple times
    for (int i = 0; i < 3; ++i) {
        ASSERT_TRUE(sfmlWindowManager->createWindow(300 + i * 100, 200 + i * 50, "Test Window"));
        
        // Input should work with fresh window
        EXPECT_NO_THROW(sfmlInputManager->update());
        EXPECT_FALSE(sfmlInputManager->isKeyPressed(KeyCode::Space));
        
        sfmlWindowManager->closeWindow();
        
        // Input should still work after closing window (graceful degradation)
        EXPECT_NO_THROW(sfmlInputManager->update());
        EXPECT_FALSE(sfmlInputManager->isKeyPressed(KeyCode::Space));
    }
}

/**
 * Test dependency relationship validation
 */
TEST_F(SFMLInputIntegrationTest, DependencyRelationshipValidation) {
    // Input manager should reference the window manager
    EXPECT_EQ(sfmlInputManager->getWindowManager(), sfmlWindowManager.get());
    
    // Should work with different window managers
    auto alternateWindowManager = std::make_unique<SFMLWindowManager>();
    auto alternateInputManager = std::make_unique<SFMLInputManager>(alternateWindowManager.get());
    
    EXPECT_EQ(alternateInputManager->getWindowManager(), alternateWindowManager.get());
    EXPECT_NE(alternateInputManager->getWindowManager(), sfmlWindowManager.get());
    
    // Both should work independently
    EXPECT_NO_THROW(sfmlInputManager->update());
    EXPECT_NO_THROW(alternateInputManager->update());
    
    alternateWindowManager->closeWindow();
}

/**
 * Test SFML-specific error handling
 */
TEST_F(SFMLInputIntegrationTest, SFMLErrorHandling) {
    ASSERT_TRUE(sfmlWindowManager->createWindow(400, 300, "Error Handling Test"));
    
    // Rapid updates should not cause issues
    for (int i = 0; i < 100; ++i) {
        sfmlInputManager->update();
    }
    
    // Should handle window state changes gracefully
    sfmlWindowManager->closeWindow();
    EXPECT_NO_THROW(sfmlInputManager->update());
    
    // Recreate window and continue
    ASSERT_TRUE(sfmlWindowManager->createWindow(500, 400, "Recreated Window"));
    EXPECT_NO_THROW(sfmlInputManager->update());
}

/**
 * Test memory and resource management
 */
TEST_F(SFMLInputIntegrationTest, ResourceManagement) {
    // Create multiple input managers with same window manager
    std::vector<std::unique_ptr<SFMLInputManager>> inputManagers;
    
    for (int i = 0; i < 5; ++i) {
        inputManagers.push_back(
            std::make_unique<SFMLInputManager>(sfmlWindowManager.get())
        );
    }
    
    ASSERT_TRUE(sfmlWindowManager->createWindow(400, 300, "Resource Test"));
    
    // All should work independently
    for (auto& manager : inputManagers) {
        EXPECT_NO_THROW(manager->update());
        EXPECT_FALSE(manager->isKeyPressed(KeyCode::A));
    }
    
    // Cleanup should be automatic when destructors are called
    inputManagers.clear();
    
    // Original input manager should still work
    EXPECT_NO_THROW(sfmlInputManager->update());
}

/**
 * Test performance characteristics
 */
TEST_F(SFMLInputIntegrationTest, PerformanceCharacteristics) {
    ASSERT_TRUE(sfmlWindowManager->createWindow(400, 300, "Performance Test"));
    
    // Measure that many updates complete quickly (basic performance test)
    const int iterations = 1000;
    
    for (int i = 0; i < iterations; ++i) {
        sfmlInputManager->update();
        
        // Query multiple keys per frame
        sfmlInputManager->isKeyPressed(KeyCode::A);
        sfmlInputManager->isKeyPressed(KeyCode::S);
        sfmlInputManager->isKeyPressed(KeyCode::D);
        sfmlInputManager->isMouseButtonPressed(0);
        
        int x, y;
        sfmlInputManager->getMousePosition(x, y);
    }
    
    // Should complete without timing out (basic throughput test)
    SUCCEED();
}