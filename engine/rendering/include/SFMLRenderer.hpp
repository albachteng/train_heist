#pragma once

#include "IRenderer.hpp"
#include "IResourceManager.hpp"
#include "IWindowManager.hpp"
#include <memory>

// Forward declarations to avoid heavy SFML includes in header
namespace sf {
    class RenderTarget;
    class Texture;
    class RectangleShape;
    class Sprite;
}

namespace ECS {

/**
 * SFMLRenderer - SFML implementation of IRenderer interface
 * 
 * Provides hardware-accelerated 2D rendering using SFML as the backend.
 * Integrates with ResourceManager for texture loading and WindowManager for render target access.
 * 
 * Features:
 * - Hardware-accelerated sprite and shape rendering
 * - Integration with SFML's render target system
 * - Texture management through ResourceManager dependency injection
 * - Window management through WindowManager dependency injection
 * - Frame lifecycle management (clear, present)
 * - Screen size queries from window manager
 * - Optimized rendering calls with SFML objects
 * 
 * Design Philosophy:
 * - SFMLRenderer receives render target from WindowManager
 * - ResourceManager handles all texture loading/unloading
 * - Clean separation of concerns with dependency injection
 * - SFML-specific optimizations while maintaining interface compatibility
 * 
 * Dependencies:
 * - IResourceManager: For texture loading and management
 * - IWindowManager: For render target access and screen dimensions
 * - SFML: For actual rendering operations
 */
class SFMLRenderer : public IRenderer {
public:
    /**
     * Constructor with dependency injection
     * @param resourceManager Pointer to resource manager for texture access
     * @param windowManager Pointer to window manager for render target access
     */
    SFMLRenderer(IResourceManager* resourceManager, IWindowManager* windowManager);
    
    /**
     * Destructor - cleanup SFML resources
     */
    ~SFMLRenderer() override;
    
    // IRenderer interface implementation
    void beginFrame() override;
    void endFrame() override;
    void clear() override;
    void renderSprite(float x, float y, float z, float width, float height, int textureHandle) override;
    void renderRect(float x, float y, float width, float height, float r, float g, float b, float a) override;
    void getScreenSize(int& width, int& height) const override;
    
    /**
     * Get injected resource manager (for testing verification)
     * @return Pointer to current resource manager
     */
    IResourceManager* getResourceManager() const;
    
    /**
     * Get injected window manager (for testing verification)
     * @return Pointer to current window manager
     */
    IWindowManager* getWindowManager() const;
    
    /**
     * Get current render target (for testing verification)
     * @return Pointer to SFML render target, or nullptr if no window
     */
    sf::RenderTarget* getCurrentRenderTarget() const;
    
    /**
     * Get count of sprites rendered since last beginFrame (for testing)
     * @return Number of sprites rendered in current frame
     */
    size_t getSpriteRenderCount() const;
    
    /**
     * Get count of rectangles rendered since last beginFrame (for testing)
     * @return Number of rectangles rendered in current frame
     */
    size_t getRectRenderCount() const;
    
    /**
     * Check if currently in frame (between beginFrame and endFrame)
     * @return true if frame is active
     */
    bool isInFrame() const;

private:
    // Dependency injection
    IResourceManager* resourceManager;
    IWindowManager* windowManager;
    
    // SFML objects for rendering
    std::unique_ptr<sf::Sprite> sprite;
    std::unique_ptr<sf::RectangleShape> rectangle;
    
    // Current render target (cached from window manager)
    sf::RenderTarget* currentRenderTarget;
    
    // Frame state tracking
    bool frameActive;
    
    // Statistics for testing
    size_t currentFrameSpriteCount;
    size_t currentFrameRectCount;
    
    /**
     * Update render target from window manager
     * Called at beginning of each frame
     */
    void updateRenderTarget();
    
    /**
     * Get SFML texture from resource manager handle
     * @param textureHandle Handle from resource manager
     * @return Pointer to SFML texture, or nullptr if invalid
     */
    const sf::Texture* getTexture(int textureHandle) const;
};

} // namespace ECS