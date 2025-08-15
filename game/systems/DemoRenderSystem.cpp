#include "DemoRenderSystem.hpp"
#include "../../engine/logging/include/Logger.hpp"
#include <cmath>

DemoRenderSystem::DemoRenderSystem(ECS::IRenderer* renderer) 
    : renderer(renderer), time(0.0f) {
    LOG_INFO("DemoSystem", "DemoRenderSystem created");
}

void DemoRenderSystem::update(float deltaTime, ECS::EntityManager& entityManager) {
    (void)entityManager; // Suppress unused parameter warning
    time += deltaTime;
    
    // Get screen dimensions for positioning
    int screenWidth, screenHeight;
    renderer->getScreenSize(screenWidth, screenHeight);
    
    // Begin rendering
    renderer->beginFrame();
    renderer->clear();
    
    // Demo: Render multiple colored rectangles to show the system working
    
    // Static rectangle in center
    float centerX = screenWidth / 2.0f - 50.0f;
    float centerY = screenHeight / 2.0f - 50.0f;
    renderer->renderRect(centerX, centerY, 100.0f, 100.0f, 
                        0.0f, 1.0f, 0.0f, 1.0f);  // Green
    
    // Animated rectangle that moves in a circle
    float radius = 150.0f;
    float animX = centerX + radius * std::cos(time);
    float animY = centerY + radius * std::sin(time);
    renderer->renderRect(animX, animY, 50.0f, 50.0f,
                        1.0f, 0.0f, 0.0f, 1.0f);  // Red
    
    // Color-changing rectangle in corner
    float colorValue = (std::sin(time * 2.0f) + 1.0f) / 2.0f; // 0.0 to 1.0
    renderer->renderRect(50.0f, 50.0f, 80.0f, 80.0f,
                        colorValue, 0.0f, 1.0f - colorValue, 1.0f);  // Purple-blue blend
    
    // Demo sprite rendering (will render as colored rect until texture system exists)
    renderer->renderSprite(screenWidth - 150.0f, screenHeight - 150.0f, 0.0f,
                          100.0f, 100.0f, 42);  // Texture ID 42
    
    // End rendering
    renderer->endFrame();
    
    // Log occasionally for debugging
    static int frameCount = 0;
    frameCount++;
    if (frameCount % 300 == 0) { // Every 5 seconds at 60 FPS
        LOG_DEBUG("DemoSystem", "Rendered frame at time");
    }
}