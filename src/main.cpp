#include "ComponentArray.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.h"
#include "EntityManager.hpp"
#include "Logger.hpp"
#include "RenderSystem.hpp"
#include "Rendering.hpp"
#include "SFMLRenderer.hpp"
#include "SFMLResourceManager.hpp"
#include "SFMLWindowManager.hpp"
#include "../engine/input/include/SFMLInputManager.hpp"
#include "../engine/ecs/systems/include/InputSystem.hpp"
#include "SystemManager.hpp"
#include "Transform.hpp"
#include <cmath>
#include <memory>

/**
 * Train Heist - Interactive Input Demo
 *
 * Demonstrates the complete engine pipeline:
 * 1. SFML window creation and management
 * 2. ECS entity creation with Position and Renderable components
 * 3. Input system integration with keyboard and mouse handling
 * 4. Interactive entity control with arrow keys
 * 5. Mouse click logging and real SFML graphics output
 */

using namespace ECS;

int main() {
  // Initialize logging system
  auto consoleOutput = std::make_unique<Engine::ConsoleOutput>();
  auto logger = std::make_unique<Engine::Logger>(std::move(consoleOutput),
                                                 Engine::LogLevel::INFO);
  Engine::GlobalLogger::setLogger(std::move(logger));

  LOG_INFO("Main", "Train Heist - Interactive Input Demo");
  LOG_INFO("Main", "====================================");

  try {
    // Create core managers
    auto windowManager = std::make_unique<SFMLWindowManager>();
    auto resourceManager = std::make_unique<SFMLResourceManager>();
    auto renderer = std::make_unique<SFMLRenderer>(resourceManager.get(),
                                                   windowManager.get());
    auto inputManager = std::make_unique<SFMLInputManager>(windowManager.get());

    // Create window
    LOG_INFO("Main", "Creating window...");
    if (!windowManager->createWindow(800, 600, "Train Heist - Interactive Demo")) {
      LOG_ERROR("Main", "Failed to create window!");
      return -1;
    }

    // Create ECS systems
    EntityManager entityManager;
    auto inputSystem = std::make_unique<InputSystem>(inputManager.get());

    // Create component arrays
    ComponentArray<Position> positionComponents;
    ComponentArray<Renderable> renderableComponents;

    LOG_INFO("ECS", "Setting up interactive demo scene...");

    // Get component bits
    uint64_t positionBit = getComponentBit<Position>();
    uint64_t renderableBit = getComponentBit<Renderable>();

    // Create some demo entities with rectangles (no texture files needed)

    // Red rectangle in top-left - CONTROLLABLE with arrow keys
    Entity redRect = entityManager.createEntity();
    positionComponents.add(redRect.id, {50.0f, 50.0f, 0.0f}, positionBit,
                           redRect);
    renderableComponents.add(redRect.id,
                             {100.0f, 100.0f, 1.0f, 0.0f, 0.0f, 1.0f},
                             renderableBit, redRect);
    
    // Set red rectangle as the controllable entity
    inputSystem->setControlledEntity(redRect.id);
    inputSystem->setMovementSpeed(5.0f); // Moderate movement speed

    // Green rectangle in top-right
    Entity greenRect = entityManager.createEntity();
    positionComponents.add(greenRect.id, {650.0f, 50.0f, 0.0f}, positionBit,
                           greenRect);
    renderableComponents.add(greenRect.id,
                             {100.0f, 100.0f, 0.0f, 1.0f, 0.0f, 1.0f},
                             renderableBit, greenRect);

    // Blue rectangle in bottom-left
    Entity blueRect = entityManager.createEntity();
    positionComponents.add(blueRect.id, {50.0f, 450.0f, 0.0f}, positionBit,
                           blueRect);
    renderableComponents.add(blueRect.id,
                             {100.0f, 100.0f, 0.0f, 0.0f, 1.0f, 1.0f},
                             renderableBit, blueRect);

    // Yellow rectangle in center (for animation)
    Entity yellowRect = entityManager.createEntity();
    positionComponents.add(yellowRect.id, {350.0f, 250.0f, 0.0f}, positionBit,
                           yellowRect);
    renderableComponents.add(yellowRect.id,
                             {100.0f, 100.0f, 1.0f, 1.0f, 0.0f, 0.8f},
                             renderableBit, yellowRect);

    // Semi-transparent purple rectangle overlapping center
    Entity purpleRect = entityManager.createEntity();
    positionComponents.add(purpleRect.id, {300.0f, 200.0f, 0.0f}, positionBit,
                           purpleRect);
    renderableComponents.add(purpleRect.id,
                             {200.0f, 200.0f, 1.0f, 0.0f, 1.0f, 0.5f},
                             renderableBit, purpleRect);

    LOG_INFO("ECS", "Created " +
                        std::to_string(entityManager.getActiveEntityCount()) +
                        " demo entities");
    LOG_INFO("Main", "=== INTERACTIVE DEMO CONTROLS ===");
    LOG_INFO("Main", "- Use ARROW KEYS to move the red rectangle");
    LOG_INFO("Main", "- Click LEFT/RIGHT mouse buttons to see click logging");
    LOG_INFO("Main", "- Press ESCAPE or close window to exit");
    LOG_INFO("Main", "Starting interactive demo...");

    // Main game loop
    int frameCount = 0;
    while (windowManager->isWindowOpen()) {
      // Let the input system process ALL events
      inputSystem->update(entityManager, 1.0f);
      
      // Check if window close was requested or ESC key was pressed
      if (inputManager->wasWindowCloseRequested() || inputManager->wasKeyPressed(KeyCode::Escape)) {
        if (inputManager->wasKeyPressed(KeyCode::Escape)) {
          LOG_INFO("Main", "Escape key pressed - closing demo");
        }
        if (inputManager->wasWindowCloseRequested()) {
          LOG_INFO("Main", "Window close requested - closing demo");
        }
        windowManager->closeWindow();
      }
      
      // Handle keyboard input for controlled entity manually 
      // (since our InputSystem logs but doesn't move entities directly)
      EntityID controlledId = inputSystem->getControlledEntity();
      if (controlledId != INVALID_ENTITY) {
        Position* controlledPos = positionComponents.get(controlledId);
        if (controlledPos) {
          float moveSpeed = inputSystem->getMovementSpeed();
          
          if (inputManager->isKeyPressed(KeyCode::Left)) {
            controlledPos->x -= moveSpeed;
          }
          if (inputManager->isKeyPressed(KeyCode::Right)) {
            controlledPos->x += moveSpeed;
          }
          if (inputManager->isKeyPressed(KeyCode::Up)) {
            controlledPos->y -= moveSpeed;
          }
          if (inputManager->isKeyPressed(KeyCode::Down)) {
            controlledPos->y += moveSpeed;
          }
          
          // Keep entity within window bounds
          if (controlledPos->x < 0) controlledPos->x = 0;
          if (controlledPos->y < 0) controlledPos->y = 0;
          if (controlledPos->x > 700) controlledPos->x = 700; // 800 - 100 (width)
          if (controlledPos->y > 500) controlledPos->y = 500; // 600 - 100 (height)
        }
      }

      // Begin rendering frame
      renderer->beginFrame();
      renderer->clear();

      // Manual rendering loop (simplified for demo)
      const auto &positions = positionComponents.getComponents();
      const auto &posEntityIDs = positionComponents.getEntityIDs();

      // Render entities that have both Position and Renderable components
      for (size_t i = 0; i < positions.size(); ++i) {
        EntityID entityId = posEntityIDs[i];

        // Check if this entity also has a Renderable component
        const Renderable *renderable = renderableComponents.get(entityId);
        if (renderable) {
          const Position &pos = positions[i];
          renderer->renderRect(pos.x, pos.y, renderable->width,
                               renderable->height, renderable->red,
                               renderable->green, renderable->blue,
                               renderable->alpha);
        }
      }

      // End rendering frame
      renderer->endFrame();

      // Simple animation: move yellow rectangle in a circle
      frameCount++;
      if (frameCount % 120 == 0) { // Every 2 seconds at 60 FPS
        Position *yellowPos = positionComponents.get(yellowRect.id);
        if (yellowPos) {
          float angle = (frameCount / 120.0f) * 3.14159f * 2.0f /
                        10.0f; // Full circle over 20 seconds
          yellowPos->x = 350.0f + 100.0f * std::cos(angle);
          yellowPos->y = 250.0f + 100.0f * std::sin(angle);
        }
      }

      // Print frame info occasionally
      if (frameCount % 300 == 0) { // Every 5 seconds
        LOG_INFO("Render",
                 "Frame " + std::to_string(frameCount) + " - Entities: " +
                     std::to_string(entityManager.getActiveEntityCount()));
      }
    }

    LOG_INFO("Main", "Demo completed successfully!");
    return 0;

  } catch (const std::exception &e) {
    LOG_ERROR("Main", "Demo failed with exception: " + std::string(e.what()));
    return -1;
  } catch (...) {
    LOG_ERROR("Main", "Demo failed with unknown exception!");
    return -1;
  }
}
