#include "../include/SFMLRenderer.hpp"
#include "../../logging/include/Logger.hpp"

namespace ECS {

SFMLRenderer::SFMLRenderer(sf::RenderWindow& renderWindow) 
    : window(&renderWindow) {
    LOG_INFO("Renderer", "SFMLRenderer initialized");
}

void SFMLRenderer::beginFrame() {
    // SFML handles frame begin automatically
    // This is where we could set up render states if needed
}

void SFMLRenderer::endFrame() {
    window->display();
}

void SFMLRenderer::clear() {
    window->clear(sf::Color::Black);
}

void SFMLRenderer::renderSprite(float x, float y, float z, 
                               float width, float height, 
                               int textureId) {
    (void)z; // Suppress unused parameter warning
    (void)textureId; // Will use when texture system exists
    
    // TODO: Implement sprite rendering with texture management
    // For now, render a placeholder colored rectangle
    
    // Placeholder: render as colored rectangle based on textureId
    sf::RectangleShape sprite(sf::Vector2f(width, height));
    sprite.setPosition(sf::Vector2f(x, y));
    
    // Use textureId to determine color (temporary)
    std::uint8_t red = (textureId * 73) % 256;
    std::uint8_t green = (textureId * 151) % 256; 
    std::uint8_t blue = (textureId * 211) % 256;
    sprite.setFillColor(sf::Color(red, green, blue));
    
    window->draw(sprite);
}

void SFMLRenderer::renderRect(float x, float y, float width, float height,
                             float red, float green, float blue, float alpha) {
    sf::RectangleShape rect(sf::Vector2f(width, height));
    rect.setPosition(sf::Vector2f(x, y));
    
    // Convert float colors [0.0-1.0] to SFML Uint8 [0-255]
    std::uint8_t r = static_cast<std::uint8_t>(red * 255.0f);
    std::uint8_t g = static_cast<std::uint8_t>(green * 255.0f);
    std::uint8_t b = static_cast<std::uint8_t>(blue * 255.0f);
    std::uint8_t a = static_cast<std::uint8_t>(alpha * 255.0f);
    
    rect.setFillColor(sf::Color(r, g, b, a));
    window->draw(rect);
}

void SFMLRenderer::getScreenSize(int& width, int& height) const {
    sf::Vector2u size = window->getSize();
    width = static_cast<int>(size.x);
    height = static_cast<int>(size.y);
}

} // namespace ECS