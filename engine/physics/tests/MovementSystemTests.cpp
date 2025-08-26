#include <gtest/gtest.h>
#include "../include/MovementSystem.hpp"
#include "../../input/include/MockInputManager.hpp"
#include "../../ecs/include/ComponentArray.hpp"
#include "../../ecs/components/include/Transform.hpp"

using namespace ECS;

/**
 * Test fixture for MovementSystem
 */
class MovementSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockInput = std::make_unique<MockInputManager>();
        movementSystem = std::make_unique<MovementSystem>(mockInput.get());
        entityManager = std::make_unique<EntityManager>();
        
        // Set up component arrays
        positions = std::make_unique<ComponentArray<Position>>();
        gridPositions = std::make_unique<ComponentArray<GridPosition>>();  
        gridMovements = std::make_unique<ComponentArray<GridMovement>>();
        velocities = std::make_unique<ComponentArray<Velocity>>();
        gridBounds = std::make_unique<ComponentArray<GridBounds>>();
    }
    
    void TearDown() override {
        movementSystem.reset();
        mockInput.reset();
        entityManager.reset();
        positions.reset();
        gridPositions.reset();
        gridMovements.reset();
        velocities.reset();
        gridBounds.reset();
    }
    
    // Helper: Create entity with basic movement components
    Entity createMovableEntity(int gridX = 0, int gridY = 0) {
        Entity entity = entityManager->createEntity();
        
        // Add required components for grid movement
        uint64_t posBit = getComponentBit<Position>();
        uint64_t gridPosBit = getComponentBit<GridPosition>();
        uint64_t gridMoveBit = getComponentBit<GridMovement>();
        
        positions->add(entity.id, Position{static_cast<float>(gridX * 32), static_cast<float>(gridY * 32), 0.0f}, posBit, entity);
        gridPositions->add(entity.id, GridPosition{gridX, gridY}, gridPosBit, entity);
        gridMovements->add(entity.id, GridMovement{}, gridMoveBit, entity);
        
        return entity;
    }
    
    std::unique_ptr<MockInputManager> mockInput;
    std::unique_ptr<MovementSystem> movementSystem;
    std::unique_ptr<EntityManager> entityManager;
    
    std::unique_ptr<ComponentArray<Position>> positions;
    std::unique_ptr<ComponentArray<GridPosition>> gridPositions;
    std::unique_ptr<ComponentArray<GridMovement>> gridMovements;
    std::unique_ptr<ComponentArray<Velocity>> velocities;
    std::unique_ptr<ComponentArray<GridBounds>> gridBounds;
};

/**
 * Test MovementSystem construction and basic interface
 */
TEST_F(MovementSystemTest, Construction) {
    // Test construction without input manager
    MovementSystem systemNoInput;
    EXPECT_EQ(systemNoInput.getControlledEntity(), INVALID_ENTITY);
    
    // Test construction with input manager
    EXPECT_EQ(movementSystem->getControlledEntity(), INVALID_ENTITY);
    EXPECT_FLOAT_EQ(movementSystem->getGlobalSpeed(), 1.0f);
    EXPECT_FLOAT_EQ(movementSystem->getGridCellSize(), 32.0f);
}

/**
 * Test controlled entity management
 */
TEST_F(MovementSystemTest, ControlledEntityManagement) {
    Entity entity = createMovableEntity(5, 10);
    
    // Initially no controlled entity
    EXPECT_EQ(movementSystem->getControlledEntity(), INVALID_ENTITY);
    
    // Set controlled entity
    movementSystem->setControlledEntity(entity.id);
    EXPECT_EQ(movementSystem->getControlledEntity(), entity.id);
    
    // Clear controlled entity
    movementSystem->setControlledEntity(INVALID_ENTITY);
    EXPECT_EQ(movementSystem->getControlledEntity(), INVALID_ENTITY);
}

/**
 * Test global speed setting
 */
TEST_F(MovementSystemTest, GlobalSpeedSettings) {
    EXPECT_FLOAT_EQ(movementSystem->getGlobalSpeed(), 1.0f);
    
    movementSystem->setGlobalSpeed(2.5f);
    EXPECT_FLOAT_EQ(movementSystem->getGlobalSpeed(), 2.5f);
    
    movementSystem->setGlobalSpeed(0.1f);
    EXPECT_FLOAT_EQ(movementSystem->getGlobalSpeed(), 0.1f);
}

/**
 * Test grid cell size configuration
 */
TEST_F(MovementSystemTest, GridCellSizeSettings) {
    EXPECT_FLOAT_EQ(movementSystem->getGridCellSize(), 32.0f);
    
    movementSystem->setGridCellSize(64.0f);
    EXPECT_FLOAT_EQ(movementSystem->getGridCellSize(), 64.0f);
    
    movementSystem->setGridCellSize(16.0f);
    EXPECT_FLOAT_EQ(movementSystem->getGridCellSize(), 16.0f);
}

/**
 * Test immediate grid movement request
 */
TEST_F(MovementSystemTest, ImmediateGridMovement) {
    Entity entity = createMovableEntity(0, 0);
    
    // Request movement to (5, 3)
    bool success = movementSystem->requestGridMovement(entity.id, 5, 3, false);
    EXPECT_TRUE(success);
    
    // Verify GridMovement component updated
    const GridMovement* movement = gridMovements->get(entity.id);
    ASSERT_NE(movement, nullptr);
    EXPECT_EQ(movement->targetX, 5);
    EXPECT_EQ(movement->targetY, 3);
    EXPECT_TRUE(movement->isMoving);
    EXPECT_FLOAT_EQ(movement->progress, 0.0f);
}

/**
 * Test queued grid movement  
 */
TEST_F(MovementSystemTest, QueuedGridMovement) {
    Entity entity = createMovableEntity(2, 4);
    
    // Queue a movement
    bool success = movementSystem->queueGridMovement(entity.id, 7, 9, false);
    EXPECT_TRUE(success);
    
    // Verify movement is queued but not started
    const GridMovement* movement = gridMovements->get(entity.id);
    ASSERT_NE(movement, nullptr);
    EXPECT_TRUE(movement->hasPendingMove);
    EXPECT_EQ(movement->pendingX, 7);
    EXPECT_EQ(movement->pendingY, 9);
    EXPECT_FALSE(movement->isMoving);  // Not started yet
}

/**
 * Test queued movement execution
 */
TEST_F(MovementSystemTest, QueuedMovementExecution) {
    Entity entity = createMovableEntity(1, 1);
    
    // Queue movement
    movementSystem->queueGridMovement(entity.id, 8, 12, false);
    
    // Execute queued movements
    movementSystem->executeQueuedMovements(*entityManager);
    
    // Verify movement started
    const GridMovement* movement = gridMovements->get(entity.id);
    ASSERT_NE(movement, nullptr);
    EXPECT_EQ(movement->targetX, 8);
    EXPECT_EQ(movement->targetY, 12);
    EXPECT_TRUE(movement->isMoving);
    EXPECT_FLOAT_EQ(movement->progress, 0.0f);
    EXPECT_FALSE(movement->hasPendingMove);
}

/**
 * Test movement bounds validation
 */
TEST_F(MovementSystemTest, MovementBoundsValidation) {
    Entity entity = createMovableEntity(5, 5);
    
    // Add bounds component
    uint64_t boundsBit = getComponentBit<GridBounds>();
    gridBounds->add(entity.id, GridBounds(0, 0, 10, 10), boundsBit, entity);
    
    // Valid movement within bounds
    bool success = movementSystem->requestGridMovement(entity.id, 8, 7, true);
    EXPECT_TRUE(success);
    
    // Invalid movement outside bounds  
    success = movementSystem->requestGridMovement(entity.id, 15, 5, true);
    EXPECT_FALSE(success);
    
    success = movementSystem->requestGridMovement(entity.id, 5, -2, true);
    EXPECT_FALSE(success);
    
    // Bypass validation
    success = movementSystem->requestGridMovement(entity.id, 15, 5, false);
    EXPECT_TRUE(success);  // Should succeed when validation disabled
}

/**
 * Test entity movement status checking
 */
TEST_F(MovementSystemTest, MovementStatusChecking) {
    Entity entity = createMovableEntity(3, 3);
    
    // Initially not moving
    EXPECT_FALSE(movementSystem->isEntityMoving(entity.id, *entityManager));
    
    // Start movement
    movementSystem->requestGridMovement(entity.id, 6, 8, false);
    EXPECT_TRUE(movementSystem->isEntityMoving(entity.id, *entityManager));
    
    // Complete movement manually
    GridMovement* movement = const_cast<GridMovement*>(gridMovements->get(entity.id));
    movement->progress = 1.0f;
    movement->isMoving = false;
    
    EXPECT_FALSE(movementSystem->isEntityMoving(entity.id, *entityManager));
}

/**
 * Test movement stopping
 */
TEST_F(MovementSystemTest, MovementStopping) {
    Entity entity = createMovableEntity(2, 2);
    
    // Start movement
    movementSystem->requestGridMovement(entity.id, 8, 10, false);
    EXPECT_TRUE(movementSystem->isEntityMoving(entity.id, *entityManager));
    
    // Stop movement with grid snapping
    movementSystem->stopMovement(entity.id, *entityManager, true);
    
    const GridMovement* movement = gridMovements->get(entity.id);
    EXPECT_FALSE(movement->isMoving);
    EXPECT_FLOAT_EQ(movement->progress, 0.0f);
    
    // Verify position snapped to grid
    const Position* pos = positions->get(entity.id);
    const GridPosition* gridPos = gridPositions->get(entity.id);
    EXPECT_FLOAT_EQ(pos->x, gridPos->x * movementSystem->getGridCellSize());
    EXPECT_FLOAT_EQ(pos->y, gridPos->y * movementSystem->getGridCellSize());
}

/**
 * Test system update with no entities (should not crash)
 */
TEST_F(MovementSystemTest, UpdateWithNoEntities) {
    EXPECT_NO_THROW(movementSystem->update(*entityManager, 1.0f));
}

/**
 * Test system update with moving entity
 */
TEST_F(MovementSystemTest, UpdateWithMovingEntity) {
    Entity entity = createMovableEntity(0, 0);
    
    // Start movement from (0,0) to (3,4)
    movementSystem->requestGridMovement(entity.id, 3, 4, false);
    
    // Simulate one frame update
    movementSystem->update(*entityManager, 1.0f);
    
    // Movement should have progressed
    const GridMovement* movement = gridMovements->get(entity.id);
    EXPECT_GT(movement->progress, 0.0f);
    EXPECT_LT(movement->progress, 1.0f);  // Should not complete in one frame
    EXPECT_TRUE(movement->isMoving);
    
    // Position should have been interpolated
    const Position* pos = positions->get(entity.id);
    EXPECT_GT(pos->x, 0.0f);  // Should have moved from start
    EXPECT_LT(pos->x, 3.0f * movementSystem->getGridCellSize());  // But not to target
}

/**
 * Test movement completion through updates
 */
TEST_F(MovementSystemTest, MovementCompletionThroughUpdates) {
    Entity entity = createMovableEntity(1, 1);
    
    // Start fast movement
    movementSystem->requestGridMovement(entity.id, 2, 1, false);  
    movementSystem->setGlobalSpeed(10.0f);  // Very fast for quick completion
    
    // Update until movement completes
    const GridMovement* movement;
    int updateCount = 0;
    do {
        movementSystem->update(*entityManager, 1.0f);
        movement = gridMovements->get(entity.id);
        updateCount++;
    } while (movement->isMoving && updateCount < 100);  // Safety limit
    
    // Movement should be complete
    EXPECT_FALSE(movement->isMoving);
    EXPECT_GE(movement->progress, 1.0f);
    
    // Position should be at target
    const Position* pos = positions->get(entity.id);
    const GridPosition* gridPos = gridPositions->get(entity.id);
    
    EXPECT_EQ(gridPos->x, 2);
    EXPECT_EQ(gridPos->y, 1);
    EXPECT_FLOAT_EQ(pos->x, 2.0f * movementSystem->getGridCellSize());
    EXPECT_FLOAT_EQ(pos->y, 1.0f * movementSystem->getGridCellSize());
}

/**
 * Test input-driven movement (controlled entity)
 */
TEST_F(MovementSystemTest, InputDrivenMovement) {
    Entity entity = createMovableEntity(5, 5);
    movementSystem->setControlledEntity(entity.id);
    
    // Simulate right arrow key press
    mockInput->simulateKeyPress(KeyCode::Right);
    
    // Update system to process input
    movementSystem->update(*entityManager, 1.0f);
    
    // Should have started movement to (6, 5)
    const GridMovement* movement = gridMovements->get(entity.id);
    EXPECT_TRUE(movement->isMoving);
    EXPECT_EQ(movement->targetX, 6);
    EXPECT_EQ(movement->targetY, 5);
    
    // Test other directions
    mockInput->reset();
    movementSystem->stopMovement(entity.id, *entityManager, true);
    
    mockInput->simulateKeyPress(KeyCode::Up);
    movementSystem->update(*entityManager, 1.0f);
    
    movement = gridMovements->get(entity.id);
    EXPECT_EQ(movement->targetX, 5);
    EXPECT_EQ(movement->targetY, 4);  // Up decreases Y
}

/**
 * Test input movement with bounds validation
 */
TEST_F(MovementSystemTest, InputMovementWithBounds) {
    Entity entity = createMovableEntity(0, 0);  // At boundary
    movementSystem->setControlledEntity(entity.id);
    
    // Add bounds
    uint64_t boundsBit = getComponentBit<GridBounds>();
    gridBounds->add(entity.id, GridBounds(0, 0, 10, 10), boundsBit, entity);
    
    // Try to move left (should be blocked by bounds)
    mockInput->simulateKeyPress(KeyCode::Left);
    movementSystem->update(*entityManager, 1.0f);
    
    // Should not have started movement
    const GridMovement* movement = gridMovements->get(entity.id);
    EXPECT_FALSE(movement->isMoving);
    
    // Try to move right (should be allowed)
    mockInput->reset();
    mockInput->simulateKeyPress(KeyCode::Right);
    movementSystem->update(*entityManager, 1.0f);
    
    EXPECT_TRUE(movement->isMoving);
    EXPECT_EQ(movement->targetX, 1);
    EXPECT_EQ(movement->targetY, 0);
}

/**
 * Test multiple entities with different movement states
 */
TEST_F(MovementSystemTest, MultipleEntitiesMovement) {
    Entity entity1 = createMovableEntity(0, 0);
    Entity entity2 = createMovableEntity(5, 5);
    Entity entity3 = createMovableEntity(10, 10);
    
    // Start movements for different entities
    movementSystem->requestGridMovement(entity1.id, 2, 3, false);
    movementSystem->requestGridMovement(entity2.id, 7, 8, false);
    // entity3 remains stationary
    
    // Update system
    movementSystem->update(*entityManager, 1.0f);
    
    // Check entity1 is moving
    const GridMovement* movement1 = gridMovements->get(entity1.id);
    EXPECT_TRUE(movement1->isMoving);
    EXPECT_GT(movement1->progress, 0.0f);
    
    // Check entity2 is moving
    const GridMovement* movement2 = gridMovements->get(entity2.id);
    EXPECT_TRUE(movement2->isMoving);
    EXPECT_GT(movement2->progress, 0.0f);
    
    // Check entity3 is not moving
    const GridMovement* movement3 = gridMovements->get(entity3.id);
    EXPECT_FALSE(movement3->isMoving);
    EXPECT_FLOAT_EQ(movement3->progress, 0.0f);
}

/**
 * Test system without input manager
 */
TEST_F(MovementSystemTest, SystemWithoutInputManager) {
    MovementSystem systemNoInput;
    Entity entity = createMovableEntity(3, 3);
    
    // Should not crash when updating without input manager
    EXPECT_NO_THROW(systemNoInput.update(*entityManager, 1.0f));
    
    // Manual movement should still work
    bool success = systemNoInput.requestGridMovement(entity.id, 5, 7, false);
    EXPECT_TRUE(success);
    
    const GridMovement* movement = gridMovements->get(entity.id);
    EXPECT_TRUE(movement->isMoving);
}