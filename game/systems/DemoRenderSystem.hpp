#pragma once

#include "../../engine/ecs/systems/include/ISystem.hpp"
#include "../../engine/ecs/systems/include/IRenderer.hpp"
#include "../../engine/input/include/InputComponents.hpp"
#include "../../engine/ecs/components/include/TransformComponents.hpp"

/**
 * DemoRenderSystem - Simple demo system for Track 1 validation
 * 
 * Game-specific system that demonstrates:
 * - System integration with EntityManager
 * - Dependency injection of IRenderer
 * - Basic rendering operations
 * 
 * This system is located in game/systems/ because it's game-specific logic,
 * not generic engine functionality.
 */
class DemoRenderSystem : public ECS::ISystem {
private:
    ECS::IRenderer* renderer;
    float time;  // Simple animation timer
    
public:
    /**
     * Constructor with dependency injection
     * @param renderer Renderer implementation (SFML, OpenGL, mock, etc.)
     */
    explicit DemoRenderSystem(ECS::IRenderer* renderer);
    
    // ISystem interface
    void update(float deltaTime, ECS::EntityManager& entityManager) override;
    uint64_t getRequiredComponents() const override { return 0; } // No specific components required
    int getPriority() const override { return 100; } // Render after logic systems
};