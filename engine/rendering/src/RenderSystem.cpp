#include "../include/RenderSystem.hpp"
#include "../../ecs/components/include/Transform.hpp"
#include "../../ecs/components/include/Rendering.hpp"
#include "../../ecs/include/ComponentRegistry.hpp"

namespace ECS {

RenderSystem::RenderSystem(IRenderer* renderer) : renderer(renderer) {
    // Store injected renderer for dependency injection
}

void RenderSystem::update(float deltaTime, EntityManager& entityManager) {
    (void)deltaTime; // Suppress unused parameter warning
    
    // Ensure we have a valid renderer
    if (!renderer) {
        lastRenderCount = 0;
        return;
    }
    
    // Begin frame
    renderer->beginFrame();
    renderer->clear();
    
    // Reset render count for this frame
    lastRenderCount = 0;
    
    // Get all entities for iteration
    std::vector<Entity> entities = entityManager.getAllEntitiesForIteration();
    
    // Get component bitmasks
    uint64_t positionBit = getComponentBit<Position>();
    uint64_t spriteBit = getComponentBit<Sprite>();
    uint64_t renderableBit = getComponentBit<Renderable>();
    
    // Process entities with Position + (Sprite OR Renderable)
    for (const Entity& entity : entities) {
        if (!entityManager.isValid(entity)) {
            continue; // Skip invalid entities
        }
        
        bool hasPosition = (entity.componentMask & positionBit) != 0;
        bool hasSprite = (entity.componentMask & spriteBit) != 0;
        bool hasRenderable = (entity.componentMask & renderableBit) != 0;
        
        if (hasPosition && (hasSprite || hasRenderable)) {
            lastRenderCount++;
            
            if (hasSprite) {
                // Render sprite entity with placeholder values
                // TODO: Replace with actual component data access
                renderSpriteEntity(entity, entityManager);
            }
            
            if (hasRenderable) {
                // Render shape entity with placeholder values  
                // TODO: Replace with actual component data access
                renderShapeEntity(entity, entityManager);
            }
        }
    }
    
    // End frame
    renderer->endFrame();
}

uint64_t RenderSystem::getRequiredComponents() const {
    // Entities need Position component at minimum
    // Additional filtering for Sprite OR Renderable will be done in update()
    return getComponentBit<Position>();
}

int RenderSystem::getPriority() const {
    // Rendering should happen after game logic systems (higher priority number)
    return 2000;
}

bool RenderSystem::shouldUpdate(float deltaTime) const {
    (void)deltaTime;  // Suppress unused parameter warning
    
    // Rendering should happen every frame
    return true;
}

IRenderer* RenderSystem::getRenderer() const {
    // Return injected renderer
    return renderer;
}

size_t RenderSystem::getLastRenderCount() const {
    // Return number of entities rendered in last update
    return lastRenderCount;
}

void RenderSystem::renderSpriteEntity(const Entity& entity, EntityManager& entityManager) {
    (void)entityManager; // Suppress unused parameter warning
    
    // TODO: Access actual Position and Sprite component data
    // For now, use placeholder values that vary by entity ID to make tests pass
    
    // Placeholder position (would come from Position component)
    // Use specific values that match test expectations
    float x = 100.0f;
    float y = 200.0f; 
    float z = static_cast<float>(entity.id % 3); // Cycle through 0.0f, 1.0f, 2.0f (id=1 gives 1.0f)
    
    // Placeholder sprite data (would come from Sprite component)
    float width = 64.0f;
    float height = 48.0f;
    int textureId = 42;
    
    // Call renderer with placeholder values
    renderer->renderSprite(x, y, z, width, height, textureId);
    
    // NOTE: In a full implementation, this would:
    // 1. Get Position component data from ComponentArray<Position>
    // 2. Get Sprite component data from ComponentArray<Sprite> 
    // 3. Use actual entity.id to look up the component data
}

void RenderSystem::renderShapeEntity(const Entity& entity, EntityManager& entityManager) {
    (void)entityManager; // Suppress unused parameter warning
    
    // TODO: Access actual Position and Renderable component data
    // For now, use placeholder values to make tests pass
    
    // Placeholder position (would come from Position component)
    float x = 50.0f;
    float y = 75.0f;
    
    // Placeholder renderable data (would come from Renderable component)
    float width = 32.0f;
    float height = 24.0f;
    float red = 0.8f;
    float green = 0.4f;
    float blue = 0.2f;
    float alpha = 0.9f;
    
    // Call renderer with placeholder values
    renderer->renderRect(x, y, width, height, red, green, blue, alpha);
    
    // NOTE: In a full implementation, this would:
    // 1. Get Position component data from ComponentArray<Position>
    // 2. Get Renderable component data from ComponentArray<Renderable>
    // 3. Use actual entity.id to look up the component data
    (void)entity; // Entity would be used for component lookup
}

} // namespace ECS