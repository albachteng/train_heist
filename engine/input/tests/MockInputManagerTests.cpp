#include "MockInputManager.hpp"
#include <gtest/gtest.h>
#include <memory>

class MockInputManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockInput = std::make_unique<ECS::MockInputManager>();
    }
    
    void TearDown() override {
        mockInput.reset();
    }
    
    std::unique_ptr<ECS::MockInputManager> mockInput;
};

// Test basic key press functionality
TEST_F(MockInputManagerTest, BasicKeyPress) {
    EXPECT_FALSE(mockInput->isKeyPressed(ECS::Keys::A));
    EXPECT_FALSE(mockInput->wasKeyJustPressed(ECS::Keys::A));
    
    mockInput->pressKey(ECS::Keys::A);
    
    EXPECT_TRUE(mockInput->isKeyPressed(ECS::Keys::A));
    EXPECT_TRUE(mockInput->wasKeyJustPressed(ECS::Keys::A));
}

// Test key release functionality
TEST_F(MockInputManagerTest, KeyRelease) {
    mockInput->pressKey(ECS::Keys::A);
    EXPECT_TRUE(mockInput->isKeyPressed(ECS::Keys::A));
    
    mockInput->releaseKey(ECS::Keys::A);
    
    EXPECT_FALSE(mockInput->isKeyPressed(ECS::Keys::A));
    EXPECT_TRUE(mockInput->wasKeyJustReleased(ECS::Keys::A));
}

// Test update clears just pressed/released states
TEST_F(MockInputManagerTest, UpdateClearsJustStates) {
    mockInput->pressKey(ECS::Keys::A);
    EXPECT_TRUE(mockInput->wasKeyJustPressed(ECS::Keys::A));
    
    mockInput->update();
    
    EXPECT_TRUE(mockInput->isKeyPressed(ECS::Keys::A)); // Still pressed
    EXPECT_FALSE(mockInput->wasKeyJustPressed(ECS::Keys::A)); // Just pressed cleared
}

// Test mouse button functionality
TEST_F(MockInputManagerTest, MouseButtons) {
    EXPECT_FALSE(mockInput->isMouseButtonPressed(ECS::MouseButtons::Left));
    
    mockInput->pressMouseButton(ECS::MouseButtons::Left);
    
    EXPECT_TRUE(mockInput->isMouseButtonPressed(ECS::MouseButtons::Left));
    EXPECT_TRUE(mockInput->wasMouseButtonJustPressed(ECS::MouseButtons::Left));
    
    mockInput->releaseMouseButton(ECS::MouseButtons::Left);
    
    EXPECT_FALSE(mockInput->isMouseButtonPressed(ECS::MouseButtons::Left));
    EXPECT_TRUE(mockInput->wasMouseButtonJustReleased(ECS::MouseButtons::Left));
}

// Test mouse position
TEST_F(MockInputManagerTest, MousePosition) {
    int x, y;
    mockInput->getMousePosition(x, y);
    EXPECT_EQ(x, 0);
    EXPECT_EQ(y, 0);
    
    mockInput->setMousePosition(100, 200);
    mockInput->getMousePosition(x, y);
    EXPECT_EQ(x, 100);
    EXPECT_EQ(y, 200);
}

// Test reset functionality
TEST_F(MockInputManagerTest, Reset) {
    mockInput->pressKey(ECS::Keys::A);
    mockInput->pressMouseButton(ECS::MouseButtons::Left);
    mockInput->setMousePosition(100, 200);
    
    mockInput->reset();
    
    EXPECT_FALSE(mockInput->isKeyPressed(ECS::Keys::A));
    EXPECT_FALSE(mockInput->wasKeyJustPressed(ECS::Keys::A));
    EXPECT_FALSE(mockInput->isMouseButtonPressed(ECS::MouseButtons::Left));
    
    int x, y;
    mockInput->getMousePosition(x, y);
    EXPECT_EQ(x, 0);
    EXPECT_EQ(y, 0);
}

// Test simulate key press cycle
TEST_F(MockInputManagerTest, SimulateKeyPress) {
    mockInput->simulateKeyPress(ECS::Keys::A);
    
    // After simulation, key should no longer be pressed
    EXPECT_FALSE(mockInput->isKeyPressed(ECS::Keys::A));
    EXPECT_FALSE(mockInput->wasKeyJustPressed(ECS::Keys::A));
    EXPECT_FALSE(mockInput->wasKeyJustReleased(ECS::Keys::A));
}

// Test multiple keys
TEST_F(MockInputManagerTest, MultipleKeys) {
    mockInput->pressKey(ECS::Keys::A);
    mockInput->pressKey(ECS::Keys::B);
    mockInput->pressKey(ECS::Keys::Up);
    
    EXPECT_TRUE(mockInput->isKeyPressed(ECS::Keys::A));
    EXPECT_TRUE(mockInput->isKeyPressed(ECS::Keys::B));
    EXPECT_TRUE(mockInput->isKeyPressed(ECS::Keys::Up));
    EXPECT_FALSE(mockInput->isKeyPressed(ECS::Keys::Down));
}

// Test key state transitions
TEST_F(MockInputManagerTest, KeyStateTransitions) {
    // Initial state
    EXPECT_FALSE(mockInput->isKeyPressed(ECS::Keys::A));
    EXPECT_FALSE(mockInput->wasKeyJustPressed(ECS::Keys::A));
    EXPECT_FALSE(mockInput->wasKeyJustReleased(ECS::Keys::A));
    
    // Press key
    mockInput->pressKey(ECS::Keys::A);
    EXPECT_TRUE(mockInput->isKeyPressed(ECS::Keys::A));
    EXPECT_TRUE(mockInput->wasKeyJustPressed(ECS::Keys::A));
    EXPECT_FALSE(mockInput->wasKeyJustReleased(ECS::Keys::A));
    
    // After update (key still held)
    mockInput->update();
    EXPECT_TRUE(mockInput->isKeyPressed(ECS::Keys::A));
    EXPECT_FALSE(mockInput->wasKeyJustPressed(ECS::Keys::A));
    EXPECT_FALSE(mockInput->wasKeyJustReleased(ECS::Keys::A));
    
    // Release key
    mockInput->releaseKey(ECS::Keys::A);
    EXPECT_FALSE(mockInput->isKeyPressed(ECS::Keys::A));
    EXPECT_FALSE(mockInput->wasKeyJustPressed(ECS::Keys::A));
    EXPECT_TRUE(mockInput->wasKeyJustReleased(ECS::Keys::A));
    
    // After update (key no longer pressed)
    mockInput->update();
    EXPECT_FALSE(mockInput->isKeyPressed(ECS::Keys::A));
    EXPECT_FALSE(mockInput->wasKeyJustPressed(ECS::Keys::A));
    EXPECT_FALSE(mockInput->wasKeyJustReleased(ECS::Keys::A));
}

// Test pressing already pressed key
TEST_F(MockInputManagerTest, PressAlreadyPressedKey) {
    mockInput->pressKey(ECS::Keys::A);
    EXPECT_TRUE(mockInput->wasKeyJustPressed(ECS::Keys::A));
    
    // Update to clear just pressed
    mockInput->update();
    EXPECT_FALSE(mockInput->wasKeyJustPressed(ECS::Keys::A));
    
    // Press again (should not trigger just pressed since already held)
    mockInput->pressKey(ECS::Keys::A);
    EXPECT_FALSE(mockInput->wasKeyJustPressed(ECS::Keys::A));
    EXPECT_TRUE(mockInput->isKeyPressed(ECS::Keys::A));
}

// Test releasing non-pressed key
TEST_F(MockInputManagerTest, ReleaseNonPressedKey) {
    EXPECT_FALSE(mockInput->isKeyPressed(ECS::Keys::A));
    
    // Try to release key that wasn't pressed
    mockInput->releaseKey(ECS::Keys::A);
    
    EXPECT_FALSE(mockInput->wasKeyJustReleased(ECS::Keys::A));
    EXPECT_FALSE(mockInput->isKeyPressed(ECS::Keys::A));
}