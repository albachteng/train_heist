#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>

// Engine includes  
#include "../../engine/ecs/include/EntityManager.hpp"
#include "../../engine/ecs/systems/include/SystemManager.hpp"
#include "../../engine/logging/include/Logger.hpp"
#include "../../engine/rendering/include/SFMLRenderer.hpp"

// Game includes
#include "../systems/DemoRenderSystem.hpp"

/**
 * Main entry point for Train Heist game
 * 
 * This implements Track 1: Minimal Viable Display
 * - Basic SFML window with main loop
 * - Integration with ECS SystemManager and EntityManager
 * - Proper separation: game-specific logic in game/, engine logic in engine/
 */

int main() {
    // Initialize logging
    auto consoleOutput = std::make_unique<Engine::ConsoleOutput>();
    auto logger = std::make_unique<Engine::Logger>(std::move(consoleOutput), Engine::LogLevel::INFO);
    Engine::GlobalLogger::setLogger(std::move(logger));
    
    LOG_INFO("Game", "Starting Train Heist game...");
    
    // Create SFML window
    const unsigned int WINDOW_WIDTH = 800;
    const unsigned int WINDOW_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)), "Train Heist - Prototype");
    window.setFramerateLimit(60);
    
    LOG_INFO("Game", "Created window");
    
    // Initialize ECS managers
    ECS::EntityManager entityManager;
    ECS::SystemManager systemManager;
    
    // Initialize renderer
    ECS::SFMLRenderer renderer(window);
    
    // Register demo rendering system
    auto demoSystem = std::make_unique<DemoRenderSystem>(&renderer);
    systemManager.registerSystem(std::move(demoSystem));
    
    LOG_INFO("Game", "Initialized ECS managers and rendering system");
    
    // Basic game state
    bool running = true;
    sf::Clock clock;
    
    LOG_INFO("Game", "Entering main game loop");
    
    // Main game loop
    while (running && window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        // Handle window events
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                running = false;
                LOG_INFO("Game", "Window close event received");
            }
            
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scan::Escape) {
                    running = false;
                    LOG_INFO("Game", "Escape key pressed - exiting");
                }
            }
        }
        
        // Update ECS systems (includes rendering)
        systemManager.updateAll(deltaTime, entityManager);
    }
    
    LOG_INFO("Game", "Game loop ended - shutting down");
    return 0;
}