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

    LOG_INFO("ECS", "Setting up grid-based movement demo...");

    // Get component bits
    uint64_t positionBit = getComponentBit<Position>();
    uint64_t renderableBit = getComponentBit<Renderable>();
    uint64_t gridPositionBit = getComponentBit<GridPosition>();
    uint64_t gridMovementBit = getComponentBit<GridMovement>();
    uint64_t gridBoundsBit = getComponentBit<GridBounds>();

    // Grid configuration (matches MovementSystem cell size)
    const int GRID_WIDTH = 10;      // 10 cells wide (640 pixels)
    const int GRID_HEIGHT = 8;      // 8 cells tall (512 pixels)

    // Create some demo entities with rectangles (no texture files needed)

    // Red rectangle in top-left - CONTROLLABLE with arrow keys on GRID
    Entity redRect = entityManager.createEntity();

    // Start at grid position (1, 1)
    GridPosition startGridPos = {1, 1};
    positionComponents.add(redRect.id,
                          {startGridPos.x * CELL_SIZE, startGridPos.y * CELL_SIZE, 0.0f},
                          positionBit, entityManager);
    renderableComponents.add(redRect.id,
                             {CELL_SIZE, CELL_SIZE, 1.0f, 0.0f, 0.0f, 1.0f},  // Red, cell-sized
                             renderableBit, entityManager);
    gridPositionComponents.add(redRect.id, startGridPos, gridPositionBit, entityManager);
    gridMovementComponents.add(redRect.id, GridMovement(), gridMovementBit, entityManager);
    gridBoundsComponents.add(redRect.id,
                            GridBounds(0, 0, GRID_WIDTH - 1, GRID_HEIGHT - 1),
                            gridBoundsBit, entityManager);

    // Set red rectangle as the controllable entity
    inputSystem->setControlledEntity(redRect.id);
    LOG_INFO("Demo", "Red square: Use ARROW KEYS to move on grid (cell size: " +
             std::to_string((int)CELL_SIZE) + "px)");

    // Green square at top-right corner of grid (9, 0)
    Entity greenRect = entityManager.createEntity();
    GridPosition greenGridPos = {9, 0};
    positionComponents.add(greenRect.id,
                          {greenGridPos.x * CELL_SIZE, greenGridPos.y * CELL_SIZE, 0.0f},
                          positionBit, entityManager);
    renderableComponents.add(greenRect.id,
                             {CELL_SIZE, CELL_SIZE, 0.0f, 1.0f, 0.0f, 1.0f},  // Green, cell-sized
                             renderableBit, entityManager);
    gridPositionComponents.add(greenRect.id, greenGridPos, gridPositionBit, entityManager);

    // Blue square at bottom-left (0, 7)
    Entity blueRect = entityManager.createEntity();
    GridPosition blueGridPos = {0, 7};
    positionComponents.add(blueRect.id,
                          {blueGridPos.x * CELL_SIZE, blueGridPos.y * CELL_SIZE, 0.0f},
                          positionBit, entityManager);
    renderableComponents.add(blueRect.id,
                             {CELL_SIZE, CELL_SIZE, 0.0f, 0.0f, 1.0f, 1.0f},  // Blue, cell-sized
                             renderableBit, entityManager);
    gridPositionComponents.add(blueRect.id, blueGridPos, gridPositionBit, entityManager);

    // Yellow square - AUTOMATIC MOVEMENT (moves in rectangular pattern)
    Entity yellowRect = entityManager.createEntity();
    GridPosition yellowGridPos = {4, 3};  // Start at top-left of pattern
    positionComponents.add(yellowRect.id,
                          {yellowGridPos.x * CELL_SIZE, yellowGridPos.y * CELL_SIZE, 0.0f},
                          positionBit, entityManager);
    renderableComponents.add(yellowRect.id,
                             {CELL_SIZE, CELL_SIZE, 1.0f, 1.0f, 0.0f, 1.0f},  // Yellow, cell-sized
                             renderableBit, entityManager);
    gridPositionComponents.add(yellowRect.id, yellowGridPos, gridPositionBit, entityManager);
    gridMovementComponents.add(yellowRect.id, GridMovement(), gridMovementBit, entityManager);
    gridBoundsComponents.add(yellowRect.id,
                            GridBounds(0, 0, GRID_WIDTH - 1, GRID_HEIGHT - 1),
                            gridBoundsBit, entityManager);

    // Purple square in center (5, 4)
    Entity purpleRect = entityManager.createEntity();
    GridPosition purpleGridPos = {5, 4};
    positionComponents.add(purpleRect.id,
                          {purpleGridPos.x * CELL_SIZE, purpleGridPos.y * CELL_SIZE, 0.0f},
                          positionBit, entityManager);
    renderableComponents.add(purpleRect.id,
                             {CELL_SIZE, CELL_SIZE, 0.5f, 0.0f, 0.5f, 1.0f},  // Purple, cell-sized
                             renderableBit, entityManager);
    gridPositionComponents.add(purpleRect.id, purpleGridPos, gridPositionBit, entityManager);

    LOG_INFO("ECS", "Created " +
                        std::to_string(entityManager.getActiveEntityCount()) +
                        " demo entities");
    LOG_INFO("Main", "=== GRID-BASED MOVEMENT DEMO ===");
    LOG_INFO("Main", "Grid Configuration:");
    LOG_INFO("Main", "  - Size: " + std::to_string(GRID_WIDTH) + "x" +
             std::to_string(GRID_HEIGHT) + " cells (" +
             std::to_string((int)CELL_SIZE) + "px each)");
    LOG_INFO("Main", "  - Total area: " + std::to_string(GRID_WIDTH * (int)CELL_SIZE) + "x" +
             std::to_string(GRID_HEIGHT * (int)CELL_SIZE) + " pixels");
    LOG_INFO("Main", "Entity Positions:");
    LOG_INFO("Main", "  - RED (controllable): grid (1, 1)");
    LOG_INFO("Main", "  - GREEN: grid (9, 0) - top-right corner");
    LOG_INFO("Main", "  - BLUE: grid (0, 7) - bottom-left corner");
    LOG_INFO("Main", "  - YELLOW (automatic): grid (4, 3) - moves in rectangular pattern");
    LOG_INFO("Main", "  - PURPLE: grid (5, 4) - center");
    LOG_INFO("Main", "Controls:");
    LOG_INFO("Main", "  - Use ARROW KEYS to move red square on grid");
    LOG_INFO("Main", "  - Yellow square moves automatically to demonstrate grid system");
    LOG_INFO("Main", "  - Movement uses smooth interpolation between cells");
    LOG_INFO("Main", "  - Press ESCAPE or close window to exit");
    LOG_INFO("Main", "Starting grid-based movement demo...");

    // Automatic movement pattern for yellow square (rectangular loop)
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

      // Automatic movement for yellow square (demonstrates grid system with multiple entities)
      GridPosition* yellowGridPos = gridPositionComponents.get(yellowRect.id);
      GridMovement* yellowGridMove = gridMovementComponents.get(yellowRect.id);
      GridBounds* yellowBounds = gridBoundsComponents.get(yellowRect.id);

      if (yellowGridPos && yellowGridMove && yellowBounds) {
        // If yellow square finished moving, queue next position in pattern
        if (!yellowGridMove->isMoving) {
          // Move to next position in pattern
          currentPatternIndex = (currentPatternIndex + 1) % patternSize;
          const GridPosition& nextPos = autoMovementPattern[currentPatternIndex];

          if (yellowBounds->isValid(nextPos.x, nextPos.y)) {
            yellowGridMove->targetX = nextPos.x;
            yellowGridMove->targetY = nextPos.y;
            yellowGridMove->progress = 0.0f;
            yellowGridMove->isMoving = true;
            LOG_INFO("AutoMovement", "Yellow square moving to grid cell (" +
                     std::to_string(nextPos.x) + ", " + std::to_string(nextPos.y) + ")");
          }
        }
      }

      // Update MovementSystem (handles smooth interpolation)
      movementSystem->update(entityManager, 0.016f);  // ~60 FPS (16ms)

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
