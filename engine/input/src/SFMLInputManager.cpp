#include "../include/SFMLInputManager.hpp"

namespace ECS {

// Static member initialization
std::unordered_map<sf::Keyboard::Key, int> SFMLInputManager::sfmlToGenericKey_;
std::unordered_map<int, sf::Keyboard::Key> SFMLInputManager::genericToSfmlKey_;
std::unordered_map<sf::Mouse::Button, int> SFMLInputManager::sfmlToGenericMouse_;
std::unordered_map<int, sf::Mouse::Button> SFMLInputManager::genericToSfmlMouse_;
bool SFMLInputManager::keyMapsInitialized_ = false;

SFMLInputManager::SFMLInputManager(sf::RenderWindow& window) 
    : window_(window) {
    if (!keyMapsInitialized_) {
        initializeKeyMaps();
        keyMapsInitialized_ = true;
    }
}

void SFMLInputManager::initializeKeyMaps() {
    // Map generic key codes to SFML keys
    genericToSfmlKey_[Keys::A] = sf::Keyboard::A;
    genericToSfmlKey_[Keys::B] = sf::Keyboard::B;
    genericToSfmlKey_[Keys::C] = sf::Keyboard::C;
    genericToSfmlKey_[Keys::D] = sf::Keyboard::D;
    genericToSfmlKey_[Keys::E] = sf::Keyboard::E;
    genericToSfmlKey_[Keys::F] = sf::Keyboard::F;
    genericToSfmlKey_[Keys::G] = sf::Keyboard::G;
    genericToSfmlKey_[Keys::H] = sf::Keyboard::H;
    genericToSfmlKey_[Keys::I] = sf::Keyboard::I;
    genericToSfmlKey_[Keys::J] = sf::Keyboard::J;
    genericToSfmlKey_[Keys::K] = sf::Keyboard::K;
    genericToSfmlKey_[Keys::L] = sf::Keyboard::L;
    genericToSfmlKey_[Keys::M] = sf::Keyboard::M;
    genericToSfmlKey_[Keys::N] = sf::Keyboard::N;
    genericToSfmlKey_[Keys::O] = sf::Keyboard::O;
    genericToSfmlKey_[Keys::P] = sf::Keyboard::P;
    genericToSfmlKey_[Keys::Q] = sf::Keyboard::Q;
    genericToSfmlKey_[Keys::R] = sf::Keyboard::R;
    genericToSfmlKey_[Keys::S] = sf::Keyboard::S;
    genericToSfmlKey_[Keys::T] = sf::Keyboard::T;
    genericToSfmlKey_[Keys::U] = sf::Keyboard::U;
    genericToSfmlKey_[Keys::V] = sf::Keyboard::V;
    genericToSfmlKey_[Keys::W] = sf::Keyboard::W;
    genericToSfmlKey_[Keys::X] = sf::Keyboard::X;
    genericToSfmlKey_[Keys::Y] = sf::Keyboard::Y;
    genericToSfmlKey_[Keys::Z] = sf::Keyboard::Z;
    
    // Arrow keys
    genericToSfmlKey_[Keys::Up] = sf::Keyboard::Up;
    genericToSfmlKey_[Keys::Down] = sf::Keyboard::Down;
    genericToSfmlKey_[Keys::Left] = sf::Keyboard::Left;
    genericToSfmlKey_[Keys::Right] = sf::Keyboard::Right;
    
    // Common keys
    genericToSfmlKey_[Keys::Space] = sf::Keyboard::Space;
    genericToSfmlKey_[Keys::Enter] = sf::Keyboard::Enter;
    genericToSfmlKey_[Keys::Escape] = sf::Keyboard::Escape;
    genericToSfmlKey_[Keys::Shift] = sf::Keyboard::LShift;
    genericToSfmlKey_[Keys::Control] = sf::Keyboard::LControl;
    genericToSfmlKey_[Keys::Alt] = sf::Keyboard::LAlt;
    
    // Build reverse mapping
    for (const auto& pair : genericToSfmlKey_) {
        sfmlToGenericKey_[pair.second] = pair.first;
    }
    
    // Mouse button mappings
    genericToSfmlMouse_[MouseButtons::Left] = sf::Mouse::Left;
    genericToSfmlMouse_[MouseButtons::Right] = sf::Mouse::Right;
    genericToSfmlMouse_[MouseButtons::Middle] = sf::Mouse::Middle;
    
    // Build reverse mapping
    for (const auto& pair : genericToSfmlMouse_) {
        sfmlToGenericMouse_[pair.second] = pair.first;
    }
}

bool SFMLInputManager::isKeyPressed(int keyCode) const {
    return pressedKeys_.find(keyCode) != pressedKeys_.end();
}

bool SFMLInputManager::wasKeyJustPressed(int keyCode) const {
    return justPressedKeys_.find(keyCode) != justPressedKeys_.end();
}

bool SFMLInputManager::wasKeyJustReleased(int keyCode) const {
    return justReleasedKeys_.find(keyCode) != justReleasedKeys_.end();
}

bool SFMLInputManager::isMouseButtonPressed(int button) const {
    return pressedMouseButtons_.find(button) != pressedMouseButtons_.end();
}

bool SFMLInputManager::wasMouseButtonJustPressed(int button) const {
    return justPressedMouseButtons_.find(button) != justPressedMouseButtons_.end();
}

bool SFMLInputManager::wasMouseButtonJustReleased(int button) const {
    return justReleasedMouseButtons_.find(button) != justReleasedMouseButtons_.end();
}

void SFMLInputManager::getMousePosition(int& x, int& y) const {
    sf::Vector2i pos = sf::Mouse::getPosition(window_);
    x = pos.x;
    y = pos.y;
}

void SFMLInputManager::update() {
    // Clear "just pressed/released" states (they only last one frame)
    justPressedKeys_.clear();
    justReleasedKeys_.clear();
    justPressedMouseButtons_.clear();
    justReleasedMouseButtons_.clear();
}

void SFMLInputManager::reset() {
    pressedKeys_.clear();
    justPressedKeys_.clear();
    justReleasedKeys_.clear();
    pressedMouseButtons_.clear();
    justPressedMouseButtons_.clear();
    justReleasedMouseButtons_.clear();
}

void SFMLInputManager::processEvent(const sf::Event& event) {
    switch (event.type) {
        case sf::Event::KeyPressed: {
            int genericKey = getGenericKey(event.key.code);
            if (genericKey != -1) {
                if (pressedKeys_.find(genericKey) == pressedKeys_.end()) {
                    justPressedKeys_.insert(genericKey);
                }
                pressedKeys_.insert(genericKey);
            }
            break;
        }
        
        case sf::Event::KeyReleased: {
            int genericKey = getGenericKey(event.key.code);
            if (genericKey != -1) {
                if (pressedKeys_.find(genericKey) != pressedKeys_.end()) {
                    justReleasedKeys_.insert(genericKey);
                    pressedKeys_.erase(genericKey);
                }
            }
            break;
        }
        
        case sf::Event::MouseButtonPressed: {
            int genericButton = getGenericMouseButton(event.mouseButton.button);
            if (genericButton != -1) {
                if (pressedMouseButtons_.find(genericButton) == pressedMouseButtons_.end()) {
                    justPressedMouseButtons_.insert(genericButton);
                }
                pressedMouseButtons_.insert(genericButton);
            }
            break;
        }
        
        case sf::Event::MouseButtonReleased: {
            int genericButton = getGenericMouseButton(event.mouseButton.button);
            if (genericButton != -1) {
                if (pressedMouseButtons_.find(genericButton) != pressedMouseButtons_.end()) {
                    justReleasedMouseButtons_.insert(genericButton);
                    pressedMouseButtons_.erase(genericButton);
                }
            }
            break;
        }
        
        default:
            // Ignore other events
            break;
    }
}

sf::Keyboard::Key SFMLInputManager::getSFMLKey(int keyCode) const {
    auto it = genericToSfmlKey_.find(keyCode);
    return (it != genericToSfmlKey_.end()) ? it->second : sf::Keyboard::Unknown;
}

int SFMLInputManager::getGenericKey(sf::Keyboard::Key sfmlKey) const {
    auto it = sfmlToGenericKey_.find(sfmlKey);
    return (it != sfmlToGenericKey_.end()) ? it->second : -1;
}

sf::Mouse::Button SFMLInputManager::getSFMLMouseButton(int button) const {
    auto it = genericToSfmlMouse_.find(button);
    return (it != genericToSfmlMouse_.end()) ? it->second : sf::Mouse::ButtonCount;
}

int SFMLInputManager::getGenericMouseButton(sf::Mouse::Button sfmlButton) const {
    auto it = sfmlToGenericMouse_.find(sfmlButton);
    return (it != sfmlToGenericMouse_.end()) ? it->second : -1;
}

} // namespace ECS