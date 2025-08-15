#pragma once

namespace ECS {

/**
 * IRenderer - Abstract rendering interface for dependency injection
 * 
 * Allows systems to render without depending on specific rendering implementations.
 * Can be swapped for different backends (SFML, OpenGL direct, mock renderer for tests).
 */
class IRenderer {
public:
    virtual ~IRenderer() = default;
    
    /**
     * Begin a new frame of rendering
     */
    virtual void beginFrame() = 0;
    
    /**
     * End the current frame and present to screen
     */
    virtual void endFrame() = 0;
    
    /**
     * Clear the screen with background color
     */
    virtual void clear() = 0;
    
    /**
     * Render a sprite at the specified position
     * @param x X coordinate in world space
     * @param y Y coordinate in world space  
     * @param z Z coordinate for depth sorting
     * @param width Sprite width
     * @param height Sprite height
     * @param textureId Identifier for the texture to render
     */
    virtual void renderSprite(float x, float y, float z, 
                             float width, float height, 
                             int textureId) = 0;
    
    /**
     * Render a colored rectangle (useful for debug visualization)
     * @param x X coordinate
     * @param y Y coordinate
     * @param width Rectangle width
     * @param height Rectangle height
     * @param red Red component (0.0-1.0)
     * @param green Green component (0.0-1.0)
     * @param blue Blue component (0.0-1.0)
     * @param alpha Alpha component (0.0-1.0)
     */
    virtual void renderRect(float x, float y, float width, float height,
                           float red, float green, float blue, float alpha = 1.0f) = 0;
    
    /**
     * Get screen dimensions
     * @param width Output parameter for screen width
     * @param height Output parameter for screen height
     */
    virtual void getScreenSize(int& width, int& height) const = 0;
};

} // namespace ECS