#pragma once

#include "../../ecs/include/EntityManager.hpp"
#include "../../ecs/components/include/Transform.hpp"
#include "../../ecs/systems/include/IInputManager.hpp"
#include "GridMovement.hpp"
#include "Physics.hpp"

namespace ECS {

/**
 * MovementSystem - Handles grid-based movement with smooth visual transitions
 * 
 * Core responsibilities:
 * 1. Process input commands and convert to grid movement requests
 * 2. Validate movement requests against bounds and obstacles
 * 3. Update visual Position components based on GridMovement progress
 * 4. Handle movement completion and state transitions
 * 5. Support both immediate movement and turn-based queued movement
 * 
 * System operates on entities with: GridPosition + GridMovement + Position
 * Optional components: Velocity, Acceleration, GridBounds, MovementConstraints
 */
class MovementSystem {
public:
    /**
     * Constructor with optional input manager for real-time movement
     */
    explicit MovementSystem(IInputManager* inputManager = nullptr);
    ~MovementSystem() = default;
    
    /**
     * Update all movement and animation
     * @param entityManager Entity manager containing entities to process
     * @param deltaTime Time elapsed since last update (for frame-rate independent movement)
     */
    void update(EntityManager& entityManager, float deltaTime = 1.0f);
    
    /**
     * Set which entity should be controlled by input (if input manager available)
     * @param entityId ID of entity to control, or INVALID_ENTITY to disable
     */
    void setControlledEntity(EntityID entityId);
    
    /**
     * Get currently controlled entity
     */
    EntityID getControlledEntity() const;
    
    /**
     * Request immediate grid movement for an entity
     * @param entityId Target entity
     * @param targetX Target grid X coordinate  
     * @param targetY Target grid Y coordinate
     * @param validateBounds Check GridBounds component if present
     * @return true if movement was initiated, false if invalid/blocked
     */
    bool requestGridMovement(EntityID entityId, int targetX, int targetY, bool validateBounds = true);
    
    /**
     * Queue grid movement for turn-based systems
     * @param entityId Target entity
     * @param targetX Target grid X coordinate
     * @param targetY Target grid Y coordinate  
     * @param validateBounds Check GridBounds component if present
     * @return true if movement was queued, false if invalid
     */
    bool queueGridMovement(EntityID entityId, int targetX, int targetY, bool validateBounds = true);
    
    /**
     * Execute all queued movements (for turn-based systems)
     * @param entityManager Entity manager
     */
    void executeQueuedMovements(EntityManager& entityManager);
    
    /**
     * Check if entity is currently moving
     * @param entityId Entity to check
     * @param entityManager Entity manager
     */
    bool isEntityMoving(EntityID entityId, EntityManager& entityManager) const;
    
    /**
     * Stop movement for specific entity
     * @param entityId Entity to stop
     * @param entityManager Entity manager
     * @param snapToGrid If true, snap to nearest grid position
     */
    void stopMovement(EntityID entityId, EntityManager& entityManager, bool snapToGrid = true);
    
    /**
     * Set global movement speed multiplier
     * @param speed Speed multiplier (1.0 = normal, 2.0 = double speed, etc.)
     */
    void setGlobalSpeed(float speed);
    
    /**
     * Get global movement speed multiplier
     */
    float getGlobalSpeed() const;
    
    /**
     * Set grid cell size for coordinate conversion
     * @param cellSize Size of each grid cell in world units
     */
    void setGridCellSize(float cellSize);
    
    /**
     * Get grid cell size
     */
    float getGridCellSize() const;

private:
    /**
     * Process keyboard input for controlled entity
     */
    void processInputMovement(EntityManager& entityManager);
    
    /**
     * Update grid movement animation for all entities
     */
    void updateGridMovement(EntityManager& entityManager, float deltaTime);
    
    /**
     * Update physics-based movement (Velocity + Acceleration)  
     */
    void updatePhysicsMovement(EntityManager& entityManager, float deltaTime);
    
    /**
     * Convert grid coordinates to world position
     * @param gridX Grid X coordinate
     * @param gridY Grid Y coordinate
     * @param worldX Output world X coordinate
     * @param worldY Output world Y coordinate
     */
    void gridToWorld(int gridX, int gridY, float& worldX, float& worldY) const;
    
    /**
     * Convert world position to grid coordinates  
     * @param worldX World X coordinate
     * @param worldY World Y coordinate
     * @param gridX Output grid X coordinate
     * @param gridY Output grid Y coordinate
     */
    void worldToGrid(float worldX, float worldY, int& gridX, int& gridY) const;
    
    /**
     * Validate movement target against bounds and obstacles
     * @param entityId Entity attempting movement
     * @param targetX Target grid X
     * @param targetY Target grid Y
     * @param entityManager Entity manager
     * @return true if movement is valid
     */
    bool validateMovement(EntityID entityId, int targetX, int targetY, EntityManager& entityManager) const;
    
    /**
     * Interpolate between start and target positions based on progress
     * @param startX Starting X coordinate
     * @param startY Starting Y coordinate  
     * @param targetX Target X coordinate
     * @param targetY Target Y coordinate
     * @param progress Interpolation progress (0.0 to 1.0)
     * @param resultX Output interpolated X
     * @param resultY Output interpolated Y
     */
    void interpolatePosition(float startX, float startY, float targetX, float targetY, 
                           float progress, float& resultX, float& resultY) const;
    
    // Dependencies
    IInputManager* inputManager;
    
    // State
    EntityID controlledEntity;
    float globalSpeedMultiplier;
    float gridCellSize;
    
    // Input debouncing for controlled entity
    bool lastFrameKeyStates[4];  // [Left, Right, Up, Down]
};

} // namespace ECS