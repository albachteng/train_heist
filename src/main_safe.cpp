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
#include <chrono>
#include <thread>

/**
 * Train Heist - Safe Integration Demo
 * 
 * This version includes better error handling and X11 error recovery
 * for WSLg compatibility.
 */

using namespace ECS;

int main() {
    std::cout << "Train Heist - Safe Rendering Demo\n";
    std::cout << "==================================\n";
    
    try {
        // Check DISPLAY environment
        const char* display = std::getenv("DISPLAY");
        if (!display) {
            std::cout << "Warning: DISPLAY environment variable not set\n";
            std::cout << "Graphics may not work properly\n";
        } else {
            std::cout << "DISPLAY: " << display << "\n";
        }
        
        // Create core managers
        std::cout << "Creating managers...\n";
        auto windowManager = std::make_unique<SFMLWindowManager>();
        auto resourceManager = std::make_unique<SFMLResourceManager>();
        auto renderer = std::make_unique<SFMLRenderer>(resourceManager.get(), windowManager.get());
        
        // Try to create window with error handling
        std::cout << "Attempting to create window...\n";
        bool windowCreated = false;
        
        try {
            windowCreated = windowManager->createWindow(800, 600, "Train Heist - Safe Demo");
        } catch (const std::exception& e) {
            std::cout << "Window creation exception: " << e.what() << "\n";
        } catch (...) {
            std::cout << "Unknown window creation error\n";
        }
        
        if (!windowCreated) {
            std::cout << "Failed to create window - running in headless mode\n";
            std::cout << "This demonstrates that the ECS systems work without graphics\n";
        } else {
            std::cout << "Window created successfully!\n";
        }
        
        // Create ECS systems regardless of window status
        std::cout << "Setting up ECS systems...\n";
        EntityManager entityManager;
        
        // Create component arrays
        ComponentArray<Position> positionComponents;
        ComponentArray<Renderable> renderableComponents;
        
        std::cout << "Setting up demo scene...\n";
        
        // Get component bits
        uint64_t positionBit = getComponentBit<Position>();
        uint64_t renderableBit = getComponentBit<Renderable>();
        
        // Create demo entities
        std::cout << "Creating entities:\n";
        
        // Red rectangle in top-left
        Entity redRect = entityManager.createEntity();
        positionComponents.add(redRect.id, {50.0f, 50.0f, 0.0f}, positionBit, redRect);
        renderableComponents.add(redRect.id, {100.0f, 100.0f, 1.0f, 0.0f, 0.0f, 1.0f}, renderableBit, redRect);
        std::cout << "  - Red rectangle (50, 50)\n";
        
        // Green rectangle in top-right
        Entity greenRect = entityManager.createEntity();
        positionComponents.add(greenRect.id, {650.0f, 50.0f, 0.0f}, positionBit, greenRect);
        renderableComponents.add(greenRect.id, {100.0f, 100.0f, 0.0f, 1.0f, 0.0f, 1.0f}, renderableBit, greenRect);
        std::cout << "  - Green rectangle (650, 50)\n";
        
        // Blue rectangle in bottom-left
        Entity blueRect = entityManager.createEntity();
        positionComponents.add(blueRect.id, {50.0f, 450.0f, 0.0f}, positionBit, blueRect);
        renderableComponents.add(blueRect.id, {100.0f, 100.0f, 0.0f, 0.0f, 1.0f, 1.0f}, renderableBit, blueRect);
        std::cout << "  - Blue rectangle (50, 450)\n";
        
        // Yellow rectangle in center (for animation)
        Entity yellowRect = entityManager.createEntity();
        positionComponents.add(yellowRect.id, {350.0f, 250.0f, 0.0f}, positionBit, yellowRect);
        renderableComponents.add(yellowRect.id, {100.0f, 100.0f, 1.0f, 1.0f, 0.0f, 0.8f}, renderableBit, yellowRect);
        std::cout << "  - Yellow rectangle (350, 250) - animated\n";
        
        // Semi-transparent purple rectangle overlapping center
        Entity purpleRect = entityManager.createEntity();
        positionComponents.add(purpleRect.id, {300.0f, 200.0f, 0.0f}, positionBit, purpleRect);
        renderableComponents.add(purpleRect.id, {200.0f, 200.0f, 1.0f, 0.0f, 1.0f, 0.5f}, renderableBit, purpleRect);
        std::cout << "  - Purple rectangle (300, 200) - semi-transparent\n";
        
        std::cout << "Created " << entityManager.getActiveEntityCount() << " demo entities\n";
        
        if (windowCreated) {
            std::cout << "Starting render loop...\n";
            std::cout << "Press any key in the window or close it to exit.\n";
        } else {
            std::cout << "Running ECS simulation for 5 seconds...\n";
        }
        
        // Main game loop
        int frameCount = 0;
        auto startTime = std::chrono::steady_clock::now();
        
        while (true) {
            // Check if we should exit (for headless mode)
            if (!windowCreated) {
                auto elapsed = std::chrono::steady_clock::now() - startTime;
                if (elapsed > std::chrono::seconds(5)) {
                    std::cout << "Headless simulation completed.\n";
                    break;
                }
            } else {
                // Check window status
                if (!windowManager->isWindowOpen()) {
                    std::cout << "Window closed, exiting.\n";
                    break;
                }
                
                // Handle events safely
                try {
                    WindowEvent event;
                    while (windowManager->pollEvent(event)) {
                        if (event.type == WindowEventType::Closed) {
                            windowManager->closeWindow();
                            break;
                        }
                        if (event.type == WindowEventType::KeyPressed) {
                            std::cout << "Key pressed: " << event.keyCode << "\n";
                        }
                    }
                } catch (...) {
                    std::cout << "Event handling error, continuing...\n";
                }
            }
            
            // Animate yellow rectangle
            frameCount++;
            if (frameCount % 60 == 0) { // Every second at 60 FPS
                Position* yellowPos = positionComponents.get(yellowRect.id);
                if (yellowPos) {
                    float angle = (frameCount / 60.0f) * 3.14159f * 2.0f / 10.0f; // Full circle over 10 seconds
                    yellowPos->x = 350.0f + 50.0f * std::cos(angle);
                    yellowPos->y = 250.0f + 50.0f * std::sin(angle);
                    
                    if (!windowCreated && frameCount % 120 == 0) {
                        std::cout << "Frame " << frameCount << " - Yellow pos: (" 
                                  << yellowPos->x << ", " << yellowPos->y << ")\n";
                    }
                }
            }
            
            // Render if window exists
            if (windowCreated) {
                try {
                    // Begin rendering frame
                    renderer->beginFrame();
                    renderer->clear();
                    
                    // Manual rendering loop
                    const auto& positions = positionComponents.getComponents();
                    const auto& posEntityIDs = positionComponents.getEntityIDs();
                    
                    // Render entities that have both Position and Renderable components
                    for (size_t i = 0; i < positions.size(); ++i) {
                        EntityID entityId = posEntityIDs[i];
                        
                        const Renderable* renderable = renderableComponents.get(entityId);
                        if (renderable) {
                            const Position& pos = positions[i];
                            renderer->renderRect(pos.x, pos.y, renderable->width, renderable->height, 
                                               renderable->red, renderable->green, renderable->blue, renderable->alpha);
                        }
                    }
                    
                    // End rendering frame
                    renderer->endFrame();
                } catch (const std::exception& e) {
                    std::cout << "Rendering error: " << e.what() << "\n";
                    break;
                } catch (...) {
                    std::cout << "Unknown rendering error\n";
                    break;
                }
            }
            
            // Print status occasionally
            if (frameCount % 300 == 0) {
                std::cout << "Frame " << frameCount << " - Entities: " << entityManager.getActiveEntityCount();
                if (windowCreated) {
                    std::cout << " - Rendering active";
                } else {
                    std::cout << " - Headless mode";
                }
                std::cout << "\n";
            }
            
            // Small delay to prevent maxing CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }
        
        std::cout << "Demo completed successfully!\n";
        std::cout << "Final stats:\n";
        std::cout << "  - Frames processed: " << frameCount << "\n";
        std::cout << "  - Entities managed: " << entityManager.getActiveEntityCount() << "\n";
        std::cout << "  - Window mode: " << (windowCreated ? "Graphics" : "Headless") << "\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Demo failed with exception: " << e.what() << "\n";
        return -1;
    } catch (...) {
        std::cerr << "Demo failed with unknown exception!\n";
        return -1;
    }
}