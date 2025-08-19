#include "../include/MockInputManager.hpp"

namespace ECS {

MockInputManager::MockInputManager() 
    : mouseX(0)
    , mouseY(0)
    , updateCount(0)
{
    // Initialize with empty sets and zero values
}

bool MockInputManager::isKeyPressed(int keyCode) const {
    return currentlyPressedKeys.find(keyCode) != currentlyPressedKeys.end();
}

bool MockInputManager::wasKeyPressed(int keyCode) const {
    return justPressedKeys.find(keyCode) != justPressedKeys.end();
}

bool MockInputManager::wasKeyReleased(int keyCode) const {
    return justReleasedKeys.find(keyCode) != justReleasedKeys.end();
}

void MockInputManager::getMousePosition(int& x, int& y) const {
    x = mouseX;
    y = mouseY;
}

bool MockInputManager::isMouseButtonPressed(int button) const {
    return currentlyPressedMouseButtons.find(button) != currentlyPressedMouseButtons.end();
}

bool MockInputManager::wasMouseButtonPressed(int button) const {
    return justPressedMouseButtons.find(button) != justPressedMouseButtons.end();
}

void MockInputManager::update() {
    // Clear frame-specific events
    justPressedKeys.clear();
    justReleasedKeys.clear();
    justPressedMouseButtons.clear();
    
    // Increment update counter
    updateCount++;
}

void MockInputManager::simulateKeyPress(int keyCode) {
    // Add to current state and just-pressed events
    currentlyPressedKeys.insert(keyCode);
    justPressedKeys.insert(keyCode);
    
    // Remove from just-released if it was there
    justReleasedKeys.erase(keyCode);
}

void MockInputManager::simulateKeyRelease(int keyCode) {
    // Remove from current state and add to just-released
    currentlyPressedKeys.erase(keyCode);
    justReleasedKeys.insert(keyCode);
    
    // Remove from just-pressed if it was there
    justPressedKeys.erase(keyCode);
}

void MockInputManager::setKeyPressed(int keyCode) {
    // Set current state without triggering just-pressed
    currentlyPressedKeys.insert(keyCode);
}

void MockInputManager::setKeyReleased(int keyCode) {
    // Remove from current state without triggering just-released
    currentlyPressedKeys.erase(keyCode);
}

void MockInputManager::simulateMousePress(int button) {
    // Add to current state and just-pressed events
    currentlyPressedMouseButtons.insert(button);
    justPressedMouseButtons.insert(button);
}

void MockInputManager::simulateMouseRelease(int button) {
    // Remove from current state
    currentlyPressedMouseButtons.erase(button);
}

void MockInputManager::setMouseButtonPressed(int button) {
    // Set current state without triggering just-pressed
    currentlyPressedMouseButtons.insert(button);
}

void MockInputManager::setMousePosition(int x, int y) {
    mouseX = x;
    mouseY = y;
}

void MockInputManager::reset() {
    // Clear all state
    currentlyPressedKeys.clear();
    currentlyPressedMouseButtons.clear();
    justPressedKeys.clear();
    justReleasedKeys.clear();
    justPressedMouseButtons.clear();
    
    // Reset position and counters
    mouseX = 0;
    mouseY = 0;
    updateCount = 0;
}

size_t MockInputManager::getUpdateCount() const {
    return updateCount;
}

} // namespace ECS