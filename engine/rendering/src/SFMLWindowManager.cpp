#include "../include/SFMLWindowManager.hpp"

namespace ECS {

SFMLWindowManager::SFMLWindowManager() : eventCount(0) {
    // Window will be created when createWindow() is called
}

SFMLWindowManager::~SFMLWindowManager() {
    if (window && window->isOpen()) {
        window->close();
    }
}

bool SFMLWindowManager::createWindow(int width, int height, const std::string& title) {
    // Close existing window if open
    if (window && window->isOpen()) {
        window->close();
    }
    
    // Create new SFML window (SFML 3.x API with WSLg compatibility)
    try {
        sf::ContextSettings settings;
        settings.majorVersion = 2;
        settings.minorVersion = 1;
        settings.attributeFlags = sf::ContextSettings::Core;
        
        window = std::make_unique<sf::RenderWindow>(
            sf::VideoMode(sf::Vector2u(static_cast<unsigned int>(width), static_cast<unsigned int>(height))), 
            title,
            sf::Style::Default,
            sf::State::Windowed,
            settings
        );
        return window && window->isOpen();
    } catch (...) {
        window.reset();
        return false;
    }
}

void SFMLWindowManager::closeWindow() {
    if (window) {
        window->close();
    }
}

bool SFMLWindowManager::isWindowOpen() const {
    return window && window->isOpen();
}

bool SFMLWindowManager::pollEvent(WindowEvent& event) {
    if (!window) {
        event.type = WindowEventType::None;
        return false;
    }
    
    try {
        // SFML 3.x API: pollEvent() returns std::optional<sf::Event>
        auto sfmlEvent = window->pollEvent();
        if (sfmlEvent.has_value()) {
            eventCount++;
            return convertSFMLEvent(sfmlEvent.value(), event);
        }
    } catch (...) {
        // Ignore polling errors and continue
    }
    
    event.type = WindowEventType::None;
    return false;
}

void SFMLWindowManager::display() {
    if (window) {
        window->display();
    }
}

void SFMLWindowManager::getWindowSize(int& width, int& height) const {
    if (window) {
        sf::Vector2u size = window->getSize();
        width = static_cast<int>(size.x);
        height = static_cast<int>(size.y);
    } else {
        width = 0;
        height = 0;
    }
}

void SFMLWindowManager::setWindowTitle(const std::string& title) {
    if (window) {
        window->setTitle(title);
    }
}

void* SFMLWindowManager::getNativeRenderTarget() {
    return window.get();
}

size_t SFMLWindowManager::getEventCount() const {
    return eventCount;
}

void SFMLWindowManager::resetEventCount() {
    eventCount = 0;
}

bool SFMLWindowManager::convertSFMLEvent(const sf::Event& sfmlEvent, WindowEvent& windowEvent) {
    // SFML 3.x uses visitor pattern for events
    if (const auto* closed = sfmlEvent.getIf<sf::Event::Closed>()) {
        (void)closed; // Suppress unused variable warning
        windowEvent.type = WindowEventType::Closed;
        return true;
    }
    
    if (const auto* resized = sfmlEvent.getIf<sf::Event::Resized>()) {
        windowEvent.type = WindowEventType::Resized;
        windowEvent.width = static_cast<int>(resized->size.x);
        windowEvent.height = static_cast<int>(resized->size.y);
        return true;
    }
    
    if (const auto* keyPressed = sfmlEvent.getIf<sf::Event::KeyPressed>()) {
        windowEvent.type = WindowEventType::KeyPressed;
        windowEvent.keyCode = static_cast<int>(keyPressed->code);
        return true;
    }
    
    if (const auto* keyReleased = sfmlEvent.getIf<sf::Event::KeyReleased>()) {
        windowEvent.type = WindowEventType::KeyReleased;
        windowEvent.keyCode = static_cast<int>(keyReleased->code);
        return true;
    }
    
    if (const auto* mousePressed = sfmlEvent.getIf<sf::Event::MouseButtonPressed>()) {
        windowEvent.type = WindowEventType::MousePressed;
        windowEvent.mouseButton = static_cast<int>(mousePressed->button);
        windowEvent.mouseX = static_cast<int>(mousePressed->position.x);
        windowEvent.mouseY = static_cast<int>(mousePressed->position.y);
        return true;
    }
    
    if (const auto* mouseReleased = sfmlEvent.getIf<sf::Event::MouseButtonReleased>()) {
        windowEvent.type = WindowEventType::MouseReleased;
        windowEvent.mouseButton = static_cast<int>(mouseReleased->button);
        windowEvent.mouseX = static_cast<int>(mouseReleased->position.x);
        windowEvent.mouseY = static_cast<int>(mouseReleased->position.y);
        return true;
    }
    
    if (const auto* mouseMoved = sfmlEvent.getIf<sf::Event::MouseMoved>()) {
        windowEvent.type = WindowEventType::MouseMoved;
        windowEvent.mouseX = static_cast<int>(mouseMoved->position.x);
        windowEvent.mouseY = static_cast<int>(mouseMoved->position.y);
        return true;
    }
    
    // Unknown event type
    windowEvent.type = WindowEventType::None;
    return false;
}

} // namespace ECS