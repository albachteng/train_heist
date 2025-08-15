#pragma once

#include <SFML/Graphics.hpp>
#include "../../ecs/systems/include/IRenderer.hpp"

namespace ECS {

/**
 * SFMLRenderer - SFML implementation of IRenderer interface
 * 
 * Provides rendering capabilities using SFML graphics library.
 * Located in engine/rendering/ as it's a platform-specific implementation
 * of the generic IRenderer interface used by systems.
 */
class SFMLRenderer : public IRenderer {
private:
    sf::RenderWindow* window;  // Reference to SFML window (owned by game layer)
    
public:
    /**
     * Constructor
     * @param renderWindow Reference to SFML window created by game layer
     */
    explicit SFMLRenderer(sf::RenderWindow& renderWindow);
    
    // IRenderer interface implementation
    void beginFrame() override;
    void endFrame() override;
    void clear() override;
    
    void renderSprite(float x, float y, float z, 
                     float width, float height, 
                     int textureId) override;
    
    void renderRect(float x, float y, float width, float height,
                   float red, float green, float blue, float alpha = 1.0f) override;
    
    void getScreenSize(int& width, int& height) const override;
};

} // namespace ECS