#pragma once

#include "../../ecs/systems/include/IInputManager.hpp"
#include <unordered_map>
#include <unordered_set>

namespace ECS {

/**
 * MockInputManager - Mock implementation of IInputManager for testing
 * 
 * Provides controllable input state for unit testing without external dependencies.
 * Supports simulating key/mouse states and frame-based input tracking.
 */
class MockInputManager : public IInputManager {
public:
    MockInputManager();
    ~MockInputManager() override = default;
    
    // IInputManager interface implementation
    bool isKeyPressed(int keyCode) const override;
    bool wasKeyPressed(int keyCode) const override;
    bool wasKeyReleased(int keyCode) const override;
    void getMousePosition(int& x, int& y) const override;
    bool isMouseButtonPressed(int button) const override;
    bool wasMouseButtonPressed(int button) const override;
    void update() override;
    
    // Testing interface - Methods for setting up test scenarios
    
    /**
     * Simulate a key press (will be detected as pressed and just-pressed)
     * @param keyCode Key code to press
     */
    void simulateKeyPress(int keyCode);
    
    /**
     * Simulate a key release (will be detected as released and just-released)
     * @param keyCode Key code to release
     */
    void simulateKeyRelease(int keyCode);
    
    /**
     * Set a key as currently pressed (without triggering just-pressed)
     * @param keyCode Key code to set as pressed
     */
    void setKeyPressed(int keyCode);
    
    /**
     * Set a key as not pressed
     * @param keyCode Key code to set as not pressed
     */
    void setKeyReleased(int keyCode);
    
    /**
     * Simulate mouse button press (will be detected as pressed and just-pressed)
     * @param button Mouse button to press (0=left, 1=right, 2=middle)
     */
    void simulateMousePress(int button);
    
    /**
     * Simulate mouse button release
     * @param button Mouse button to release
     */
    void simulateMouseRelease(int button);
    
    /**
     * Set mouse button as currently pressed (without triggering just-pressed)
     * @param button Mouse button to set as pressed
     */
    void setMouseButtonPressed(int button);
    
    /**
     * Set mouse position
     * @param x Mouse X coordinate
     * @param y Mouse Y coordinate
     */
    void setMousePosition(int x, int y);
    
    /**
     * Reset all input states to default (all keys/buttons released)
     */
    void reset();
    
    /**
     * Get number of update() calls made
     * @return Update call count
     */
    size_t getUpdateCount() const;

private:
    // Current key/button states
    std::unordered_set<int> currentlyPressedKeys;
    std::unordered_set<int> currentlyPressedMouseButtons;
    
    // Frame-specific states (cleared on update)
    std::unordered_set<int> justPressedKeys;
    std::unordered_set<int> justReleasedKeys;
    std::unordered_set<int> justPressedMouseButtons;
    
    // Mouse position
    int mouseX;
    int mouseY;
    
    // Statistics for testing
    size_t updateCount;
};

} // namespace ECS