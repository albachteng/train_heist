#include "../include/SFMLInputManager.hpp"
#include "../../ecs/systems/include/IInputManager.hpp"
#include "../../rendering/include/IWindowManager.hpp"
#include <SFML/Window.hpp>

namespace ECS {

SFMLInputManager::SFMLInputManager(IWindowManager* windowManager)
    : windowManager(windowManager)
    , mouseX(0)
    , mouseY(0)
    , eventCount(0)
{
    // Initialize key mapping tables
    initializeKeyMappings();
}

bool SFMLInputManager::isKeyPressed(int keyCode) const {
    return currentlyPressedKeys.find(keyCode) != currentlyPressedKeys.end();
}

bool SFMLInputManager::wasKeyPressed(int keyCode) const {
    return justPressedKeys.find(keyCode) != justPressedKeys.end();
}

bool SFMLInputManager::wasKeyReleased(int keyCode) const {
    return justReleasedKeys.find(keyCode) != justReleasedKeys.end();
}

void SFMLInputManager::getMousePosition(int& x, int& y) const {
    x = mouseX;
    y = mouseY;
}

bool SFMLInputManager::isMouseButtonPressed(int button) const {
    return currentlyPressedMouseButtons.find(button) != currentlyPressedMouseButtons.end();
}

bool SFMLInputManager::wasMouseButtonPressed(int button) const {
    return justPressedMouseButtons.find(button) != justPressedMouseButtons.end();
}

void SFMLInputManager::update() {
    // Clear frame-specific events from previous frame
    justPressedKeys.clear();
    justReleasedKeys.clear();
    justPressedMouseButtons.clear();
    
    // Process events from window manager if available
    if (windowManager) {
        WindowEvent event;
        while (windowManager->pollEvent(event)) {
            processEvent(event);
            eventCount++;
        }
    }
}

size_t SFMLInputManager::getEventCount() const {
    return eventCount;
}

void SFMLInputManager::resetEventCount() {
    eventCount = 0;
}

IWindowManager* SFMLInputManager::getWindowManager() const {
    return windowManager;
}

void SFMLInputManager::processEvent(const WindowEvent& event) {
    switch (event.type) {
        case WindowEventType::KeyPressed: {
            int engineKeyCode = convertFromSFMLKeyCode(event.keyCode);
            if (engineKeyCode != -1) {
                currentlyPressedKeys.insert(engineKeyCode);
                justPressedKeys.insert(engineKeyCode);
            }
            break;
        }
        
        case WindowEventType::KeyReleased: {
            int engineKeyCode = convertFromSFMLKeyCode(event.keyCode);
            if (engineKeyCode != -1) {
                currentlyPressedKeys.erase(engineKeyCode);
                justReleasedKeys.insert(engineKeyCode);
            }
            break;
        }
        
        case WindowEventType::MousePressed: {
            int engineButton = convertFromSFMLMouseButton(event.mouseButton);
            if (engineButton != -1) {
                currentlyPressedMouseButtons.insert(engineButton);
                justPressedMouseButtons.insert(engineButton);
            }
            // Update mouse position
            mouseX = event.mouseX;
            mouseY = event.mouseY;
            break;
        }
        
        case WindowEventType::MouseReleased: {
            int engineButton = convertFromSFMLMouseButton(event.mouseButton);
            if (engineButton != -1) {
                currentlyPressedMouseButtons.erase(engineButton);
            }
            // Update mouse position
            mouseX = event.mouseX;
            mouseY = event.mouseY;
            break;
        }
        
        case WindowEventType::MouseMoved: {
            mouseX = event.mouseX;
            mouseY = event.mouseY;
            break;
        }
        
        default:
            // Ignore other event types
            break;
    }
}

void SFMLInputManager::initializeKeyMappings() {
    // Map engine key codes to SFML key codes
    // Letters A-Z
    for (int i = 0; i <= 25; ++i) {
        engineToSFMLKeyMap[i] = static_cast<int>(sf::Keyboard::Key::A) + i;
        sfmlToEngineKeyMap[static_cast<int>(sf::Keyboard::Key::A) + i] = i;
    }
    
    // Special keys
    engineToSFMLKeyMap[KeyCode::Space] = static_cast<int>(sf::Keyboard::Key::Space);
    sfmlToEngineKeyMap[static_cast<int>(sf::Keyboard::Key::Space)] = KeyCode::Space;
    
    engineToSFMLKeyMap[KeyCode::Enter] = static_cast<int>(sf::Keyboard::Key::Enter);
    sfmlToEngineKeyMap[static_cast<int>(sf::Keyboard::Key::Enter)] = KeyCode::Enter;
    
    engineToSFMLKeyMap[KeyCode::Escape] = static_cast<int>(sf::Keyboard::Key::Escape);
    sfmlToEngineKeyMap[static_cast<int>(sf::Keyboard::Key::Escape)] = KeyCode::Escape;
    
    engineToSFMLKeyMap[KeyCode::Tab] = static_cast<int>(sf::Keyboard::Key::Tab);
    sfmlToEngineKeyMap[static_cast<int>(sf::Keyboard::Key::Tab)] = KeyCode::Tab;
    
    engineToSFMLKeyMap[KeyCode::Backspace] = static_cast<int>(sf::Keyboard::Key::Backspace);
    sfmlToEngineKeyMap[static_cast<int>(sf::Keyboard::Key::Backspace)] = KeyCode::Backspace;
    
    // Arrow keys
    engineToSFMLKeyMap[KeyCode::Left] = static_cast<int>(sf::Keyboard::Key::Left);
    sfmlToEngineKeyMap[static_cast<int>(sf::Keyboard::Key::Left)] = KeyCode::Left;
    
    engineToSFMLKeyMap[KeyCode::Up] = static_cast<int>(sf::Keyboard::Key::Up);
    sfmlToEngineKeyMap[static_cast<int>(sf::Keyboard::Key::Up)] = KeyCode::Up;
    
    engineToSFMLKeyMap[KeyCode::Right] = static_cast<int>(sf::Keyboard::Key::Right);
    sfmlToEngineKeyMap[static_cast<int>(sf::Keyboard::Key::Right)] = KeyCode::Right;
    
    engineToSFMLKeyMap[KeyCode::Down] = static_cast<int>(sf::Keyboard::Key::Down);
    sfmlToEngineKeyMap[static_cast<int>(sf::Keyboard::Key::Down)] = KeyCode::Down;
}

int SFMLInputManager::convertToSFMLKeyCode(int engineKeyCode) const {
    auto it = engineToSFMLKeyMap.find(engineKeyCode);
    return (it != engineToSFMLKeyMap.end()) ? it->second : -1;
}

int SFMLInputManager::convertFromSFMLKeyCode(int sfmlKeyCode) const {
    auto it = sfmlToEngineKeyMap.find(sfmlKeyCode);
    return (it != sfmlToEngineKeyMap.end()) ? it->second : -1;
}

int SFMLInputManager::convertToSFMLMouseButton(int engineButton) const {
    switch (engineButton) {
        case 0: return static_cast<int>(sf::Mouse::Button::Left);
        case 1: return static_cast<int>(sf::Mouse::Button::Right);
        case 2: return static_cast<int>(sf::Mouse::Button::Middle);
        default: return -1;
    }
}

int SFMLInputManager::convertFromSFMLMouseButton(int sfmlButton) const {
    switch (sfmlButton) {
        case static_cast<int>(sf::Mouse::Button::Left): return 0;
        case static_cast<int>(sf::Mouse::Button::Right): return 1;
        case static_cast<int>(sf::Mouse::Button::Middle): return 2;
        default: return -1;
    }
}

} // namespace ECS