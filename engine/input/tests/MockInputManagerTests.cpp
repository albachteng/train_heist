#include <gtest/gtest.h>
#include "../include/MockInputManager.hpp"
#include "../../ecs/systems/include/IInputManager.hpp"

using namespace ECS;

/**
 * Test fixture for MockInputManager
 */
class MockInputManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockInputManager = std::make_unique<MockInputManager>();
    }
    
    void TearDown() override {
        mockInputManager.reset();
    }
    
    std::unique_ptr<MockInputManager> mockInputManager;
};

/**
 * Test interface compliance and basic construction
 */
TEST_F(MockInputManagerTest, InterfaceCompliance) {
    // Should be able to use through interface pointer
    std::unique_ptr<IInputManager> inputManager = std::make_unique<MockInputManager>();
    
    // Basic interface calls shouldn't crash
    EXPECT_NO_THROW(inputManager->isKeyPressed(KeyCode::A));
    EXPECT_NO_THROW(inputManager->wasKeyPressed(KeyCode::A));
    EXPECT_NO_THROW(inputManager->wasKeyReleased(KeyCode::A));
    
    int x, y;
    EXPECT_NO_THROW(inputManager->getMousePosition(x, y));
    EXPECT_NO_THROW(inputManager->isMouseButtonPressed(0));
    EXPECT_NO_THROW(inputManager->wasMouseButtonPressed(0));
    EXPECT_NO_THROW(inputManager->update());
}

/**
 * Test initial state - all keys/buttons should be released
 */
TEST_F(MockInputManagerTest, InitialState) {
    // No keys should be pressed initially
    EXPECT_FALSE(mockInputManager->isKeyPressed(KeyCode::A));
    EXPECT_FALSE(mockInputManager->isKeyPressed(KeyCode::Space));
    EXPECT_FALSE(mockInputManager->isKeyPressed(KeyCode::Left));
    
    // No just-pressed/released events initially
    EXPECT_FALSE(mockInputManager->wasKeyPressed(KeyCode::A));
    EXPECT_FALSE(mockInputManager->wasKeyReleased(KeyCode::A));
    
    // No mouse buttons pressed
    EXPECT_FALSE(mockInputManager->isMouseButtonPressed(0));
    EXPECT_FALSE(mockInputManager->isMouseButtonPressed(1));
    EXPECT_FALSE(mockInputManager->isMouseButtonPressed(2));
    
    // No mouse button events
    EXPECT_FALSE(mockInputManager->wasMouseButtonPressed(0));
    
    // Mouse position should be at origin
    int x, y;
    mockInputManager->getMousePosition(x, y);
    EXPECT_EQ(x, 0);
    EXPECT_EQ(y, 0);
    
    // No updates called
    EXPECT_EQ(mockInputManager->getUpdateCount(), 0u);
}

/**
 * Test key simulation - simulateKeyPress
 */
TEST_F(MockInputManagerTest, KeySimulation) {
    // Simulate key press
    mockInputManager->simulateKeyPress(KeyCode::A);
    
    // Should be detected as pressed and just-pressed
    EXPECT_TRUE(mockInputManager->isKeyPressed(KeyCode::A));
    EXPECT_TRUE(mockInputManager->wasKeyPressed(KeyCode::A));
    EXPECT_FALSE(mockInputManager->wasKeyReleased(KeyCode::A));
    
    // Other keys should remain unaffected
    EXPECT_FALSE(mockInputManager->isKeyPressed(KeyCode::B));
    EXPECT_FALSE(mockInputManager->wasKeyPressed(KeyCode::B));
}

/**
 * Test key release simulation
 */
TEST_F(MockInputManagerTest, KeyReleaseSimulation) {
    // First press, then release
    mockInputManager->simulateKeyPress(KeyCode::Space);
    mockInputManager->simulateKeyRelease(KeyCode::Space);
    
    // Should be detected as released and just-released
    EXPECT_FALSE(mockInputManager->isKeyPressed(KeyCode::Space));
    EXPECT_FALSE(mockInputManager->wasKeyPressed(KeyCode::Space));
    EXPECT_TRUE(mockInputManager->wasKeyReleased(KeyCode::Space));
}

/**
 * Test persistent key state setting
 */
TEST_F(MockInputManagerTest, PersistentKeyState) {
    // Set key as pressed (without just-pressed trigger)
    mockInputManager->setKeyPressed(KeyCode::W);
    
    // Should be detected as pressed but not just-pressed
    EXPECT_TRUE(mockInputManager->isKeyPressed(KeyCode::W));
    EXPECT_FALSE(mockInputManager->wasKeyPressed(KeyCode::W));
    EXPECT_FALSE(mockInputManager->wasKeyReleased(KeyCode::W));
    
    // Set key as released
    mockInputManager->setKeyReleased(KeyCode::W);
    
    // Should no longer be pressed
    EXPECT_FALSE(mockInputManager->isKeyPressed(KeyCode::W));
    EXPECT_FALSE(mockInputManager->wasKeyPressed(KeyCode::W));
    EXPECT_FALSE(mockInputManager->wasKeyReleased(KeyCode::W));
}

/**
 * Test mouse button simulation
 */
TEST_F(MockInputManagerTest, MouseButtonSimulation) {
    // Simulate left mouse button press
    mockInputManager->simulateMousePress(0);
    
    // Should be detected as pressed and just-pressed
    EXPECT_TRUE(mockInputManager->isMouseButtonPressed(0));
    EXPECT_TRUE(mockInputManager->wasMouseButtonPressed(0));
    
    // Other buttons should be unaffected
    EXPECT_FALSE(mockInputManager->isMouseButtonPressed(1));
    EXPECT_FALSE(mockInputManager->isMouseButtonPressed(2));
}

/**
 * Test mouse position setting
 */
TEST_F(MockInputManagerTest, MousePositionSetting) {
    // Set mouse position
    mockInputManager->setMousePosition(100, 200);
    
    int x, y;
    mockInputManager->getMousePosition(x, y);
    EXPECT_EQ(x, 100);
    EXPECT_EQ(y, 200);
    
    // Test negative coordinates
    mockInputManager->setMousePosition(-50, -75);
    mockInputManager->getMousePosition(x, y);
    EXPECT_EQ(x, -50);
    EXPECT_EQ(y, -75);
}

/**
 * Test frame-based input state management (update() behavior)
 */
TEST_F(MockInputManagerTest, FrameBasedStateManagement) {
    // Simulate key press
    mockInputManager->simulateKeyPress(KeyCode::Enter);
    EXPECT_TRUE(mockInputManager->wasKeyPressed(KeyCode::Enter));
    
    // After update, just-pressed should be cleared
    mockInputManager->update();
    EXPECT_TRUE(mockInputManager->isKeyPressed(KeyCode::Enter));  // Still pressed
    EXPECT_FALSE(mockInputManager->wasKeyPressed(KeyCode::Enter)); // No longer just-pressed
    EXPECT_EQ(mockInputManager->getUpdateCount(), 1u);
    
    // Simulate release
    mockInputManager->simulateKeyRelease(KeyCode::Enter);
    EXPECT_TRUE(mockInputManager->wasKeyReleased(KeyCode::Enter));
    
    // After update, just-released should be cleared
    mockInputManager->update();
    EXPECT_FALSE(mockInputManager->isKeyPressed(KeyCode::Enter));
    EXPECT_FALSE(mockInputManager->wasKeyReleased(KeyCode::Enter));
    EXPECT_EQ(mockInputManager->getUpdateCount(), 2u);
}

/**
 * Test multiple key handling
 */
TEST_F(MockInputManagerTest, MultipleKeyHandling) {
    // Press multiple keys
    mockInputManager->simulateKeyPress(KeyCode::A);
    mockInputManager->simulateKeyPress(KeyCode::S);
    mockInputManager->simulateKeyPress(KeyCode::D);
    
    // All should be detected
    EXPECT_TRUE(mockInputManager->isKeyPressed(KeyCode::A));
    EXPECT_TRUE(mockInputManager->isKeyPressed(KeyCode::S));
    EXPECT_TRUE(mockInputManager->isKeyPressed(KeyCode::D));
    
    EXPECT_TRUE(mockInputManager->wasKeyPressed(KeyCode::A));
    EXPECT_TRUE(mockInputManager->wasKeyPressed(KeyCode::S));
    EXPECT_TRUE(mockInputManager->wasKeyPressed(KeyCode::D));
    
    // Release one key
    mockInputManager->simulateKeyRelease(KeyCode::S);
    
    // Only S should be released
    EXPECT_TRUE(mockInputManager->isKeyPressed(KeyCode::A));
    EXPECT_FALSE(mockInputManager->isKeyPressed(KeyCode::S));
    EXPECT_TRUE(mockInputManager->isKeyPressed(KeyCode::D));
    
    EXPECT_FALSE(mockInputManager->wasKeyPressed(KeyCode::S));
    EXPECT_TRUE(mockInputManager->wasKeyReleased(KeyCode::S));
}

/**
 * Test edge case key codes
 */
TEST_F(MockInputManagerTest, EdgeCaseKeyCodes) {
    // Test with various key codes
    int testKeys[] = {0, -1, 999, KeyCode::Escape, KeyCode::Up, KeyCode::Down};
    
    for (int key : testKeys) {
        mockInputManager->simulateKeyPress(key);
        EXPECT_TRUE(mockInputManager->isKeyPressed(key));
        EXPECT_TRUE(mockInputManager->wasKeyPressed(key));
        
        mockInputManager->simulateKeyRelease(key);
        EXPECT_FALSE(mockInputManager->isKeyPressed(key));
        EXPECT_TRUE(mockInputManager->wasKeyReleased(key));
    }
}

/**
 * Test mouse button edge cases
 */
TEST_F(MockInputManagerTest, MouseButtonEdgeCases) {
    // Test various mouse button indices
    int testButtons[] = {0, 1, 2, -1, 5};
    
    for (int button : testButtons) {
        mockInputManager->simulateMousePress(button);
        EXPECT_TRUE(mockInputManager->isMouseButtonPressed(button));
        EXPECT_TRUE(mockInputManager->wasMouseButtonPressed(button));
        
        mockInputManager->simulateMouseRelease(button);
        EXPECT_FALSE(mockInputManager->isMouseButtonPressed(button));
    }
}

/**
 * Test reset functionality
 */
TEST_F(MockInputManagerTest, ResetFunctionality) {
    // Set up some input state
    mockInputManager->simulateKeyPress(KeyCode::A);
    mockInputManager->simulateKeyPress(KeyCode::B);
    mockInputManager->simulateMousePress(0);
    mockInputManager->setMousePosition(100, 200);
    mockInputManager->update();
    
    // Verify state is set
    EXPECT_TRUE(mockInputManager->isKeyPressed(KeyCode::A));
    EXPECT_TRUE(mockInputManager->isMouseButtonPressed(0));
    EXPECT_EQ(mockInputManager->getUpdateCount(), 1u);
    
    // Reset everything
    mockInputManager->reset();
    
    // All state should be cleared
    EXPECT_FALSE(mockInputManager->isKeyPressed(KeyCode::A));
    EXPECT_FALSE(mockInputManager->isKeyPressed(KeyCode::B));
    EXPECT_FALSE(mockInputManager->wasKeyPressed(KeyCode::A));
    EXPECT_FALSE(mockInputManager->wasKeyReleased(KeyCode::A));
    
    EXPECT_FALSE(mockInputManager->isMouseButtonPressed(0));
    EXPECT_FALSE(mockInputManager->wasMouseButtonPressed(0));
    
    int x, y;
    mockInputManager->getMousePosition(x, y);
    EXPECT_EQ(x, 0);
    EXPECT_EQ(y, 0);
    
    EXPECT_EQ(mockInputManager->getUpdateCount(), 0u);
}

/**
 * Test complex input sequence
 */
TEST_F(MockInputManagerTest, ComplexInputSequence) {
    // Frame 1: Press A
    mockInputManager->simulateKeyPress(KeyCode::A);
    EXPECT_TRUE(mockInputManager->wasKeyPressed(KeyCode::A));
    mockInputManager->update();
    
    // Frame 2: A still pressed, press B
    EXPECT_TRUE(mockInputManager->isKeyPressed(KeyCode::A));
    EXPECT_FALSE(mockInputManager->wasKeyPressed(KeyCode::A));
    
    mockInputManager->simulateKeyPress(KeyCode::B);
    EXPECT_TRUE(mockInputManager->wasKeyPressed(KeyCode::B));
    mockInputManager->update();
    
    // Frame 3: Both pressed, release A
    EXPECT_TRUE(mockInputManager->isKeyPressed(KeyCode::A));
    EXPECT_TRUE(mockInputManager->isKeyPressed(KeyCode::B));
    EXPECT_FALSE(mockInputManager->wasKeyPressed(KeyCode::A));
    EXPECT_FALSE(mockInputManager->wasKeyPressed(KeyCode::B));
    
    mockInputManager->simulateKeyRelease(KeyCode::A);
    EXPECT_TRUE(mockInputManager->wasKeyReleased(KeyCode::A));
    mockInputManager->update();
    
    // Frame 4: Only B pressed
    EXPECT_FALSE(mockInputManager->isKeyPressed(KeyCode::A));
    EXPECT_TRUE(mockInputManager->isKeyPressed(KeyCode::B));
    EXPECT_FALSE(mockInputManager->wasKeyReleased(KeyCode::A));
}

/**
 * Test polymorphic usage through IInputManager interface
 */
TEST_F(MockInputManagerTest, PolymorphicUsage) {
    std::unique_ptr<IInputManager> inputManager = std::make_unique<MockInputManager>();
    
    // Should be able to call all interface methods
    EXPECT_FALSE(inputManager->isKeyPressed(KeyCode::Space));
    EXPECT_FALSE(inputManager->wasKeyPressed(KeyCode::Space));
    EXPECT_FALSE(inputManager->wasKeyReleased(KeyCode::Space));
    
    int x, y;
    inputManager->getMousePosition(x, y);
    EXPECT_EQ(x, 0);
    EXPECT_EQ(y, 0);
    
    EXPECT_FALSE(inputManager->isMouseButtonPressed(0));
    EXPECT_FALSE(inputManager->wasMouseButtonPressed(0));
    
    EXPECT_NO_THROW(inputManager->update());
}