#include <gtest/gtest.h>
#include "../include/SFMLInputManager.hpp"
#include "../../rendering/include/MockWindowManager.hpp"
#include "../../ecs/systems/include/IInputManager.hpp"
#include <SFML/Window.hpp>

using namespace ECS;

/**
 * Test fixture for SFMLInputManager
 */
class SFMLInputManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockWindowManager = std::make_unique<MockWindowManager>();
        sfmlInputManager = std::make_unique<SFMLInputManager>(mockWindowManager.get());
    }
    
    void TearDown() override {
        sfmlInputManager.reset();
        mockWindowManager.reset();
    }
    
    std::unique_ptr<MockWindowManager> mockWindowManager;
    std::unique_ptr<SFMLInputManager> sfmlInputManager;
};

/**
 * Test interface compliance and basic construction
 */
TEST_F(SFMLInputManagerTest, InterfaceCompliance) {
    // Should be able to use through interface pointer
    std::unique_ptr<IInputManager> inputManager = std::make_unique<SFMLInputManager>(mockWindowManager.get());
    
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
 * Test dependency injection
 */
TEST_F(SFMLInputManagerTest, DependencyInjection) {
    // Should store the provided window manager
    EXPECT_EQ(sfmlInputManager->getWindowManager(), mockWindowManager.get());
    
    // Should work with nullptr (graceful degradation)
    auto nullInputManager = std::make_unique<SFMLInputManager>(nullptr);
    EXPECT_EQ(nullInputManager->getWindowManager(), nullptr);
    
    // Should not crash when calling methods with null window manager
    EXPECT_NO_THROW(nullInputManager->update());
    EXPECT_FALSE(nullInputManager->isKeyPressed(KeyCode::A));
}

/**
 * Test initial state
 */
TEST_F(SFMLInputManagerTest, InitialState) {
    // No keys should be pressed initially
    EXPECT_FALSE(sfmlInputManager->isKeyPressed(KeyCode::A));
    EXPECT_FALSE(sfmlInputManager->isKeyPressed(KeyCode::Space));
    EXPECT_FALSE(sfmlInputManager->isKeyPressed(KeyCode::Left));
    
    // No just-pressed/released events initially
    EXPECT_FALSE(sfmlInputManager->wasKeyPressed(KeyCode::A));
    EXPECT_FALSE(sfmlInputManager->wasKeyReleased(KeyCode::A));
    
    // No mouse buttons pressed
    EXPECT_FALSE(sfmlInputManager->isMouseButtonPressed(0));
    EXPECT_FALSE(sfmlInputManager->isMouseButtonPressed(1));
    EXPECT_FALSE(sfmlInputManager->isMouseButtonPressed(2));
    
    // No mouse button events
    EXPECT_FALSE(sfmlInputManager->wasMouseButtonPressed(0));
    
    // Mouse position should be at origin
    int x, y;
    sfmlInputManager->getMousePosition(x, y);
    EXPECT_EQ(x, 0);
    EXPECT_EQ(y, 0);
    
    // No events processed
    EXPECT_EQ(sfmlInputManager->getEventCount(), 0u);
}

/**
 * Test event processing with key press events
 */
TEST_F(SFMLInputManagerTest, KeyPressEventProcessing) {
    // Simulate key press event from window manager
    WindowEvent keyPressEvent;
    keyPressEvent.type = WindowEventType::KeyPressed;
    keyPressEvent.keyCode = static_cast<int>(sf::Keyboard::Key::A); // SFML key code for 'A'
    
    mockWindowManager->addEvent(keyPressEvent);
    
    // Update should process the event
    sfmlInputManager->update();
    
    // Key should be detected as pressed and just-pressed
    EXPECT_TRUE(sfmlInputManager->isKeyPressed(KeyCode::A));
    EXPECT_TRUE(sfmlInputManager->wasKeyPressed(KeyCode::A));
    EXPECT_FALSE(sfmlInputManager->wasKeyReleased(KeyCode::A));
    
    // Event should be counted
    EXPECT_EQ(sfmlInputManager->getEventCount(), 1u);
}

/**
 * Test event processing with key release events
 */
TEST_F(SFMLInputManagerTest, KeyReleaseEventProcessing) {
    // First simulate a press to have something to release
    WindowEvent keyPressEvent;
    keyPressEvent.type = WindowEventType::KeyPressed;
    keyPressEvent.keyCode = static_cast<int>(sf::Keyboard::Key::Space); // SFML Space key
    mockWindowManager->addEvent(keyPressEvent);
    sfmlInputManager->update();
    
    // Clear just-pressed state
    sfmlInputManager->update();
    
    // Now simulate release
    WindowEvent keyReleaseEvent;
    keyReleaseEvent.type = WindowEventType::KeyReleased;
    keyReleaseEvent.keyCode = static_cast<int>(sf::Keyboard::Key::Space); // SFML Space key
    mockWindowManager->addEvent(keyReleaseEvent);
    
    sfmlInputManager->update();
    
    // Key should be detected as released and just-released
    EXPECT_FALSE(sfmlInputManager->isKeyPressed(KeyCode::Space));
    EXPECT_FALSE(sfmlInputManager->wasKeyPressed(KeyCode::Space));
    EXPECT_TRUE(sfmlInputManager->wasKeyReleased(KeyCode::Space));
    
    // Should have processed multiple events
    EXPECT_GT(sfmlInputManager->getEventCount(), 1u);
}

/**
 * Test mouse button event processing
 */
TEST_F(SFMLInputManagerTest, MouseButtonEventProcessing) {
    // Simulate mouse press event
    WindowEvent mouseEvent;
    mouseEvent.type = WindowEventType::MousePressed;
    mouseEvent.mouseButton = 0; // Left mouse button
    mouseEvent.mouseX = 100;
    mouseEvent.mouseY = 200;
    
    mockWindowManager->addEvent(mouseEvent);
    sfmlInputManager->update();
    
    // Mouse button should be detected as pressed and just-pressed
    EXPECT_TRUE(sfmlInputManager->isMouseButtonPressed(0));
    EXPECT_TRUE(sfmlInputManager->wasMouseButtonPressed(0));
    
    // Mouse position should be updated
    int x, y;
    sfmlInputManager->getMousePosition(x, y);
    EXPECT_EQ(x, 100);
    EXPECT_EQ(y, 200);
}

/**
 * Test mouse movement event processing
 */
TEST_F(SFMLInputManagerTest, MouseMovementEventProcessing) {
    // Simulate mouse move event
    WindowEvent mouseEvent;
    mouseEvent.type = WindowEventType::MouseMoved;
    mouseEvent.mouseX = 300;
    mouseEvent.mouseY = 400;
    
    mockWindowManager->addEvent(mouseEvent);
    sfmlInputManager->update();
    
    // Mouse position should be updated
    int x, y;
    sfmlInputManager->getMousePosition(x, y);
    EXPECT_EQ(x, 300);
    EXPECT_EQ(y, 400);
    
    // Event should be counted
    EXPECT_EQ(sfmlInputManager->getEventCount(), 1u);
}

/**
 * Test frame-based state management
 */
TEST_F(SFMLInputManagerTest, FrameBasedStateManagement) {
    // Simulate key press
    WindowEvent keyEvent;
    keyEvent.type = WindowEventType::KeyPressed;
    keyEvent.keyCode = static_cast<int>(sf::Keyboard::Key::Enter); // SFML Enter key
    mockWindowManager->addEvent(keyEvent);
    
    sfmlInputManager->update();
    
    // Should be just-pressed in first frame
    EXPECT_TRUE(sfmlInputManager->isKeyPressed(KeyCode::Enter));
    EXPECT_TRUE(sfmlInputManager->wasKeyPressed(KeyCode::Enter));
    
    // Update without new events
    sfmlInputManager->update();
    
    // Should still be pressed but not just-pressed
    EXPECT_TRUE(sfmlInputManager->isKeyPressed(KeyCode::Enter));
    EXPECT_FALSE(sfmlInputManager->wasKeyPressed(KeyCode::Enter));
    EXPECT_FALSE(sfmlInputManager->wasKeyReleased(KeyCode::Enter));
}

/**
 * Test multiple simultaneous keys
 */
TEST_F(SFMLInputManagerTest, MultipleSimultaneousKeys) {
    // Simulate pressing multiple keys in same frame
    WindowEvent events[] = {
        {WindowEventType::KeyPressed, static_cast<int>(sf::Keyboard::Key::A), 0, 0, 0, 0, 0}, // A key
        {WindowEventType::KeyPressed, static_cast<int>(sf::Keyboard::Key::S), 0, 0, 0, 0, 0}, // S key  
        {WindowEventType::KeyPressed, static_cast<int>(sf::Keyboard::Key::D), 0, 0, 0, 0, 0}  // D key
    };
    
    for (const auto& event : events) {
        mockWindowManager->addEvent(event);
    }
    
    sfmlInputManager->update();
    
    // All keys should be detected as pressed and just-pressed
    EXPECT_TRUE(sfmlInputManager->isKeyPressed(KeyCode::A));
    EXPECT_TRUE(sfmlInputManager->isKeyPressed(KeyCode::S));
    EXPECT_TRUE(sfmlInputManager->isKeyPressed(KeyCode::D));
    
    EXPECT_TRUE(sfmlInputManager->wasKeyPressed(KeyCode::A));
    EXPECT_TRUE(sfmlInputManager->wasKeyPressed(KeyCode::S));
    EXPECT_TRUE(sfmlInputManager->wasKeyPressed(KeyCode::D));
    
    EXPECT_EQ(sfmlInputManager->getEventCount(), 3u);
}

/**
 * Test key mapping edge cases
 */
TEST_F(SFMLInputManagerTest, KeyMappingEdgeCases) {
    // Test arrow keys (common edge case)
    WindowEvent events[] = {
        {WindowEventType::KeyPressed, static_cast<int>(sf::Keyboard::Key::Left), 0, 0, 0, 0, 0}, // Left
        {WindowEventType::KeyPressed, static_cast<int>(sf::Keyboard::Key::Up), 0, 0, 0, 0, 0}, // Up
        {WindowEventType::KeyPressed, static_cast<int>(sf::Keyboard::Key::Right), 0, 0, 0, 0, 0}, // Right
        {WindowEventType::KeyPressed, static_cast<int>(sf::Keyboard::Key::Down), 0, 0, 0, 0, 0}  // Down
    };
    
    for (const auto& event : events) {
        mockWindowManager->addEvent(event);
    }
    
    sfmlInputManager->update();
    
    // Arrow keys should be properly mapped
    EXPECT_TRUE(sfmlInputManager->isKeyPressed(KeyCode::Left));
    EXPECT_TRUE(sfmlInputManager->isKeyPressed(KeyCode::Up));
    EXPECT_TRUE(sfmlInputManager->isKeyPressed(KeyCode::Right));
    EXPECT_TRUE(sfmlInputManager->isKeyPressed(KeyCode::Down));
}

/**
 * Test unmapped key handling
 */
TEST_F(SFMLInputManagerTest, UnmappedKeyHandling) {
    // Simulate an unmapped/unknown key code
    WindowEvent unknownKeyEvent;
    unknownKeyEvent.type = WindowEventType::KeyPressed;
    unknownKeyEvent.keyCode = 9999; // Unknown key code
    
    mockWindowManager->addEvent(unknownKeyEvent);
    
    // Should not crash when processing unknown keys
    EXPECT_NO_THROW(sfmlInputManager->update());
    
    // Event should still be counted (even if key is unmapped)
    EXPECT_EQ(sfmlInputManager->getEventCount(), 1u);
}

/**
 * Test event count statistics
 */
TEST_F(SFMLInputManagerTest, EventCountStatistics) {
    // Add multiple events
    for (int i = 0; i < 5; ++i) {
        WindowEvent event;
        event.type = WindowEventType::KeyPressed;
        event.keyCode = 65 + i; // A, B, C, D, E
        mockWindowManager->addEvent(event);
    }
    
    sfmlInputManager->update();
    EXPECT_EQ(sfmlInputManager->getEventCount(), 5u);
    
    // Reset should clear count
    sfmlInputManager->resetEventCount();
    EXPECT_EQ(sfmlInputManager->getEventCount(), 0u);
}

/**
 * Test polymorphic usage
 */
TEST_F(SFMLInputManagerTest, PolymorphicUsage) {
    std::unique_ptr<IInputManager> inputManager = 
        std::make_unique<SFMLInputManager>(mockWindowManager.get());
    
    // Should work through interface
    EXPECT_FALSE(inputManager->isKeyPressed(KeyCode::A));
    EXPECT_FALSE(inputManager->wasKeyPressed(KeyCode::A));
    
    int x, y;
    inputManager->getMousePosition(x, y);
    EXPECT_EQ(x, 0);
    EXPECT_EQ(y, 0);
    
    EXPECT_NO_THROW(inputManager->update());
}

/**
 * Test error handling with null window manager
 */
TEST_F(SFMLInputManagerTest, NullWindowManagerHandling) {
    auto nullInputManager = std::make_unique<SFMLInputManager>(nullptr);
    
    // Should handle null gracefully
    EXPECT_NO_THROW(nullInputManager->update());
    EXPECT_FALSE(nullInputManager->isKeyPressed(KeyCode::A));
    EXPECT_FALSE(nullInputManager->wasKeyPressed(KeyCode::A));
    
    int x, y;
    nullInputManager->getMousePosition(x, y);
    EXPECT_EQ(x, 0);
    EXPECT_EQ(y, 0);
    
    EXPECT_EQ(nullInputManager->getEventCount(), 0u);
}