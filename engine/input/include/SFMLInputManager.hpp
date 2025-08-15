#pragma once

#include "IInputManager.hpp"
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <unordered_set>

namespace ECS {

/**
 * SFMLInputManager - SFML implementation of IInputManager
 * 
 * Handles keyboard and mouse input using SFML's input system.
 * Maps generic key codes to SFML key codes.
 */
class SFMLInputManager : public IInputManager {
public:
    explicit SFMLInputManager(sf::RenderWindow& window);
    virtual ~SFMLInputManager() = default;
    
    // IInputManager interface
    bool isKeyPressed(int keyCode) const override;
    bool wasKeyJustPressed(int keyCode) const override;
    bool wasKeyJustReleased(int keyCode) const override;
    
    bool isMouseButtonPressed(int button) const override;
    bool wasMouseButtonJustPressed(int button) const override;
    bool wasMouseButtonJustReleased(int button) const override;
    void getMousePosition(int& x, int& y) const override;
    
    void update() override;
    void reset() override;
    
    // SFML-specific methods
    void processEvent(const sf::Event& event);
    
private:
    sf::RenderWindow& window_;
    
    // Key state tracking
    std::unordered_set<int> pressedKeys_;
    std::unordered_set<int> justPressedKeys_;
    std::unordered_set<int> justReleasedKeys_;
    
    // Mouse state tracking
    std::unordered_set<int> pressedMouseButtons_;
    std::unordered_set<int> justPressedMouseButtons_;
    std::unordered_set<int> justReleasedMouseButtons_;
    
    // Key code mapping
    static std::unordered_map<sf::Keyboard::Key, int> sfmlToGenericKey_;
    static std::unordered_map<int, sf::Keyboard::Key> genericToSfmlKey_;
    static std::unordered_map<sf::Mouse::Button, int> sfmlToGenericMouse_;
    static std::unordered_map<int, sf::Mouse::Button> genericToSfmlMouse_;
    
    static void initializeKeyMaps();
    static bool keyMapsInitialized_;
    
    // Helper methods
    sf::Keyboard::Key getSFMLKey(int keyCode) const;
    int getGenericKey(sf::Keyboard::Key sfmlKey) const;
    sf::Mouse::Button getSFMLMouseButton(int button) const;
    int getGenericMouseButton(sf::Mouse::Button sfmlButton) const;
};

} // namespace ECS