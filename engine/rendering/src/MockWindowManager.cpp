#include "../include/MockWindowManager.hpp"
#include <algorithm>

namespace ECS {

MockWindowManager::MockWindowManager() 
    : windowOpen(false), nextCreateResult(true), windowWidth(800), windowHeight(600), 
      windowTitle(""), eventCount(0), mockRenderTarget(42) {
    // Constructor for test setup
}

bool MockWindowManager::createWindow(int width, int height, const std::string& title) {
    methodCalls.push_back("createWindow");
    
    // Record call details
    CreateWindowCall call;
    call.width = width;
    call.height = height;
    call.title = title;
    call.returnedSuccess = nextCreateResult;
    createWindowCalls.push_back(call);
    
    // Update window state if successful
    if (nextCreateResult) {
        windowOpen = true;
        windowWidth = width;
        windowHeight = height;
        windowTitle = title;
    }
    
    return nextCreateResult;
}

void MockWindowManager::closeWindow() {
    methodCalls.push_back("closeWindow");
    
    // Close the window
    windowOpen = false;
}

bool MockWindowManager::isWindowOpen() const {
    return windowOpen;
}

bool MockWindowManager::pollEvent(WindowEvent& event) {
    if (eventQueue.empty()) {
        event.type = WindowEventType::None;
        return false;
    }
    
    // Get next event from queue
    event = eventQueue.front();
    eventQueue.pop();
    eventCount++;
    
    return true;
}

void MockWindowManager::display() {
    methodCalls.push_back("display");
    
    // Display operation complete (no additional state needed)
}

void MockWindowManager::getWindowSize(int& width, int& height) const {
    width = windowWidth;
    height = windowHeight;
}

void MockWindowManager::setWindowTitle(const std::string& title) {
    methodCalls.push_back("setWindowTitle");
    
    // Record title change
    setWindowTitleCalls.push_back(title);
    windowTitle = title;
}

void* MockWindowManager::getNativeRenderTarget() {
    // Return mock render target when window is open
    return windowOpen ? &mockRenderTarget : nullptr;
}

size_t MockWindowManager::getEventCount() const {
    return eventCount;
}

void MockWindowManager::resetEventCount() {
    methodCalls.push_back("resetEventCount");
    
    // Reset event count
    eventCount = 0;
}

void MockWindowManager::reset() {
    createWindowCalls.clear();
    setWindowTitleCalls.clear();
    methodCalls.clear();
    
    // Clear event queue
    while (!eventQueue.empty()) {
        eventQueue.pop();
    }
    
    // Reset state
    windowOpen = false;
    nextCreateResult = true;
    windowWidth = 800;
    windowHeight = 600;
    windowTitle = "";
    eventCount = 0;
}

size_t MockWindowManager::getCallCount(const std::string& methodName) const {
    return std::count(methodCalls.begin(), methodCalls.end(), methodName);
}

bool MockWindowManager::wasMethodCalled(const std::string& methodName) const {
    return std::find(methodCalls.begin(), methodCalls.end(), methodName) != methodCalls.end();
}

void MockWindowManager::setCreateWindowResult(bool success) {
    nextCreateResult = success;
}

void MockWindowManager::addEvent(const WindowEvent& event) {
    eventQueue.push(event);
}

void MockWindowManager::simulateWindowClose() {
    WindowEvent closeEvent;
    closeEvent.type = WindowEventType::Closed;
    eventQueue.push(closeEvent);
}

void MockWindowManager::setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

} // namespace ECS