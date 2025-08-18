#include "SFMLRenderer.hpp"
#include "SFMLResourceManager.hpp"
#include "SFMLWindowManager.hpp"
#include "RenderSystem.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"
#include "ComponentArray.hpp"
#include "ComponentRegistry.hpp"
#include "Transform.hpp"
#include "Rendering.hpp"
#include <iostream>
#include <memory>
#include <cmath>

/**
 * Train Heist - Basic Integration Demo
 * 
 * Demonstrates the complete rendering pipeline:
 * 1. SFML window creation and management
 * 2. ECS entity creation with Position and Renderable components
 * 3. Manual rendering of entities (simplified for demo)
 * 4. Real SFML graphics output with colored rectangles
 */

using namespace ECS;

int main() {
    std::cout << "Train Heist - Basic Rendering Demo\n";
    std::cout << "===================================\n";
    
    try {
        // Create core managers
        auto windowManager = std::make_unique<SFMLWindowManager>();
        auto resourceManager = std::make_unique<SFMLResourceManager>();
        auto renderer = std::make_unique<SFMLRenderer>(resourceManager.get(), windowManager.get());
        
        // Create window
        std::cout << "Creating window...\n";
        if (!windowManager->createWindow(800, 600, "Train Heist - Basic Demo")) {
            std::cerr << "Failed to create window!\n";
            return -1;
        }
        
        // Create ECS systems
        EntityManager entityManager;
        
        // Create component arrays
        ComponentArray<Position> positionComponents;
        ComponentArray<Renderable> renderableComponents;
        
        std::cout << "Setting up demo scene...\n";
        
        // Get component bits
        uint64_t positionBit = getComponentBit<Position>();
        uint64_t renderableBit = getComponentBit<Renderable>();
        
        // Create some demo entities with rectangles (no texture files needed)
        
        // Red rectangle in top-left
        Entity redRect = entityManager.createEntity();
        positionComponents.add(redRect.id, {50.0f, 50.0f, 0.0f}, positionBit, redRect);
        renderableComponents.add(redRect.id, {100.0f, 100.0f, 1.0f, 0.0f, 0.0f, 1.0f}, renderableBit, redRect);
        
        // Green rectangle in top-right
        Entity greenRect = entityManager.createEntity();
        positionComponents.add(greenRect.id, {650.0f, 50.0f, 0.0f}, positionBit, greenRect);
        renderableComponents.add(greenRect.id, {100.0f, 100.0f, 0.0f, 1.0f, 0.0f, 1.0f}, renderableBit, greenRect);
        
        // Blue rectangle in bottom-left
        Entity blueRect = entityManager.createEntity();
        positionComponents.add(blueRect.id, {50.0f, 450.0f, 0.0f}, positionBit, blueRect);
        renderableComponents.add(blueRect.id, {100.0f, 100.0f, 0.0f, 0.0f, 1.0f, 1.0f}, renderableBit, blueRect);
        
        // Yellow rectangle in center (for animation)
        Entity yellowRect = entityManager.createEntity();
        positionComponents.add(yellowRect.id, {350.0f, 250.0f, 0.0f}, positionBit, yellowRect);
        renderableComponents.add(yellowRect.id, {100.0f, 100.0f, 1.0f, 1.0f, 0.0f, 0.8f}, renderableBit, yellowRect);
        
        // Semi-transparent purple rectangle overlapping center
        Entity purpleRect = entityManager.createEntity();
        positionComponents.add(purpleRect.id, {300.0f, 200.0f, 0.0f}, positionBit, purpleRect);
        renderableComponents.add(purpleRect.id, {200.0f, 200.0f, 1.0f, 0.0f, 1.0f, 0.5f}, renderableBit, purpleRect);
        
        std::cout << "Created " << entityManager.getActiveEntityCount() << " demo entities\n";
        std::cout << "Starting render loop...\n";
        std::cout << "Close window to exit.\n";
        
        // Main game loop
        int frameCount = 0;
        while (windowManager->isWindowOpen()) {
            // Handle events
            WindowEvent event;
            while (windowManager->pollEvent(event)) {
                if (event.type == WindowEventType::Closed) {
                    windowManager->closeWindow();
                }
                if (event.type == WindowEventType::KeyPressed) {
                    std::cout << "Key pressed: " << event.keyCode << "\n";
                }
            }
            
            // Begin rendering frame
            renderer->beginFrame();
            renderer->clear();
            
            // Manual rendering loop (simplified for demo)
            const auto& positions = positionComponents.getComponents();
            const auto& posEntityIDs = positionComponents.getEntityIDs();
            
            // Render entities that have both Position and Renderable components
            for (size_t i = 0; i < positions.size(); ++i) {
                EntityID entityId = posEntityIDs[i];
                
                // Check if this entity also has a Renderable component
                const Renderable* renderable = renderableComponents.get(entityId);
                if (renderable) {
                    const Position& pos = positions[i];
                    renderer->renderRect(pos.x, pos.y, renderable->width, renderable->height, 
                                       renderable->red, renderable->green, renderable->blue, renderable->alpha);
                }
            }
            
            // End rendering frame
            renderer->endFrame();
            
            // Simple animation: move yellow rectangle in a circle
            frameCount++;
            if (frameCount % 120 == 0) { // Every 2 seconds at 60 FPS
                Position* yellowPos = positionComponents.get(yellowRect.id);
                if (yellowPos) {
                    float angle = (frameCount / 120.0f) * 3.14159f * 2.0f / 10.0f; // Full circle over 20 seconds
                    yellowPos->x = 350.0f + 100.0f * std::cos(angle);
                    yellowPos->y = 250.0f + 100.0f * std::sin(angle);
                }
            }
            
            // Print frame info occasionally
            if (frameCount % 300 == 0) { // Every 5 seconds
                std::cout << "Frame " << frameCount << " - Entities: " << entityManager.getActiveEntityCount() << "\n";
            }
        }
        
        std::cout << "Demo completed successfully!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Demo failed with exception: " << e.what() << "\n";
        return -1;
    } catch (...) {
        std::cerr << "Demo failed with unknown exception!\n";
        return -1;
    }
}