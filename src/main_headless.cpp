#include "MockRenderer.hpp"
#include "MockResourceManager.hpp"
#include "MockWindowManager.hpp"
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
#include <chrono>
#include <thread>

/**
 * Train Heist - Headless Demo
 * 
 * This version runs without any graphics, using mock objects
 * to demonstrate the complete ECS pipeline working.
 */

using namespace ECS;

int main() {
    std::cout << "Train Heist - Headless ECS Demo\n";
    std::cout << "================================\n";
    std::cout << "Running complete ECS simulation without graphics\n\n";
    
    try {
        // Create mock managers (no X11 dependencies)
        std::cout << "Creating mock rendering system...\n";
        auto windowManager = std::make_unique<MockWindowManager>();
        auto resourceManager = std::make_unique<MockResourceManager>();
        auto renderer = std::make_unique<MockRenderer>();
        
        // Configure mock window
        windowManager->setCreateWindowResult(true);
        windowManager->setWindowSize(800, 600);
        
        std::cout << "Setting up ECS systems...\n";
        EntityManager entityManager;
        SystemManager systemManager;
        
        // Create component arrays
        ComponentArray<Position> positionComponents;
        ComponentArray<Renderable> renderableComponents;
        
        // Create and register render system
        auto renderSystem = std::make_unique<RenderSystem>(renderer.get());
        systemManager.registerSystem(std::move(renderSystem));
        
        std::cout << "Setting up demo scene...\n";
        
        // Get component bits
        uint64_t positionBit = getComponentBit<Position>();
        uint64_t renderableBit = getComponentBit<Renderable>();
        
        // Create demo entities
        std::cout << "Creating entities:\n";
        
        // Red rectangle
        Entity redRect = entityManager.createEntity();
        positionComponents.add(redRect.id, {50.0f, 50.0f, 0.0f}, positionBit, redRect);
        renderableComponents.add(redRect.id, {100.0f, 100.0f, 1.0f, 0.0f, 0.0f, 1.0f}, renderableBit, redRect);
        std::cout << "  - Entity " << redRect.id << ": Red rectangle at (50, 50)\n";
        
        // Green rectangle
        Entity greenRect = entityManager.createEntity();
        positionComponents.add(greenRect.id, {650.0f, 50.0f, 0.0f}, positionBit, greenRect);
        renderableComponents.add(greenRect.id, {100.0f, 100.0f, 0.0f, 1.0f, 0.0f, 1.0f}, renderableBit, greenRect);
        std::cout << "  - Entity " << greenRect.id << ": Green rectangle at (650, 50)\n";
        
        // Blue rectangle
        Entity blueRect = entityManager.createEntity();
        positionComponents.add(blueRect.id, {50.0f, 450.0f, 0.0f}, positionBit, blueRect);
        renderableComponents.add(blueRect.id, {100.0f, 100.0f, 0.0f, 0.0f, 1.0f, 1.0f}, renderableBit, blueRect);
        std::cout << "  - Entity " << blueRect.id << ": Blue rectangle at (50, 450)\n";
        
        // Yellow rectangle (animated)
        Entity yellowRect = entityManager.createEntity();
        positionComponents.add(yellowRect.id, {350.0f, 250.0f, 0.0f}, positionBit, yellowRect);
        renderableComponents.add(yellowRect.id, {100.0f, 100.0f, 1.0f, 1.0f, 0.0f, 0.8f}, renderableBit, yellowRect);
        std::cout << "  - Entity " << yellowRect.id << ": Yellow rectangle at (350, 250) - will animate\n";
        
        // Purple rectangle (semi-transparent)
        Entity purpleRect = entityManager.createEntity();
        positionComponents.add(purpleRect.id, {300.0f, 200.0f, 0.0f}, positionBit, purpleRect);
        renderableComponents.add(purpleRect.id, {200.0f, 200.0f, 1.0f, 0.0f, 1.0f, 0.5f}, renderableBit, purpleRect);
        std::cout << "  - Entity " << purpleRect.id << ": Purple rectangle at (300, 200) - semi-transparent\n";
        
        std::cout << "\nCreated " << entityManager.getActiveEntityCount() << " entities\n";
        std::cout << "Component registry has " << getRegisteredComponentCount() << " component types\n\n";
        
        std::cout << "Starting ECS simulation (10 seconds)...\n";
        std::cout << "Watch the yellow rectangle animate!\n\n";
        
        // Simulation loop
        int frameCount = 0;
        auto startTime = std::chrono::steady_clock::now();
        
        while (true) {
            auto elapsed = std::chrono::steady_clock::now() - startTime;
            if (elapsed > std::chrono::seconds(10)) {
                break;
            }
            
            // Simulate frame processing
            frameCount++;
            float deltaTime = 1.0f / 60.0f;
            
            // Update systems using the SystemManager
            systemManager.updateAll(deltaTime, entityManager);
            
            // Animate yellow rectangle
            if (frameCount % 30 == 0) { // Every 0.5 seconds
                Position* yellowPos = positionComponents.get(yellowRect.id);
                if (yellowPos) {
                    float angle = (frameCount / 30.0f) * 3.14159f * 2.0f / 20.0f; // Full circle over 10 seconds
                    float newX = 350.0f + 80.0f * std::cos(angle);
                    float newY = 250.0f + 80.0f * std::sin(angle);
                    
                    std::cout << "Frame " << frameCount << " - Yellow entity " << yellowRect.id 
                              << " moved to (" << newX << ", " << newY << ")\n";
                    
                    yellowPos->x = newX;
                    yellowPos->y = newY;
                }
            }
            
            // Show render stats every 2 seconds
            if (frameCount % 120 == 0) {
                std::cout << "  Stats: Frame " << frameCount 
                          << " | Entities: " << entityManager.getActiveEntityCount()
                          << " | Positions: " << positionComponents.size()
                          << " | Renderables: " << renderableComponents.size()
                          << " | Mock render calls: " << renderer->rectCalls.size() << "\n";
            }
            
            // Simulate 60 FPS
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        
        std::cout << "\nSimulation completed!\n";
        std::cout << "===================\n";
        std::cout << "Final Stats:\n";
        std::cout << "  - Total frames: " << frameCount << "\n";
        std::cout << "  - Entities created: " << entityManager.getActiveEntityCount() << "\n";
        std::cout << "  - Position components: " << positionComponents.size() << "\n";
        std::cout << "  - Renderable components: " << renderableComponents.size() << "\n";
        std::cout << "  - Mock render calls: " << renderer->rectCalls.size() << "\n";
        std::cout << "  - Component types registered: " << getRegisteredComponentCount() << "\n";
        
        // Show final positions
        std::cout << "\nFinal entity positions:\n";
        const auto& positions = positionComponents.getComponents();
        const auto& posEntityIDs = positionComponents.getEntityIDs();
        
        for (size_t i = 0; i < positions.size(); ++i) {
            EntityID entityId = posEntityIDs[i];
            const Position& pos = positions[i];
            const Renderable* renderable = renderableComponents.get(entityId);
            
            std::cout << "  - Entity " << entityId << ": (" << pos.x << ", " << pos.y << ", " << pos.z << ")";
            if (renderable) {
                std::cout << " - Color: (" << renderable->red << ", " << renderable->green 
                          << ", " << renderable->blue << ", " << renderable->alpha << ")";
            }
            std::cout << "\n";
        }
        
        std::cout << "\nThe ECS system is working perfectly!\n";
        std::cout << "To see graphics, set up X11 forwarding and run 'make safe'\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Demo failed with exception: " << e.what() << "\n";
        return -1;
    } catch (...) {
        std::cerr << "Demo failed with unknown exception!\n";
        return -1;
    }
}