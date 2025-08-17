#pragma once

#include "../../ecs/systems/include/ISystem.hpp"
#include "../../ecs/systems/include/IRenderer.hpp"
#include "../../ecs/include/EntityManager.hpp"

namespace ECS {

/**
 * RenderSystem - Renders entities with visual components
 * 
 * Features:
 * - Dependency injection of IRenderer interface for graphics abstraction
 * - Processes entities with Position + Sprite components for texture rendering
 * - Processes entities with Position + Renderable components for shape rendering
 * - Handles frame lifecycle management (beginFrame/endFrame)
 * - ECS-compliant system with bitmask-based entity queries
 * - Testable using MockRenderer without graphics dependencies
 * 
 * Component Requirements:
 * - Position (for world coordinates)
 * - Sprite OR Renderable (for visual representation)
 * 
 * Usage:
 *   MockRenderer mockRenderer;
 *   RenderSystem renderSystem(&mockRenderer);
 *   renderSystem.update(deltaTime, entityManager);
 */
class RenderSystem : public ISystem {
private:
    IRenderer* renderer;  // Injected rendering implementation
    
public:
    /**
     * Constructor with dependency injection
     * @param renderer Pointer to IRenderer implementation (MockRenderer, SFMLRenderer, etc.)
     */
    explicit RenderSystem(IRenderer* renderer);
    
    /**
     * Update system - renders all entities with visual components
     * @param deltaTime Time elapsed since last update (in seconds)
     * @param entityManager Reference to entity manager for component queries
     */
    void update(float deltaTime, EntityManager& entityManager) override;
    
    /**
     * Get required components bitmask
     * Entities need Position + (Sprite OR Renderable) to be processed
     * @return Component bitmask for entity filtering
     */
    uint64_t getRequiredComponents() const override;
    
    /**
     * Get system priority - renders after game logic systems
     * @return Priority value (higher = later, for rendering after updates)
     */
    int getPriority() const override;
    
    /**
     * Check if system should update this frame
     * @param deltaTime Time elapsed since last update
     * @return true (rendering should happen every frame)
     */
    bool shouldUpdate(float deltaTime) const override;
    
    /**
     * Get injected renderer (for testing verification)
     * @return Pointer to current renderer implementation
     */
    IRenderer* getRenderer() const;
    
    /**
     * Get count of entities rendered in last update (for testing)
     * @return Number of entities processed in last update call
     */
    size_t getLastRenderCount() const;
    
private:
    // Statistics for testing verification
    size_t lastRenderCount = 0;
    
    /**
     * Render entity with Position + Sprite components
     * @param entity Entity to render
     * @param entityManager Entity manager for component access
     */
    void renderSpriteEntity(const Entity& entity, EntityManager& entityManager);
    
    /**
     * Render entity with Position + Renderable components
     * @param entity Entity to render
     * @param entityManager Entity manager for component access
     */
    void renderShapeEntity(const Entity& entity, EntityManager& entityManager);
};

} // namespace ECS