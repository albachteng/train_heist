#pragma once

#include "IWindowManager.hpp"
#include <vector>
#include <string>
#include <queue>

namespace ECS {

/**
 * MockWindowManager - Test implementation of IWindowManager interface
 * 
 * Simulates window management operations without requiring actual window creation.
 * Records all method calls and provides configurable behavior for testing.
 * 
 * Features:
 * - Records all method calls for test verification
 * - Simulates window creation/destruction without actual windows
 * - Event queue simulation for testing event handling
 * - Configurable success/failure behavior for testing error cases
 * - Full window lifecycle management simulation
 */
class MockWindowManager : public IWindowManager {
public:
    MockWindowManager();
    
    // Recorded calls for test verification
    struct CreateWindowCall {
        int width;
        int height;
        std::string title;
        bool returnedSuccess;
    };
    
    struct WindowSizeCall {
        int width;
        int height;
    };
    
    std::vector<CreateWindowCall> createWindowCalls;
    std::vector<std::string> setWindowTitleCalls;
    std::vector<std::string> methodCalls;
    
    // Event simulation
    std::queue<WindowEvent> eventQueue;
    
    // IWindowManager interface (will cause test failures in RED phase)
    bool createWindow(int width, int height, const std::string& title) override;
    void closeWindow() override;
    bool isWindowOpen() const override;
    bool pollEvent(WindowEvent& event) override;
    void display() override;
    void getWindowSize(int& width, int& height) const override;
    void setWindowTitle(const std::string& title) override;
    void* getNativeRenderTarget() override;
    size_t getEventCount() const override;
    void resetEventCount() override;
    
    // Test utility methods
    void reset();
    size_t getCallCount(const std::string& methodName) const;
    bool wasMethodCalled(const std::string& methodName) const;
    
    // Test configuration methods
    void setCreateWindowResult(bool success);
    void addEvent(const WindowEvent& event);
    void simulateWindowClose();
    void setWindowSize(int width, int height);
    
private:
    // Mock state
    bool windowOpen;
    bool nextCreateResult;
    int windowWidth;
    int windowHeight;
    std::string windowTitle;
    size_t eventCount;
    
    // Mock render target (just a placeholder pointer)
    int mockRenderTarget;
};

} // namespace ECS