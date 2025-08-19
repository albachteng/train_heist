#pragma once

#include "../../ecs/systems/include/IInputManager.hpp"
#include "../../rendering/include/IWindowManager.hpp"
#include <unordered_map>
#include <unordered_set>

namespace ECS {

/**
 * SFMLInputManager - SFML-based implementation of IInputManager
 * 
 * Processes SFML events and manages input state for keyboard and mouse.
 * Requires a WindowManager to poll events from the window system.
 */
class SFMLInputManager : public IInputManager {
public:
    /**
     * Constructor with dependency injection
     * @param windowManager Window manager to poll events from
     */
    explicit SFMLInputManager(IWindowManager* windowManager);
    ~SFMLInputManager() override = default;
    
    // IInputManager interface implementation
    bool isKeyPressed(int keyCode) const override;
    bool wasKeyPressed(int keyCode) const override;
    bool wasKeyReleased(int keyCode) const override;
    void getMousePosition(int& x, int& y) const override;
    bool isMouseButtonPressed(int button) const override;
    bool wasMouseButtonPressed(int button) const override;
    void update() override;
    
    // Testing and debugging interface
    
    /**
     * Get number of events processed since last reset
     * @return Event count
     */
    size_t getEventCount() const;
    
    /**
     * Reset event count statistics
     */
    void resetEventCount();
    
    /**
     * Get the window manager being used
     * @return Pointer to window manager
     */
    IWindowManager* getWindowManager() const;

private:
    /**
     * Convert engine key code to SFML key code
     * @param engineKeyCode Engine key code (KeyCode namespace)
     * @return SFML key code, or -1 if unmapped
     */
    int convertToSFMLKeyCode(int engineKeyCode) const;
    
    /**
     * Convert SFML key code to engine key code
     * @param sfmlKeyCode SFML key code
     * @return Engine key code, or -1 if unmapped
     */
    int convertFromSFMLKeyCode(int sfmlKeyCode) const;
    
    /**
     * Convert engine mouse button to SFML mouse button
     * @param engineButton Engine mouse button (0=left, 1=right, 2=middle)
     * @return SFML mouse button, or -1 if invalid
     */
    int convertToSFMLMouseButton(int engineButton) const;
    
    // Dependencies
    IWindowManager* windowManager;
    
    // Current state
    std::unordered_set<int> currentlyPressedKeys;    // Engine key codes
    std::unordered_set<int> currentlyPressedMouseButtons; // Engine button codes
    
    // Frame-specific events (cleared on update)
    std::unordered_set<int> justPressedKeys;         // Engine key codes  
    std::unordered_set<int> justReleasedKeys;        // Engine key codes
    std::unordered_set<int> justPressedMouseButtons; // Engine button codes
    
    // Mouse position
    int mouseX;
    int mouseY;
    
    // Statistics
    size_t eventCount;
    
    // Key mapping tables (initialized in constructor)
    std::unordered_map<int, int> engineToSFMLKeyMap;
    std::unordered_map<int, int> sfmlToEngineKeyMap;
};

} // namespace ECS