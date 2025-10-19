#include "../include/MovementSystem.hpp"
#include "../../ecs/include/ComponentArray.hpp"
#include "../../ecs/systems/include/IInputManager.hpp"
#include "../../ecs/include/ComponentRegistry.hpp"
#include <cmath>

namespace ECS {

MovementSystem::MovementSystem(ComponentArray<Position>* positions,
                               ComponentArray<GridPosition>* gridPositions,
                               ComponentArray<GridMovement>* gridMovements,
                               ComponentArray<Velocity>* velocities,
                               ComponentArray<Acceleration>* accelerations,
                               ComponentArray<MovementConstraints>* constraints,
                               ComponentArray<GridBounds>* gridBounds,
                               IInputManager* inputManager)
    : positions(positions)
    , gridPositions(gridPositions)
    , gridMovements(gridMovements)
    , velocities(velocities)
    , accelerations(accelerations)
    , constraints(constraints)
    , gridBounds(gridBounds)
    , inputManager(inputManager)
    , controlledEntity(INVALID_ENTITY)
    , globalSpeedMultiplier(1.0f)
    , gridCellSize(32.0f)
    , lastFrameKeyStates{false, false, false, false} {
}

MovementSystem::MovementSystem(IInputManager* inputManager)
    : positions(nullptr)
    , gridPositions(nullptr)
    , gridMovements(nullptr)
    , velocities(nullptr)
    , accelerations(nullptr)
    , constraints(nullptr)
    , gridBounds(nullptr)
    , inputManager(inputManager)
    , controlledEntity(INVALID_ENTITY)
    , globalSpeedMultiplier(1.0f)
    , gridCellSize(32.0f)
    , lastFrameKeyStates{false, false, false, false} {
}

void MovementSystem::setComponentArrays(ComponentArray<Position>* pos,
                                       ComponentArray<GridPosition>* gridPos,
                                       ComponentArray<GridMovement>* gridMove,
                                       ComponentArray<Velocity>* vel,
                                       ComponentArray<Acceleration>* accel,
                                       ComponentArray<MovementConstraints>* constr,
                                       ComponentArray<GridBounds>* bounds) {
    positions = pos;
    gridPositions = gridPos;
    gridMovements = gridMove;
    velocities = vel;
    accelerations = accel;
    constraints = constr;
    gridBounds = bounds;
}

void MovementSystem::update(EntityManager& entityManager, float deltaTime) {
    // Process input for controlled entity
    if (inputManager && controlledEntity != INVALID_ENTITY) {
        processInputMovement(entityManager);
    }
    
    // Update all grid-based movement animations
    updateGridMovement(entityManager, deltaTime);
    
    // Update physics-based movement
    updatePhysicsMovement(entityManager, deltaTime);
}

void MovementSystem::setControlledEntity(EntityID entityId) {
    controlledEntity = entityId;
}

EntityID MovementSystem::getControlledEntity() const {
    return controlledEntity;
}

bool MovementSystem::requestGridMovement(EntityID entityId, int targetX, int targetY, bool validateBounds) {
    if (!gridMovements || !gridPositions) {
        return false;
    }
    
    // Get required components
    auto* gridMovement = gridMovements->get(entityId);
    auto* gridPosition = gridPositions->get(entityId);
    
    if (!gridMovement || !gridPosition) {
        return false;
    }
    
    // Validate movement if requested - for now we'll skip this until validateMovement is fixed
    if (validateBounds && gridBounds) {
        auto* bounds = gridBounds->get(entityId);
        if (bounds && !bounds->isValid(targetX, targetY)) {
            return false;
        }
    }
    
    // Start movement
    gridMovement->targetX = targetX;
    gridMovement->targetY = targetY;
    gridMovement->progress = 0.0f;
    gridMovement->isMoving = true;
    
    return true;
}

bool MovementSystem::queueGridMovement(EntityID entityId, int targetX, int targetY, bool validateBounds) {
    if (!gridMovements) {
        return false;
    }
    
    auto* gridMovement = gridMovements->get(entityId);
    
    if (!gridMovement) {
        return false;
    }
    
    // Validate movement if requested
    if (validateBounds && gridBounds) {
        auto* bounds = gridBounds->get(entityId);
        if (bounds && !bounds->isValid(targetX, targetY)) {
            return false;
        }
    }
    
    // Queue movement
    gridMovement->queueMove(targetX, targetY);
    
    return true;
}

void MovementSystem::executeQueuedMovements(EntityManager& entityManager) {
    if (!gridMovements) {
        return;
    }

    // Get all entities with GridMovement component
    auto entities = entityManager.getAllEntitiesForIteration();
    uint64_t gridMovementBit = getComponentBit<GridMovement>();

    for (const Entity* entity : entities) {
        if (!entityManager.isValid(*entity) || !(entity->componentMask & gridMovementBit)) {
            continue;
        }

        auto* gridMovement = gridMovements->get(entity->id);
        if (gridMovement && gridMovement->hasPendingMove && !gridMovement->isMoving) {
            gridMovement->startQueuedMove();
        }
    }
}

bool MovementSystem::isEntityMoving(EntityID entityId, EntityManager& entityManager) const {
    (void)entityManager; // Suppress unused parameter warning
    if (!gridMovements) {
        return false;
    }
    auto* gridMovement = gridMovements->get(entityId);
    return gridMovement && gridMovement->isMoving;
}

void MovementSystem::stopMovement(EntityID entityId, EntityManager& entityManager, bool snapToGrid) {
    (void)entityManager; // Suppress unused parameter warning
    
    if (!gridMovements) {
        return;
    }
    
    auto* gridMovement = gridMovements->get(entityId);
    if (!gridMovement) {
        return;
    }
    
    gridMovement->isMoving = false;
    gridMovement->progress = 0.0f;
    
    if (snapToGrid && positions && gridPositions) {
        auto* position = positions->get(entityId);
        auto* gridPosition = gridPositions->get(entityId);
        
        if (position && gridPosition) {
            // Snap to current grid position
            float worldX, worldY;
            gridToWorld(gridPosition->x, gridPosition->y, worldX, worldY);
            position->x = worldX;
            position->y = worldY;
        }
    }
}

void MovementSystem::setGlobalSpeed(float speed) {
    globalSpeedMultiplier = speed;
}

float MovementSystem::getGlobalSpeed() const {
    return globalSpeedMultiplier;
}

void MovementSystem::setGridCellSize(float cellSize) {
    gridCellSize = cellSize;
}

float MovementSystem::getGridCellSize() const {
    return gridCellSize;
}

void MovementSystem::processInputMovement(EntityManager& entityManager) {
    (void)entityManager; // Suppress unused parameter warning
    if (!inputManager) {
        return;
    }
    
    // Check current key states
    bool leftPressed = inputManager->isKeyPressed(KeyCode::Left);
    bool rightPressed = inputManager->isKeyPressed(KeyCode::Right);
    bool upPressed = inputManager->isKeyPressed(KeyCode::Up);
    bool downPressed = inputManager->isKeyPressed(KeyCode::Down);
    
    // Get current grid position
    if (!gridPositions || !gridMovements) {
        // Update key states and return
        lastFrameKeyStates[0] = leftPressed;
        lastFrameKeyStates[1] = rightPressed;
        lastFrameKeyStates[2] = upPressed;
        lastFrameKeyStates[3] = downPressed;
        return;
    }
    
    auto* gridPosition = gridPositions->get(controlledEntity);
    auto* gridMovement = gridMovements->get(controlledEntity);
    
    if (!gridPosition || !gridMovement || gridMovement->isMoving) {
        // Update key states for next frame
        lastFrameKeyStates[0] = leftPressed;
        lastFrameKeyStates[1] = rightPressed;
        lastFrameKeyStates[2] = upPressed;
        lastFrameKeyStates[3] = downPressed;
        return;
    }
    
    // Process movement (only on key press, not hold)
    int targetX = gridPosition->x;
    int targetY = gridPosition->y;
    bool movementRequested = false;
    
    if (leftPressed && !lastFrameKeyStates[0]) {
        targetX--;
        movementRequested = true;
    }
    else if (rightPressed && !lastFrameKeyStates[1]) {
        targetX++;
        movementRequested = true;
    }
    else if (upPressed && !lastFrameKeyStates[2]) {
        targetY--;  // Up decreases Y in grid coordinates
        movementRequested = true;
    }
    else if (downPressed && !lastFrameKeyStates[3]) {
        targetY++;  // Down increases Y in grid coordinates
        movementRequested = true;
    }
    
    if (movementRequested) {
        requestGridMovement(controlledEntity, targetX, targetY, true);
    }
    
    // Update key states for next frame
    lastFrameKeyStates[0] = leftPressed;
    lastFrameKeyStates[1] = rightPressed;
    lastFrameKeyStates[2] = upPressed;
    lastFrameKeyStates[3] = downPressed;
}

void MovementSystem::updateGridMovement(EntityManager& entityManager, float deltaTime) {
    if (!gridPositions || !gridMovements || !positions) {
        return;
    }

    // Get all entities with required components
    auto entities = entityManager.getAllEntitiesForIteration();
    uint64_t requiredMask = getComponentBit<GridPosition>() | getComponentBit<GridMovement>() | getComponentBit<Position>();

    for (const Entity* entity : entities) {
        if (!entityManager.isValid(*entity) || (entity->componentMask & requiredMask) != requiredMask) {
            continue;
        }

        auto* gridPosition = gridPositions->get(entity->id);
        auto* gridMovement = gridMovements->get(entity->id);
        auto* position = positions->get(entity->id);

        if (!gridPosition || !gridMovement || !position) {
            continue;
        }

        if (!gridMovement->isMoving) {
            continue;
        }

        // Calculate movement speed (affected by global multiplier and individual speed)
        float effectiveSpeed = gridMovement->speed * globalSpeedMultiplier * deltaTime;

        // Update progress
        gridMovement->progress += effectiveSpeed;

        // Check if movement is complete
        if (gridMovement->isComplete()) {
            // Complete movement - snap to target
            gridPosition->x = gridMovement->targetX;
            gridPosition->y = gridMovement->targetY;

            float worldX, worldY;
            gridToWorld(gridPosition->x, gridPosition->y, worldX, worldY);
            position->x = worldX;
            position->y = worldY;

            // Reset movement state (queued movements must be started manually via executeQueuedMovements)
            gridMovement->reset();
        }
        else {
            // Interpolate position between start and target
            float startWorldX, startWorldY;
            float targetWorldX, targetWorldY;
            
            gridToWorld(gridPosition->x, gridPosition->y, startWorldX, startWorldY);
            gridToWorld(gridMovement->targetX, gridMovement->targetY, targetWorldX, targetWorldY);
            
            interpolatePosition(startWorldX, startWorldY, targetWorldX, targetWorldY, 
                              gridMovement->progress, position->x, position->y);
        }
    }
}

void MovementSystem::updatePhysicsMovement(EntityManager& entityManager, float deltaTime) {
    if (!velocities || !positions) {
        return;
    }

    // Get all entities with physics components
    auto entities = entityManager.getAllEntitiesForIteration();
    uint64_t physicsRequiredMask = getComponentBit<Velocity>() | getComponentBit<Position>();

    for (const Entity* entity : entities) {
        if (!entityManager.isValid(*entity) || (entity->componentMask & physicsRequiredMask) != physicsRequiredMask) {
            continue;
        }

        auto* velocity = velocities->get(entity->id);
        auto* position = positions->get(entity->id);
        auto* acceleration = accelerations ? accelerations->get(entity->id) : nullptr;
        auto* constraint = constraints ? constraints->get(entity->id) : nullptr;
        
        if (!velocity || !position) {
            continue;
        }
        
        // Apply acceleration if present
        if (acceleration && !acceleration->isZero()) {
            acceleration->applyTo(*velocity);
        }
        
        // Apply movement constraints if present
        if (constraint) {
            constraint->applyTo(*velocity);
        }
        
        // Update position based on velocity
        if (!velocity->isZero()) {
            position->x += velocity->dx * deltaTime;
            position->y += velocity->dy * deltaTime;
        }
    }
}

void MovementSystem::gridToWorld(int gridX, int gridY, float& worldX, float& worldY) const {
    worldX = static_cast<float>(gridX) * gridCellSize;
    worldY = static_cast<float>(gridY) * gridCellSize;
}

void MovementSystem::worldToGrid(float worldX, float worldY, int& gridX, int& gridY) const {
    gridX = static_cast<int>(std::floor(worldX / gridCellSize + 0.5f));
    gridY = static_cast<int>(std::floor(worldY / gridCellSize + 0.5f));
}

bool MovementSystem::validateMovement(EntityID entityId, int targetX, int targetY, EntityManager& entityManager) const {
    (void)entityManager; // Suppress unused parameter warning
    
    // Check bounds if entity has GridBounds component
    if (gridBounds) {
        auto* bounds = gridBounds->get(entityId);
        if (bounds && !bounds->isValid(targetX, targetY)) {
            return false;
        }
    }
    
    // TODO: Add obstacle collision checking here when obstacle system is implemented
    
    return true;
}

void MovementSystem::interpolatePosition(float startX, float startY, float targetX, float targetY, 
                                       float progress, float& resultX, float& resultY) const {
    // Clamp progress to [0, 1] range
    progress = std::max(0.0f, std::min(1.0f, progress));
    
    // Linear interpolation
    resultX = startX + (targetX - startX) * progress;
    resultY = startY + (targetY - startY) * progress;
}

} // namespace ECS