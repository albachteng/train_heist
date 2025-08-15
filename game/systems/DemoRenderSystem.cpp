#include "DemoRenderSystem.hpp"
#include "../../engine/logging/include/Logger.hpp"
#include <cmath>

DemoRenderSystem::DemoRenderSystem(ECS::IRenderer* renderer) 
    : renderer(renderer), time(0.0f) {
    LOG_INFO("DemoSystem", "DemoRenderSystem created");
}

void DemoRenderSystem::update(float deltaTime, ECS::EntityManager& entityManager) {
    time += deltaTime;
    
    // Get screen dimensions for positioning
    int screenWidth, screenHeight;
    renderer->getScreenSize(screenWidth, screenHeight);
    
    // Begin rendering
    renderer->beginFrame();
    renderer->clear();
    
    // Update and render controllable entities
    const float MOVE_SPEED = 200.0f; // pixels per second
    
    // Query entities with Position, InputState, and Controllable components
    auto requiredMask = ECS::ComponentRegistry::getBitMask<ECS::Position>() |
                       ECS::ComponentRegistry::getBitMask<ECS::InputState>() |
                       ECS::ComponentRegistry::getBitMask<ECS::Controllable>();
    
    auto controllableEntities = entityManager.getEntitiesWithComponents(requiredMask);
    
    for (auto entity : controllableEntities) {
        auto* position = entityManager.getComponent<ECS::Position>(entity);
        auto* inputState = entityManager.getComponent<ECS::InputState>(entity);
        auto* controllable = entityManager.getComponent<ECS::Controllable>(entity);
        
        if (position && inputState && controllable && controllable->enabled) {
            // Update position based on input
            if (inputState->moveLeft) {
                position->x -= MOVE_SPEED * deltaTime;
            }
            if (inputState->moveRight) {
                position->x += MOVE_SPEED * deltaTime;
            }
            if (inputState->moveUp) {
                position->y -= MOVE_SPEED * deltaTime;
            }
            if (inputState->moveDown) {
                position->y += MOVE_SPEED * deltaTime;
            }
            
            // Keep player within screen bounds
            const float PLAYER_SIZE = 50.0f;
            if (position->x < 0) position->x = 0;
            if (position->y < 0) position->y = 0;
            if (position->x > screenWidth - PLAYER_SIZE) position->x = screenWidth - PLAYER_SIZE;
            if (position->y > screenHeight - PLAYER_SIZE) position->y = screenHeight - PLAYER_SIZE;
            
            // Render controllable entity as a blue square
            renderer->renderRect(position->x, position->y, PLAYER_SIZE, PLAYER_SIZE,
                               0.0f, 0.0f, 1.0f, 1.0f);  // Blue
            
            // Log input for debugging
            if (inputState->upPressed || inputState->downPressed || 
                inputState->leftPressed || inputState->rightPressed ||
                inputState->actionPressed) {
                LOG_INFO("DemoSystem", "Entity {} input: U:{} D:{} L:{} R:{} A:{}", 
                        entity.id, inputState->moveUp, inputState->moveDown, 
                        inputState->moveLeft, inputState->moveRight, inputState->action);
            }
        }
    }
    
    // Demo: Render some background elements to show the system working
    
    // Static rectangle in center
    float centerX = screenWidth / 2.0f - 50.0f;
    float centerY = screenHeight / 2.0f - 50.0f;
    renderer->renderRect(centerX, centerY, 100.0f, 100.0f, 
                        0.0f, 1.0f, 0.0f, 0.3f);  // Semi-transparent green
    
    // Animated rectangle that moves in a circle
    float radius = 100.0f;
    float animX = centerX + radius * std::cos(time);
    float animY = centerY + radius * std::sin(time);
    renderer->renderRect(animX, animY, 30.0f, 30.0f,
                        1.0f, 0.0f, 0.0f, 0.5f);  // Semi-transparent red
    
    // Color-changing rectangle in corner
    float colorValue = (std::sin(time * 2.0f) + 1.0f) / 2.0f; // 0.0 to 1.0
    renderer->renderRect(50.0f, 50.0f, 80.0f, 80.0f,
                        colorValue, 0.0f, 1.0f - colorValue, 0.4f);  // Semi-transparent purple-blue blend
    
    // Instructions text placeholder (will be actual text when UI system exists)
    renderer->renderRect(10.0f, screenHeight - 40.0f, 200.0f, 30.0f,
                        0.2f, 0.2f, 0.2f, 0.8f);  // Dark background for "text"
    
    // End rendering
    renderer->endFrame();
    
    // Log occasionally for debugging
    static int frameCount = 0;
    frameCount++;
    if (frameCount % 300 == 0) { // Every 5 seconds at 60 FPS
        LOG_DEBUG("DemoSystem", "Rendered frame {} with {} controllable entities", 
                 frameCount, controllableEntities.size());
    }
}