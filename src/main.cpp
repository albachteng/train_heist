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
#include "../engine/physics/include/GridMovement.hpp"
#include "../engine/physics/include/MovementSystem.hpp"
#include "SystemManager.hpp"
#include "Transform.hpp"
#include <cmath>
#include <memory>

/**
 * Train Heist - Grid-Based Movement Demo
 *
 * Demonstrates the complete engine pipeline:
 * 1. SFML window creation and management
 * 2. ECS entity creation with Position, Renderable, GridMovement, and GridBounds
 * 3. Grid-based physics with smooth visual interpolation
 * 4. MovementSystem with queued movement requests
 * 5. Interactive grid navigation with arrow keys
 * 6. Mouse click logging and real SFML graphics output
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
    if (!windowManager->createWindow(800, 600, "Train Heist - Sprite Demo")) {
      LOG_ERROR("Main", "Failed to create window!");
      return -1;
    }

    // Load sprite textures
    LOG_INFO("Main", "Loading sprite textures...");
    TextureHandle enemyTexture = resourceManager->loadTexture("assets/sprites/enemy_sprite.gif");
    TextureHandle trainTexture = resourceManager->loadTexture("assets/sprites/train_sprite.png");
    TextureHandle landscapeTexture = resourceManager->loadTexture("assets/sprites/landscape_tiles.png");

    if (enemyTexture == INVALID_TEXTURE) {
      LOG_ERROR("Main", "Failed to load enemy sprite!");
      return -1;
    }
    if (trainTexture == INVALID_TEXTURE) {
      LOG_ERROR("Main", "Failed to load train sprite!");
      return -1;
    }
    if (landscapeTexture == INVALID_TEXTURE) {
      LOG_ERROR("Main", "Failed to load landscape tiles!");
      return -1;
    }

    LOG_INFO("Main", "All sprites loaded successfully!");

    // Create ECS systems
    EntityManager entityManager;
    auto inputSystem = std::make_unique<InputSystem>(inputManager.get());

    // Create component arrays
    ComponentArray<Position> positionComponents;
    ComponentArray<Sprite> spriteComponents;
    ComponentArray<Renderable> renderableComponents;
    ComponentArray<GridPosition> gridPositionComponents;
    ComponentArray<GridMovement> gridMovementComponents;
    ComponentArray<GridBounds> gridBoundsComponents;

    // Create MovementSystem with component array pointers
    auto movementSystem = std::make_unique<MovementSystem>(
        &positionComponents,
        &gridPositionComponents,
        &gridMovementComponents,
        nullptr,  // velocities
        nullptr,  // accelerations
        nullptr,  // constraints
        &gridBoundsComponents,
        nullptr   // inputManager
    );

    // Configure grid cell size for coordinate conversion
    const float CELL_SIZE = 64.0f;
    movementSystem->setGridCellSize(CELL_SIZE);

    // Create RenderSystem with component array pointers
    auto renderSystem = std::make_unique<RenderSystem>(
        renderer.get(),
        &positionComponents,
        &spriteComponents,
        &renderableComponents
    );

    LOG_INFO("ECS", "Setting up sprite-based grid demo...");

    // Get component bits
    uint64_t positionBit = getComponentBit<Position>();
    uint64_t spriteBit = getComponentBit<Sprite>();
    uint64_t gridPositionBit = getComponentBit<GridPosition>();
    uint64_t gridMovementBit = getComponentBit<GridMovement>();
    uint64_t gridBoundsBit = getComponentBit<GridBounds>();

    // Grid configuration (matches MovementSystem cell size)
    const int GRID_WIDTH = 10;      // 10 cells wide (640 pixels)
    const int GRID_HEIGHT = 8;      // 8 cells tall (512 pixels)

    // Create demo entities with sprite textures

    // Player entity - CONTROLLABLE with arrow keys on GRID (using enemy sprite)
    Entity playerEntity = entityManager.createEntity();

    // Start at grid position (1, 1)
    GridPosition startGridPos = {1, 1};
    positionComponents.add(playerEntity.id,
                          {startGridPos.x * CELL_SIZE, startGridPos.y * CELL_SIZE, 0.0f},
                          positionBit, entityManager);
    spriteComponents.add(playerEntity.id,
                        {enemyTexture, CELL_SIZE, CELL_SIZE},
                        spriteBit, entityManager);
    gridPositionComponents.add(playerEntity.id, startGridPos, gridPositionBit, entityManager);
    gridMovementComponents.add(playerEntity.id, GridMovement(), gridMovementBit, entityManager);
    gridBoundsComponents.add(playerEntity.id,
                            GridBounds(0, 0, GRID_WIDTH - 1, GRID_HEIGHT - 1),
                            gridBoundsBit, entityManager);

    // Set player entity as the controllable entity
    inputSystem->setControlledEntity(playerEntity.id);
    LOG_INFO("Demo", "Player entity: Use ARROW KEYS to move on grid (cell size: " +
             std::to_string((int)CELL_SIZE) + "px)");

    // Landscape entity at top-right corner of grid (9, 0)
    Entity landscapeEntity1 = entityManager.createEntity();
    GridPosition landscapePos1 = {9, 0};
    positionComponents.add(landscapeEntity1.id,
                          {landscapePos1.x * CELL_SIZE, landscapePos1.y * CELL_SIZE, 0.0f},
                          positionBit, entityManager);
    spriteComponents.add(landscapeEntity1.id,
                        {landscapeTexture, CELL_SIZE, CELL_SIZE},
                        spriteBit, entityManager);
    gridPositionComponents.add(landscapeEntity1.id, landscapePos1, gridPositionBit, entityManager);

    // Landscape entity at bottom-left (0, 7)
    Entity landscapeEntity2 = entityManager.createEntity();
    GridPosition landscapePos2 = {0, 7};
    positionComponents.add(landscapeEntity2.id,
                          {landscapePos2.x * CELL_SIZE, landscapePos2.y * CELL_SIZE, 0.0f},
                          positionBit, entityManager);
    spriteComponents.add(landscapeEntity2.id,
                        {landscapeTexture, CELL_SIZE, CELL_SIZE},
                        spriteBit, entityManager);
    gridPositionComponents.add(landscapeEntity2.id, landscapePos2, gridPositionBit, entityManager);

    // Train entity - AUTOMATIC MOVEMENT (moves in rectangular pattern)
    Entity trainEntity = entityManager.createEntity();
    GridPosition trainGridPos = {4, 3};  // Start at top-left of pattern
    positionComponents.add(trainEntity.id,
                          {trainGridPos.x * CELL_SIZE, trainGridPos.y * CELL_SIZE, 0.0f},
                          positionBit, entityManager);
    spriteComponents.add(trainEntity.id,
                        {trainTexture, CELL_SIZE, CELL_SIZE},
                        spriteBit, entityManager);
    gridPositionComponents.add(trainEntity.id, trainGridPos, gridPositionBit, entityManager);
    gridMovementComponents.add(trainEntity.id, GridMovement(), gridMovementBit, entityManager);
    gridBoundsComponents.add(trainEntity.id,
                            GridBounds(0, 0, GRID_WIDTH - 1, GRID_HEIGHT - 1),
                            gridBoundsBit, entityManager);

    // Landscape entity in center (5, 4)
    Entity landscapeEntity3 = entityManager.createEntity();
    GridPosition landscapePos3 = {5, 4};
    positionComponents.add(landscapeEntity3.id,
                          {landscapePos3.x * CELL_SIZE, landscapePos3.y * CELL_SIZE, 0.0f},
                          positionBit, entityManager);
    spriteComponents.add(landscapeEntity3.id,
                        {landscapeTexture, CELL_SIZE, CELL_SIZE},
                        spriteBit, entityManager);
    gridPositionComponents.add(landscapeEntity3.id, landscapePos3, gridPositionBit, entityManager);

    LOG_INFO("ECS", "Created " +
                        std::to_string(entityManager.getActiveEntityCount()) +
                        " demo entities");
    LOG_INFO("Main", "=== SPRITE-BASED GRID MOVEMENT DEMO ===");
    LOG_INFO("Main", "Grid Configuration:");
    LOG_INFO("Main", "  - Size: " + std::to_string(GRID_WIDTH) + "x" +
             std::to_string(GRID_HEIGHT) + " cells (" +
             std::to_string((int)CELL_SIZE) + "px each)");
    LOG_INFO("Main", "  - Total area: " + std::to_string(GRID_WIDTH * (int)CELL_SIZE) + "x" +
             std::to_string(GRID_HEIGHT * (int)CELL_SIZE) + " pixels");
    LOG_INFO("Main", "Entity Sprites:");
    LOG_INFO("Main", "  - PLAYER (enemy sprite, controllable): grid (1, 1)");
    LOG_INFO("Main", "  - LANDSCAPE 1: grid (9, 0) - top-right corner");
    LOG_INFO("Main", "  - LANDSCAPE 2: grid (0, 7) - bottom-left corner");
    LOG_INFO("Main", "  - TRAIN (automatic): grid (4, 3) - moves in rectangular pattern");
    LOG_INFO("Main", "  - LANDSCAPE 3: grid (5, 4) - center");
    LOG_INFO("Main", "Controls:");
    LOG_INFO("Main", "  - Use ARROW KEYS to move player entity on grid");
    LOG_INFO("Main", "  - Train moves automatically to demonstrate grid system");
    LOG_INFO("Main", "  - Movement uses smooth interpolation between cells");
    LOG_INFO("Main", "  - Press ESCAPE or close window to exit");
    LOG_INFO("Main", "Starting sprite-based grid demo...");

    // Automatic movement pattern for train entity (rectangular loop)
    const GridPosition autoMovementPattern[] = {
        {4, 3},  // Top-left
        {6, 3},  // Top-right
        {6, 5},  // Bottom-right
        {4, 5}   // Bottom-left
    };
    const int patternSize = 4;
    int currentPatternIndex = 0;

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
      
      // Handle grid-based keyboard input for controlled entity
      EntityID controlledId = inputSystem->getControlledEntity();
      if (controlledId != INVALID_ENTITY) {
        GridPosition* gridPos = gridPositionComponents.get(controlledId);
        GridMovement* gridMove = gridMovementComponents.get(controlledId);
        GridBounds* bounds = gridBoundsComponents.get(controlledId);

        if (gridPos && gridMove && bounds) {
          // Only queue new movement if not currently moving
          if (!gridMove->isMoving) {
            int targetX = gridPos->x;
            int targetY = gridPos->y;

            if (inputManager->wasKeyPressed(KeyCode::Left)) {
              targetX = gridPos->x - 1;
              targetY = gridPos->y;
            } else if (inputManager->wasKeyPressed(KeyCode::Right)) {
              targetX = gridPos->x + 1;
              targetY = gridPos->y;
            } else if (inputManager->wasKeyPressed(KeyCode::Up)) {
              targetX = gridPos->x;
              targetY = gridPos->y - 1;
            } else if (inputManager->wasKeyPressed(KeyCode::Down)) {
              targetX = gridPos->x;
              targetY = gridPos->y + 1;
            }

            // Request movement if target changed and is valid
            if ((targetX != gridPos->x || targetY != gridPos->y) &&
                bounds->isValid(targetX, targetY)) {
              gridMove->targetX = targetX;
              gridMove->targetY = targetY;
              gridMove->progress = 0.0f;
              gridMove->isMoving = true;
              LOG_INFO("Movement", "Moving to grid cell (" + std::to_string(targetX) +
                       ", " + std::to_string(targetY) + ")");
            }
          }
        }
      }

      // Automatic movement for train entity (demonstrates grid system with multiple entities)
      GridPosition* trainGridPos = gridPositionComponents.get(trainEntity.id);
      GridMovement* trainGridMove = gridMovementComponents.get(trainEntity.id);
      GridBounds* trainBounds = gridBoundsComponents.get(trainEntity.id);

      if (trainGridPos && trainGridMove && trainBounds) {
        // If train finished moving, queue next position in pattern
        if (!trainGridMove->isMoving) {
          // Move to next position in pattern
          currentPatternIndex = (currentPatternIndex + 1) % patternSize;
          const GridPosition& nextPos = autoMovementPattern[currentPatternIndex];

          if (trainBounds->isValid(nextPos.x, nextPos.y)) {
            trainGridMove->targetX = nextPos.x;
            trainGridMove->targetY = nextPos.y;
            trainGridMove->progress = 0.0f;
            trainGridMove->isMoving = true;
            LOG_INFO("AutoMovement", "Train moving to grid cell (" +
                     std::to_string(nextPos.x) + ", " + std::to_string(nextPos.y) + ")");
          }
        }
      }

      // Update MovementSystem (handles smooth interpolation)
      movementSystem->update(entityManager, 0.016f);  // ~60 FPS (16ms)

      // Render all entities with sprites using RenderSystem
      renderSystem->update(0.016f, entityManager);

      // Print frame info occasionally
      frameCount++;
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
