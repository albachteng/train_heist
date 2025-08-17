#include "../include/MockWindowManager.hpp"
#include <gtest/gtest.h>

using namespace ECS;

/**
 * MockWindowManager Tests
 *
 * These tests define the expected behavior of MockWindowManager for testing
 * window systems. MockWindowManager should simulate window creation/management
 * without requiring actual windows.
 *
 * Key test areas:
 * - Window creation and destruction
 * - Window state queries (open/closed, dimensions)
 * - Event polling and processing
 * - Display and presentation operations
 * - Call recording for test verification
 * - Error handling and edge cases
 */

class MockWindowManagerTest : public ::testing::Test {
protected:
  void SetUp() override {
    mockWindowManager = std::make_unique<MockWindowManager>();
  }

  void TearDown() override { mockWindowManager.reset(); }

  std::unique_ptr<MockWindowManager> mockWindowManager;
};

// Test basic interface compliance
TEST_F(MockWindowManagerTest, InterfaceCompliance) {
  // Should implement all IWindowManager methods without crashing
  EXPECT_NO_THROW(mockWindowManager->createWindow(800, 600, "Test Window"));
  EXPECT_NO_THROW(mockWindowManager->closeWindow());
  EXPECT_NO_THROW(mockWindowManager->isWindowOpen());

  WindowEvent event;
  EXPECT_NO_THROW(mockWindowManager->pollEvent(event));
  EXPECT_NO_THROW(mockWindowManager->display());

  int width, height;
  EXPECT_NO_THROW(mockWindowManager->getWindowSize(width, height));
  EXPECT_NO_THROW(mockWindowManager->setWindowTitle("New Title"));
  EXPECT_NO_THROW(mockWindowManager->getNativeRenderTarget());
  EXPECT_NO_THROW(mockWindowManager->getEventCount());
  EXPECT_NO_THROW(mockWindowManager->resetEventCount());
}

// Test window creation functionality
TEST_F(MockWindowManagerTest, CreateWindow) {
  // Should successfully create window
  bool result = mockWindowManager->createWindow(1024, 768, "Game Window");
  EXPECT_TRUE(result);

  // Should record the create call
  EXPECT_TRUE(mockWindowManager->wasMethodCalled("createWindow"));
  EXPECT_EQ(mockWindowManager->getCallCount("createWindow"), 1);

  // Should record call details
  EXPECT_EQ(mockWindowManager->createWindowCalls.size(), 1);
  const auto &call = mockWindowManager->createWindowCalls[0];
  EXPECT_EQ(call.width, 1024);
  EXPECT_EQ(call.height, 768);
  EXPECT_EQ(call.title, "Game Window");
  EXPECT_TRUE(call.returnedSuccess);

  // Window should be open after successful creation
  EXPECT_TRUE(mockWindowManager->isWindowOpen());
}

// Test window creation failure
TEST_F(MockWindowManagerTest, CreateWindowFailure) {
  // Configure to fail
  mockWindowManager->setCreateWindowResult(false);

  bool result = mockWindowManager->createWindow(800, 600, "Test");
  EXPECT_FALSE(result);

  // Should still record the call
  EXPECT_TRUE(mockWindowManager->wasMethodCalled("createWindow"));
  EXPECT_EQ(mockWindowManager->createWindowCalls.size(), 1);
  EXPECT_FALSE(mockWindowManager->createWindowCalls[0].returnedSuccess);

  // Window should not be open after failed creation
  EXPECT_FALSE(mockWindowManager->isWindowOpen());
}

// Test multiple window creation attempts
TEST_F(MockWindowManagerTest, MultipleCreateAttempts) {
  // First creation
  mockWindowManager->createWindow(800, 600, "Window 1");

  // Second creation (should replace first)
  mockWindowManager->createWindow(1024, 768, "Window 2");

  // Should record both calls
  EXPECT_EQ(mockWindowManager->getCallCount("createWindow"), 2);
  EXPECT_EQ(mockWindowManager->createWindowCalls.size(), 2);

  // Check call details
  EXPECT_EQ(mockWindowManager->createWindowCalls[0].title, "Window 1");
  EXPECT_EQ(mockWindowManager->createWindowCalls[1].title, "Window 2");

  // Second window creation should overwrite the first
  int width, height;
  mockWindowManager->getWindowSize(width, height);
  EXPECT_EQ(width, 1024);
  EXPECT_EQ(height, 768);
}

// Test window closing
TEST_F(MockWindowManagerTest, CloseWindow) {
  // Create window first
  mockWindowManager->createWindow(800, 600, "Test Window");
  EXPECT_TRUE(mockWindowManager->isWindowOpen());

  // Close window
  mockWindowManager->closeWindow();

  // Should record the close call
  EXPECT_TRUE(mockWindowManager->wasMethodCalled("closeWindow"));
  EXPECT_EQ(mockWindowManager->getCallCount("closeWindow"), 1);

  // Window should no longer be open
  EXPECT_FALSE(mockWindowManager->isWindowOpen());
}

// Test window state queries
TEST_F(MockWindowManagerTest, WindowStateQueries) {
  // Initially no window should be open
  EXPECT_FALSE(mockWindowManager->isWindowOpen());

  // Create window
  mockWindowManager->createWindow(1280, 720, "State Test");
  EXPECT_TRUE(mockWindowManager->isWindowOpen());

  // Check window size
  int width, height;
  mockWindowManager->getWindowSize(width, height);
  EXPECT_EQ(width, 1280);
  EXPECT_EQ(height, 720);

  // Close window
  mockWindowManager->closeWindow();
  EXPECT_FALSE(mockWindowManager->isWindowOpen());
}

// Test event polling with no events
TEST_F(MockWindowManagerTest, PollEventEmpty) {
  WindowEvent event;

  // Should return false when no events
  bool hasEvent = mockWindowManager->pollEvent(event);
  EXPECT_FALSE(hasEvent);

  // Event should remain unchanged (or be None type)
  EXPECT_EQ(event.type, WindowEventType::None);
}

// Test event polling with queued events
TEST_F(MockWindowManagerTest, PollEventWithEvents) {
  // Add events to queue
  WindowEvent closeEvent;
  closeEvent.type = WindowEventType::Closed;
  mockWindowManager->addEvent(closeEvent);

  WindowEvent keyEvent;
  keyEvent.type = WindowEventType::KeyPressed;
  keyEvent.keyCode = 65; // 'A' key
  mockWindowManager->addEvent(keyEvent);

  // Poll first event
  WindowEvent polledEvent;
  bool hasEvent = mockWindowManager->pollEvent(polledEvent);
  EXPECT_TRUE(hasEvent);
  EXPECT_EQ(polledEvent.type, WindowEventType::Closed);

  // Poll second event
  hasEvent = mockWindowManager->pollEvent(polledEvent);
  EXPECT_TRUE(hasEvent);
  EXPECT_EQ(polledEvent.type, WindowEventType::KeyPressed);
  EXPECT_EQ(polledEvent.keyCode, 65);

  // No more events
  hasEvent = mockWindowManager->pollEvent(polledEvent);
  EXPECT_FALSE(hasEvent);
}

// Test event count tracking
TEST_F(MockWindowManagerTest, EventCountTracking) {
  // Initially should have no events processed
  EXPECT_EQ(mockWindowManager->getEventCount(), 0);

  // Add and poll events
  WindowEvent event1;
  event1.type = WindowEventType::KeyPressed;
  mockWindowManager->addEvent(event1);

  WindowEvent event2;
  event2.type = WindowEventType::MousePressed;
  mockWindowManager->addEvent(event2);

  // Poll events
  WindowEvent polledEvent;
  mockWindowManager->pollEvent(polledEvent);
  mockWindowManager->pollEvent(polledEvent);

  // Should track processed events
  EXPECT_EQ(mockWindowManager->getEventCount(), 2);

  // Reset count
  mockWindowManager->resetEventCount();
  EXPECT_EQ(mockWindowManager->getEventCount(), 0);
  EXPECT_TRUE(mockWindowManager->wasMethodCalled("resetEventCount"));
}

// Test display functionality
TEST_F(MockWindowManagerTest, Display) {
  mockWindowManager->display();

  // Should record display calls
  EXPECT_TRUE(mockWindowManager->wasMethodCalled("display"));
  EXPECT_EQ(mockWindowManager->getCallCount("display"), 1);

  // Multiple display calls
  mockWindowManager->display();
  mockWindowManager->display();
  EXPECT_EQ(mockWindowManager->getCallCount("display"), 3);
}

// Test window title operations
TEST_F(MockWindowManagerTest, WindowTitle) {
  mockWindowManager->setWindowTitle("New Game Title");

  // Should record the call
  EXPECT_TRUE(mockWindowManager->wasMethodCalled("setWindowTitle"));
  EXPECT_EQ(mockWindowManager->getCallCount("setWindowTitle"), 1);

  // Should record title changes
  EXPECT_EQ(mockWindowManager->setWindowTitleCalls.size(), 1);
  EXPECT_EQ(mockWindowManager->setWindowTitleCalls[0], "New Game Title");

  // Multiple title changes
  mockWindowManager->setWindowTitle("Updated Title");
  EXPECT_EQ(mockWindowManager->setWindowTitleCalls.size(), 2);
  EXPECT_EQ(mockWindowManager->setWindowTitleCalls[1], "Updated Title");
}

// Test native render target
TEST_F(MockWindowManagerTest, NativeRenderTarget) {
  // Should return valid pointer when window is open
  mockWindowManager->createWindow(800, 600, "Render Test");
  void *renderTarget = mockWindowManager->getNativeRenderTarget();
  EXPECT_NE(renderTarget, nullptr);

  // Should return nullptr when no window
  mockWindowManager->closeWindow();
  renderTarget = mockWindowManager->getNativeRenderTarget();
  EXPECT_EQ(renderTarget, nullptr);
}

// Test window size configuration
TEST_F(MockWindowManagerTest, WindowSizeConfiguration) {
  // Set custom window size
  mockWindowManager->setWindowSize(1920, 1080);

  int width, height;
  mockWindowManager->getWindowSize(width, height);
  EXPECT_EQ(width, 1920);
  EXPECT_EQ(height, 1080);

  // Change size
  mockWindowManager->setWindowSize(640, 480);
  mockWindowManager->getWindowSize(width, height);
  EXPECT_EQ(width, 640);
  EXPECT_EQ(height, 480);
}

// Test reset functionality
TEST_F(MockWindowManagerTest, ResetClearsState) {
  // Create some state
  mockWindowManager->createWindow(800, 600, "Test");
  mockWindowManager->setWindowTitle("Title");
  mockWindowManager->display();

  WindowEvent event;
  event.type = WindowEventType::KeyPressed;
  mockWindowManager->addEvent(event);

  EXPECT_GT(mockWindowManager->methodCalls.size(), 0);
  EXPECT_GT(mockWindowManager->createWindowCalls.size(), 0);
  EXPECT_GT(mockWindowManager->setWindowTitleCalls.size(), 0);

  // Reset should clear everything
  mockWindowManager->reset();

  EXPECT_EQ(mockWindowManager->methodCalls.size(), 0);
  EXPECT_EQ(mockWindowManager->createWindowCalls.size(), 0);
  EXPECT_EQ(mockWindowManager->setWindowTitleCalls.size(), 0);
  EXPECT_FALSE(mockWindowManager->wasMethodCalled("createWindow"));
  EXPECT_FALSE(mockWindowManager->isWindowOpen());

  // Event queue should be empty
  WindowEvent polledEvent;
  EXPECT_FALSE(mockWindowManager->pollEvent(polledEvent));
}

// Test window close event simulation
TEST_F(MockWindowManagerTest, WindowCloseEventSimulation) {
  mockWindowManager->createWindow(800, 600, "Close Test");
  EXPECT_TRUE(mockWindowManager->isWindowOpen());

  // Simulate window close event
  mockWindowManager->simulateWindowClose();

  // Should have close event in queue
  WindowEvent event;
  bool hasEvent = mockWindowManager->pollEvent(event);
  EXPECT_TRUE(hasEvent);
  EXPECT_EQ(event.type, WindowEventType::Closed);
}

// Test edge cases and robustness
TEST_F(MockWindowManagerTest, EdgeCases) {
  // Zero-size window should be handled
  EXPECT_NO_THROW(mockWindowManager->createWindow(0, 0, "Zero Size"));

  // Negative size should be handled
  EXPECT_NO_THROW(mockWindowManager->createWindow(-100, -100, "Negative"));

  // Empty title should be handled
  EXPECT_NO_THROW(mockWindowManager->createWindow(800, 600, ""));

  // Very long title should be handled
  std::string longTitle(1000, 'A');
  EXPECT_NO_THROW(mockWindowManager->createWindow(800, 600, longTitle));

  // Operations on non-existent window should be handled gracefully
  mockWindowManager->closeWindow(); // No window to close
  mockWindowManager->display();     // No window to display
  EXPECT_NO_THROW(mockWindowManager->getNativeRenderTarget());
}

// Test typical usage pattern
TEST_F(MockWindowManagerTest, TypicalUsagePattern) {
  // Create window
  bool created = mockWindowManager->createWindow(1024, 768, "My Game");
  EXPECT_TRUE(created);
  EXPECT_TRUE(mockWindowManager->isWindowOpen());

  // Set title
  mockWindowManager->setWindowTitle("My Game - Level 1");

  // Simulate game loop operations
  mockWindowManager->display();

  // Process some events
  WindowEvent keyEvent;
  keyEvent.type = WindowEventType::KeyPressed;
  keyEvent.keyCode = 32; // Space key
  mockWindowManager->addEvent(keyEvent);

  WindowEvent event;
  bool hasEvent = mockWindowManager->pollEvent(event);
  EXPECT_TRUE(hasEvent);
  EXPECT_EQ(event.type, WindowEventType::KeyPressed);
  EXPECT_EQ(event.keyCode, 32);

  // Verify operations were recorded
  EXPECT_TRUE(mockWindowManager->wasMethodCalled("createWindow"));
  EXPECT_TRUE(mockWindowManager->wasMethodCalled("setWindowTitle"));
  EXPECT_TRUE(mockWindowManager->wasMethodCalled("display"));

  // Check state
  int width, height;
  mockWindowManager->getWindowSize(width, height);
  EXPECT_EQ(width, 1024);
  EXPECT_EQ(height, 768);

  // Get render target
  void *renderTarget = mockWindowManager->getNativeRenderTarget();
  EXPECT_NE(renderTarget, nullptr);

  // Close window
  mockWindowManager->closeWindow();
  EXPECT_FALSE(mockWindowManager->isWindowOpen());
}

// Test polymorphic usage through interface
TEST_F(MockWindowManagerTest, PolymorphicUsage) {
  IWindowManager *windowManager = mockWindowManager.get();

  // Should work through base interface
  bool created = windowManager->createWindow(800, 600, "Interface Test");
  EXPECT_TRUE(created);
  EXPECT_TRUE(windowManager->isWindowOpen());

  int width, height;
  windowManager->getWindowSize(width, height);
  EXPECT_EQ(width, 800);
  EXPECT_EQ(height, 600);

  windowManager->display();
  windowManager->setWindowTitle("Interface Title");

  // Verify calls were recorded
  EXPECT_TRUE(mockWindowManager->wasMethodCalled("createWindow"));
  EXPECT_TRUE(mockWindowManager->wasMethodCalled("display"));
  EXPECT_TRUE(mockWindowManager->wasMethodCalled("setWindowTitle"));
}
