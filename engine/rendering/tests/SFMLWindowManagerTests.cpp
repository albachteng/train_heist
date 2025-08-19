#include <gtest/gtest.h>
#include "../include/SFMLWindowManager.hpp"
#include <SFML/Window.hpp>

using namespace ECS;

/**
 * Tests for SFML window management functionality
 * These tests focus on SFML wrapper functionality that MockWindowManager doesn't cover
 * 
 * Note: We test the public interface and observable behavior rather than internal methods
 */
class SFMLWindowManagerTests : public ::testing::Test {
protected:
    void SetUp() override {
        windowManager = std::make_unique<SFMLWindowManager>();
    }
    
    void TearDown() override {
        if (windowManager) {
            windowManager->closeWindow();
        }
        windowManager.reset();
    }
    
    std::unique_ptr<SFMLWindowManager> windowManager;
};

/**
 * Test SFML window creation - specific to SFML implementation
 */
TEST_F(SFMLWindowManagerTests, SFMLWindowCreation) {
    // Test creation with various parameters
    EXPECT_TRUE(windowManager->createWindow(800, 600, "SFML Test Window"));
    EXPECT_TRUE(windowManager->isWindowOpen());
    
    int width, height;
    windowManager->getWindowSize(width, height);
    EXPECT_EQ(width, 800);
    EXPECT_EQ(height, 600);
    
    // Test recreation
    EXPECT_TRUE(windowManager->createWindow(1024, 768, "New SFML Window"));
    EXPECT_TRUE(windowManager->isWindowOpen());
    
    windowManager->getWindowSize(width, height);
    EXPECT_EQ(width, 1024);
    EXPECT_EQ(height, 768);
}

/**
 * Test event counting during SFML polling - SFML-specific state tracking
 */
TEST_F(SFMLWindowManagerTests, EventCountingDuringPolling) {
    // Create window to enable event polling
    ASSERT_TRUE(windowManager->createWindow(400, 300, "Test Window"));
    
    // Reset event count
    windowManager->resetEventCount();
    EXPECT_EQ(windowManager->getEventCount(), 0u);
    
    // Poll events (may or may not have events, but count should be consistent)
    size_t initialCount = windowManager->getEventCount();
    
    WindowEvent event;
    while (windowManager->pollEvent(event)) {
        // Each successful poll should increment the counter
    }
    
    // Count should be >= initial count
    EXPECT_GE(windowManager->getEventCount(), initialCount);
    
    // Reset should work
    windowManager->resetEventCount();
    EXPECT_EQ(windowManager->getEventCount(), 0u);
}

/**
 * Test SFML window creation edge cases
 */
TEST_F(SFMLWindowManagerTests, WindowCreationEdgeCases) {
    // Test multiple window creation
    EXPECT_TRUE(windowManager->createWindow(800, 600, "First Window"));
    EXPECT_TRUE(windowManager->isWindowOpen());
    
    // Creating another window should close the first and create a new one
    EXPECT_TRUE(windowManager->createWindow(400, 300, "Second Window"));
    EXPECT_TRUE(windowManager->isWindowOpen());
    
    // Test with edge case dimensions
    EXPECT_TRUE(windowManager->createWindow(1, 1, "Tiny Window"));
    EXPECT_TRUE(windowManager->isWindowOpen());
}

/**
 * Test SFML-specific error handling
 */
TEST_F(SFMLWindowManagerTests, SFMLErrorHandling) {
    // Test polling without window
    WindowEvent event;
    EXPECT_FALSE(windowManager->pollEvent(event));
    EXPECT_EQ(event.type, WindowEventType::None);
    
    // Test operations on closed window
    windowManager->closeWindow();
    EXPECT_FALSE(windowManager->isWindowOpen());
    
    int width, height;
    windowManager->getWindowSize(width, height);
    EXPECT_EQ(width, 0);
    EXPECT_EQ(height, 0);
    
    // Test that getNativeRenderTarget handles null window
    void* target = windowManager->getNativeRenderTarget();
    // Should not crash, may return nullptr
    (void)target; // Suppress unused variable warning
}

/**
 * Test SFML window lifecycle
 */
TEST_F(SFMLWindowManagerTests, SFMLWindowLifecycle) {
    // Test initial state
    EXPECT_FALSE(windowManager->isWindowOpen());
    EXPECT_EQ(windowManager->getEventCount(), 0u);
    
    // Test window creation
    EXPECT_TRUE(windowManager->createWindow(640, 480, "Lifecycle Test"));
    EXPECT_TRUE(windowManager->isWindowOpen());
    
    // Test window closing
    windowManager->closeWindow();
    EXPECT_FALSE(windowManager->isWindowOpen());
    
    // Test that we can create another window after closing
    EXPECT_TRUE(windowManager->createWindow(800, 600, "Second Window"));
    EXPECT_TRUE(windowManager->isWindowOpen());
}

/**
 * Test SFML native render target functionality
 */
TEST_F(SFMLWindowManagerTests, NativeRenderTargetAccess) {
    // Test without window
    EXPECT_EQ(windowManager->getNativeRenderTarget(), nullptr);
    
    // Test with window
    EXPECT_TRUE(windowManager->createWindow(400, 300, "Render Target Test"));
    void* target = windowManager->getNativeRenderTarget();
    
    // Should return a valid pointer when window exists
    // We can't test the exact type without casting, but it shouldn't be null
    EXPECT_NE(target, nullptr);
    
    // Test after closing
    windowManager->closeWindow();
    // Behavior after closing is implementation-defined, just shouldn't crash
    (void)windowManager->getNativeRenderTarget();
}

/**
 * Test SFML window title functionality
 */
TEST_F(SFMLWindowManagerTests, WindowTitleFunctionality) {
    // Create window
    EXPECT_TRUE(windowManager->createWindow(400, 300, "Initial Title"));
    
    // Change title (shouldn't crash)
    windowManager->setWindowTitle("New Title");
    windowManager->setWindowTitle("Another Title");
    
    // Test with empty title
    windowManager->setWindowTitle("");
    
    // Test with long title
    std::string longTitle(1000, 'A');
    windowManager->setWindowTitle(longTitle);
}

/**
 * Test SFML display functionality
 */
TEST_F(SFMLWindowManagerTests, DisplayFunctionality) {
    // Test display without window (shouldn't crash)
    windowManager->display();
    
    // Test display with window
    EXPECT_TRUE(windowManager->createWindow(400, 300, "Display Test"));
    windowManager->display(); // Shouldn't crash
    
    // Test multiple displays
    for (int i = 0; i < 10; ++i) {
        windowManager->display();
    }
}

/**
 * Test SFML event conversion - Closed events
 */
TEST_F(SFMLWindowManagerTests, EventConversionClosed) {
    sf::Event sfmlEvent = sf::Event::Closed{};
    WindowEvent windowEvent;
    
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::Closed);
    
    // Other fields should remain at defaults
    EXPECT_EQ(windowEvent.keyCode, 0);
    EXPECT_EQ(windowEvent.mouseButton, 0);
    EXPECT_EQ(windowEvent.mouseX, 0);
    EXPECT_EQ(windowEvent.mouseY, 0);
    EXPECT_EQ(windowEvent.width, 0);
    EXPECT_EQ(windowEvent.height, 0);
}

/**
 * Test SFML event conversion - Resized events
 */
TEST_F(SFMLWindowManagerTests, EventConversionResized) {
    sf::Event sfmlEvent = sf::Event::Resized{{800u, 600u}};
    WindowEvent windowEvent;
    
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::Resized);
    EXPECT_EQ(windowEvent.width, 800);
    EXPECT_EQ(windowEvent.height, 600);
    
    // Other fields should remain at defaults
    EXPECT_EQ(windowEvent.keyCode, 0);
    EXPECT_EQ(windowEvent.mouseButton, 0);
    EXPECT_EQ(windowEvent.mouseX, 0);
    EXPECT_EQ(windowEvent.mouseY, 0);
}

/**
 * Test SFML event conversion - Key pressed events
 */
TEST_F(SFMLWindowManagerTests, EventConversionKeyPressed) {
    sf::Event::KeyPressed keyPressedData;
    keyPressedData.code = sf::Keyboard::Key::A;
    keyPressedData.alt = false;
    keyPressedData.control = false;
    keyPressedData.shift = false;
    keyPressedData.system = false;
    
    sf::Event sfmlEvent = keyPressedData;
    WindowEvent windowEvent;
    
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::KeyPressed);
    EXPECT_EQ(windowEvent.keyCode, static_cast<int>(sf::Keyboard::Key::A));
    
    // Test different keys
    keyPressedData.code = sf::Keyboard::Key::Space;
    sfmlEvent = keyPressedData;
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::KeyPressed);
    EXPECT_EQ(windowEvent.keyCode, static_cast<int>(sf::Keyboard::Key::Space));
    
    // Test arrow keys
    keyPressedData.code = sf::Keyboard::Key::Up;
    sfmlEvent = keyPressedData;
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::KeyPressed);
    EXPECT_EQ(windowEvent.keyCode, static_cast<int>(sf::Keyboard::Key::Up));
}

/**
 * Test SFML event conversion - Key released events
 */
TEST_F(SFMLWindowManagerTests, EventConversionKeyReleased) {
    sf::Event::KeyReleased keyReleasedData;
    keyReleasedData.code = sf::Keyboard::Key::Escape;
    keyReleasedData.alt = false;
    keyReleasedData.control = false;
    keyReleasedData.shift = false;
    keyReleasedData.system = false;
    
    sf::Event sfmlEvent = keyReleasedData;
    WindowEvent windowEvent;
    
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::KeyReleased);
    EXPECT_EQ(windowEvent.keyCode, static_cast<int>(sf::Keyboard::Key::Escape));
    
    // Test modifier keys
    keyReleasedData.code = sf::Keyboard::Key::LShift;
    sfmlEvent = keyReleasedData;
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::KeyReleased);
    EXPECT_EQ(windowEvent.keyCode, static_cast<int>(sf::Keyboard::Key::LShift));
}

/**
 * Test SFML event conversion - Mouse button pressed events
 */
TEST_F(SFMLWindowManagerTests, EventConversionMousePressed) {
    sf::Event::MouseButtonPressed mousePressedData;
    mousePressedData.button = sf::Mouse::Button::Left;
    mousePressedData.position = {100, 200};
    
    sf::Event sfmlEvent = mousePressedData;
    WindowEvent windowEvent;
    
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::MousePressed);
    EXPECT_EQ(windowEvent.mouseButton, static_cast<int>(sf::Mouse::Button::Left));
    EXPECT_EQ(windowEvent.mouseX, 100);
    EXPECT_EQ(windowEvent.mouseY, 200);
    
    // Test right mouse button
    mousePressedData.button = sf::Mouse::Button::Right;
    mousePressedData.position = {50, 75};
    sfmlEvent = mousePressedData;
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::MousePressed);
    EXPECT_EQ(windowEvent.mouseButton, static_cast<int>(sf::Mouse::Button::Right));
    EXPECT_EQ(windowEvent.mouseX, 50);
    EXPECT_EQ(windowEvent.mouseY, 75);
    
    // Test middle mouse button
    mousePressedData.button = sf::Mouse::Button::Middle;
    mousePressedData.position = {0, 0};
    sfmlEvent = mousePressedData;
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::MousePressed);
    EXPECT_EQ(windowEvent.mouseButton, static_cast<int>(sf::Mouse::Button::Middle));
    EXPECT_EQ(windowEvent.mouseX, 0);
    EXPECT_EQ(windowEvent.mouseY, 0);
}

/**
 * Test SFML event conversion - Mouse button released events
 */
TEST_F(SFMLWindowManagerTests, EventConversionMouseReleased) {
    sf::Event::MouseButtonReleased mouseReleasedData;
    mouseReleasedData.button = sf::Mouse::Button::Left;
    mouseReleasedData.position = {300, 400};
    
    sf::Event sfmlEvent = mouseReleasedData;
    WindowEvent windowEvent;
    
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::MouseReleased);
    EXPECT_EQ(windowEvent.mouseButton, static_cast<int>(sf::Mouse::Button::Left));
    EXPECT_EQ(windowEvent.mouseX, 300);
    EXPECT_EQ(windowEvent.mouseY, 400);
    
    // Test edge coordinates
    mouseReleasedData.button = sf::Mouse::Button::Right;
    mouseReleasedData.position = {-10, -5};
    sfmlEvent = mouseReleasedData;
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::MouseReleased);
    EXPECT_EQ(windowEvent.mouseButton, static_cast<int>(sf::Mouse::Button::Right));
    EXPECT_EQ(windowEvent.mouseX, -10);
    EXPECT_EQ(windowEvent.mouseY, -5);
}

/**
 * Test SFML event conversion - Mouse moved events
 */
TEST_F(SFMLWindowManagerTests, EventConversionMouseMoved) {
    sf::Event::MouseMoved mouseMovedData;
    mouseMovedData.position = {150, 250};
    
    sf::Event sfmlEvent = mouseMovedData;
    WindowEvent windowEvent;
    
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::MouseMoved);
    EXPECT_EQ(windowEvent.mouseX, 150);
    EXPECT_EQ(windowEvent.mouseY, 250);
    
    // Other fields should remain at defaults for mouse moved events
    EXPECT_EQ(windowEvent.keyCode, 0);
    EXPECT_EQ(windowEvent.mouseButton, 0);
    EXPECT_EQ(windowEvent.width, 0);
    EXPECT_EQ(windowEvent.height, 0);
    
    // Test large coordinates
    mouseMovedData.position = {1920, 1080};
    sfmlEvent = mouseMovedData;
    EXPECT_TRUE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::MouseMoved);
    EXPECT_EQ(windowEvent.mouseX, 1920);
    EXPECT_EQ(windowEvent.mouseY, 1080);
}

/**
 * Test SFML event conversion - Unknown/unsupported events
 */
TEST_F(SFMLWindowManagerTests, EventConversionUnknownEvents) {
    // Test with events that our converter doesn't handle
    sf::Event::TextEntered textEnteredData;
    textEnteredData.unicode = 65u; // Character 'A' - not supported
    sf::Event sfmlEvent = textEnteredData;
    WindowEvent windowEvent;
    
    EXPECT_FALSE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::None);
    
    // Test with mouse wheel events (not currently supported)
    sf::Event::MouseWheelScrolled mouseWheelData;
    mouseWheelData.wheel = sf::Mouse::Wheel::Vertical;
    mouseWheelData.delta = 1.0f;
    mouseWheelData.position = {100, 200};
    sfmlEvent = mouseWheelData;
    EXPECT_FALSE(windowManager->testConvertSFMLEvent(sfmlEvent, windowEvent));
    EXPECT_EQ(windowEvent.type, WindowEventType::None);
}