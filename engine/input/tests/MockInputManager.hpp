#pragma once

#include "../include/IInputManager.hpp"
#include <unordered_map>
#include <unordered_set>

namespace ECS {

/**
 * MockInputManager - Test implementation of IInputManager
 * 
 * Allows programmatic control of input state for testing.
 */
class MockInputManager : public IInputManager {
public:
    // Simulated input state
    std::unordered_set<int> pressedKeys;
    std::unordered_set<int> justPressedKeys;
    std::unordered_set<int> justReleasedKeys;
    
    std::unordered_set<int> pressedMouseButtons;
    std::unordered_set<int> justPressedMouseButtons;
    std::unordered_set<int> justReleasedMouseButtons;
    
    int mouseX = 0;
    int mouseY = 0;
    
    // IInputManager interface
    bool isKeyPressed(int keyCode) const override {
        return pressedKeys.find(keyCode) != pressedKeys.end();
    }
    
    bool wasKeyJustPressed(int keyCode) const override {
        return justPressedKeys.find(keyCode) != justPressedKeys.end();
    }
    
    bool wasKeyJustReleased(int keyCode) const override {
        return justReleasedKeys.find(keyCode) != justReleasedKeys.end();
    }
    
    bool isMouseButtonPressed(int button) const override {
        return pressedMouseButtons.find(button) != pressedMouseButtons.end();
    }
    
    bool wasMouseButtonJustPressed(int button) const override {
        return justPressedMouseButtons.find(button) != justPressedMouseButtons.end();
    }
    
    bool wasMouseButtonJustReleased(int button) const override {
        return justReleasedMouseButtons.find(button) != justReleasedMouseButtons.end();
    }
    
    void getMousePosition(int& x, int& y) const override {
        x = mouseX;
        y = mouseY;
    }
    
    void update() override {
        // Clear "just pressed/released" states (they only last one frame)
        justPressedKeys.clear();
        justReleasedKeys.clear();
        justPressedMouseButtons.clear();
        justReleasedMouseButtons.clear();
    }
    
    void reset() override {
        pressedKeys.clear();
        justPressedKeys.clear();
        justReleasedKeys.clear();
        pressedMouseButtons.clear();
        justPressedMouseButtons.clear();
        justReleasedMouseButtons.clear();
        mouseX = 0;
        mouseY = 0;
    }
    
    // Test utility methods
    void pressKey(int keyCode) {
        if (pressedKeys.find(keyCode) == pressedKeys.end()) {
            justPressedKeys.insert(keyCode);
        }
        pressedKeys.insert(keyCode);
    }
    
    void releaseKey(int keyCode) {
        if (pressedKeys.find(keyCode) != pressedKeys.end()) {
            justReleasedKeys.insert(keyCode);
            pressedKeys.erase(keyCode);
        }
    }
    
    void pressMouseButton(int button) {
        if (pressedMouseButtons.find(button) == pressedMouseButtons.end()) {
            justPressedMouseButtons.insert(button);
        }
        pressedMouseButtons.insert(button);
    }
    
    void releaseMouseButton(int button) {
        if (pressedMouseButtons.find(button) != pressedMouseButtons.end()) {
            justReleasedMouseButtons.insert(button);
            pressedMouseButtons.erase(button);
        }
    }
    
    void setMousePosition(int x, int y) {
        mouseX = x;
        mouseY = y;
    }
    
    // Test a complete key press cycle (press -> update -> release -> update)
    void simulateKeyPress(int keyCode) {
        pressKey(keyCode);
        update();
        releaseKey(keyCode);
        update();
    }
};

} // namespace ECS