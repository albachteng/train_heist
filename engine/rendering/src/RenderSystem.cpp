#include "../include/RenderSystem.hpp"
#include "../../ecs/components/include/Transform.hpp"
#include "../../ecs/components/include/Rendering.hpp"
#include "../../ecs/include/ComponentRegistry.hpp"

namespace ECS {

RenderSystem::RenderSystem(IRenderer* renderer,
                           ComponentArray<Position>* positions,
                           ComponentArray<Sprite>* sprites,
                           ComponentArray<Renderable>* renderables)
    : renderer(renderer),
      positions(positions),
      sprites(sprites),
      renderables(renderables) {
    // Store injected dependencies for rendering
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
    std::vector<const Entity*> entities = entityManager.getAllEntitiesForIteration();

    // Get component bitmasks
    uint64_t positionBit = getComponentBit<Position>();
    uint64_t spriteBit = getComponentBit<Sprite>();
    uint64_t renderableBit = getComponentBit<Renderable>();

    // Process entities with Position + (Sprite OR Renderable)
    for (const Entity* entity : entities) {
        if (!entityManager.isValid(*entity)) {
            continue; // Skip invalid entities
        }

        bool hasPosition = (entity->componentMask & positionBit) != 0;
        bool hasSprite = (entity->componentMask & spriteBit) != 0;
        bool hasRenderable = (entity->componentMask & renderableBit) != 0;
        
        if (hasPosition && (hasSprite || hasRenderable)) {
            lastRenderCount++;
            
            if (hasSprite) {
                // Render sprite entity with placeholder values
                // TODO: Replace with actual component data access
                renderSpriteEntity(*entity, entityManager);
            }

            if (hasRenderable) {
                // Render shape entity with placeholder values
                // TODO: Replace with actual component data access
                renderShapeEntity(*entity, entityManager);
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

    // Get actual Position component data (or use placeholders if not available)
    float x = 100.0f;
    float y = 200.0f;
    float z = static_cast<float>(entity.id % 3); // Default fallback for old tests

    if (positions) {
        const Position* pos = positions->get(entity.id);
        if (pos) {
            x = pos->x;
            y = pos->y;
            z = pos->z;
        }
    }

    // Get actual Sprite component data (or use placeholders if not available)
    float width = 64.0f;
    float height = 48.0f;
    int textureId = 42;

    if (sprites) {
        const Sprite* sprite = sprites->get(entity.id);
        if (sprite) {
            textureId = sprite->textureId;
            width = sprite->width;
            height = sprite->height;
        }
    }

    // Call renderer with actual or placeholder values
    renderer->renderSprite(x, y, z, width, height, textureId);
}

void RenderSystem::renderShapeEntity(const Entity& entity, EntityManager& entityManager) {
    (void)entityManager; // Suppress unused parameter warning

    // Get actual Position component data (or use placeholders if not available)
    float x = 50.0f;
    float y = 75.0f;

    if (positions) {
        const Position* pos = positions->get(entity.id);
        if (pos) {
            x = pos->x;
            y = pos->y;
            // Note: z-coordinate not used for rectangles currently
        }
    }

    // Get actual Renderable component data (or use placeholders if not available)
    float width = 32.0f;
    float height = 24.0f;
    float red = 0.8f;
    float green = 0.4f;
    float blue = 0.2f;
    float alpha = 0.9f;

    if (renderables) {
        const Renderable* renderable = renderables->get(entity.id);
        if (renderable) {
            width = renderable->width;
            height = renderable->height;
            red = renderable->red;
            green = renderable->green;
            blue = renderable->blue;
            alpha = renderable->alpha;
        }
    }

    // Call renderer with actual or placeholder values
    renderer->renderRect(x, y, width, height, red, green, blue, alpha);
}

} // namespace ECS