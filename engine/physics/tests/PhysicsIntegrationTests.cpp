#include <gtest/gtest.h>
#include "../include/MovementSystem.hpp"
#include "../include/GridMovement.hpp"
#include "../include/Physics.hpp"
#include "../../input/include/MockInputManager.hpp"
#include "../../ecs/include/EntityManager.hpp"
#include "../../ecs/include/ComponentArray.hpp"
#include "../../ecs/components/include/Transform.hpp"

using namespace ECS;

/**
 * Integration test fixture for complete physics system
 * Tests interactions between MovementSystem, GridMovement, Physics components, and input
 */
class PhysicsIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockInput = std::make_unique<MockInputManager>();
        movementSystem = std::make_unique<MovementSystem>(mockInput.get());
        entityManager = std::make_unique<EntityManager>();
        
        // Initialize all component arrays
        positions = std::make_unique<ComponentArray<Position>>();
        gridPositions = std::make_unique<ComponentArray<GridPosition>>();
        gridMovements = std::make_unique<ComponentArray<GridMovement>>();
        velocities = std::make_unique<ComponentArray<Velocity>>();
        accelerations = std::make_unique<ComponentArray<Acceleration>>();
        constraints = std::make_unique<ComponentArray<MovementConstraints>>();
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
        accelerations.reset();
        constraints.reset();
        gridBounds.reset();
    }
    
    /**
     * Create a fully-featured physics entity with all components
     */
    Entity createPhysicsEntity(int gridX = 0, int gridY = 0, 
                              float maxSpeed = 100.0f, bool bounded = false) {
        Entity entity = entityManager->createEntity();
        
        // Get component bits
        uint64_t posBit = getComponentBit<Position>();
        uint64_t gridPosBit = getComponentBit<GridPosition>();
        uint64_t gridMoveBit = getComponentBit<GridMovement>();
        uint64_t velBit = getComponentBit<Velocity>();
        uint64_t accelBit = getComponentBit<Acceleration>();
        uint64_t constraintsBit = getComponentBit<MovementConstraints>();
        
        // Add core components
        positions->add(entity.id, Position{static_cast<float>(gridX * 32), static_cast<float>(gridY * 32), 0.0f}, posBit, entity);
        gridPositions->add(entity.id, GridPosition{gridX, gridY}, gridPosBit, entity);
        gridMovements->add(entity.id, GridMovement{}, gridMoveBit, entity);
        velocities->add(entity.id, Velocity{}, velBit, entity);
        accelerations->add(entity.id, Acceleration{}, accelBit, entity);
        
        // Add movement constraints
        MovementConstraints entityConstraints;
        entityConstraints.maxSpeed = maxSpeed;
        constraints->add(entity.id, entityConstraints, constraintsBit, entity);
        
        // Optional bounds
        if (bounded) {
            uint64_t boundsBit = getComponentBit<GridBounds>();
            gridBounds->add(entity.id, GridBounds(0, 0, 20, 20), boundsBit, entity);
        }
        
        return entity;
    }
    
    /**
     * Simulate multiple frame updates until condition is met or timeout
     */
    void updateUntil(std::function<bool()> condition, int maxFrames = 100, float deltaTime = 1.0f) {
        int frameCount = 0;
        while (!condition() && frameCount < maxFrames) {
            movementSystem->update(*entityManager, deltaTime);
            frameCount++;
        }
    }
    
    std::unique_ptr<MockInputManager> mockInput;
    std::unique_ptr<MovementSystem> movementSystem;
    std::unique_ptr<EntityManager> entityManager;
    
    std::unique_ptr<ComponentArray<Position>> positions;
    std::unique_ptr<ComponentArray<GridPosition>> gridPositions;
    std::unique_ptr<ComponentArray<GridMovement>> gridMovements;
    std::unique_ptr<ComponentArray<Velocity>> velocities;
    std::unique_ptr<ComponentArray<Acceleration>> accelerations;
    std::unique_ptr<ComponentArray<MovementConstraints>> constraints;
    std::unique_ptr<ComponentArray<GridBounds>> gridBounds;
};

/**
 * Test complete movement pipeline: input -> grid movement -> visual interpolation -> completion
 */
TEST_F(PhysicsIntegrationTest, CompleteMovementPipeline) {
    Entity entity = createPhysicsEntity(5, 5);
    movementSystem->setControlledEntity(entity.id);
    movementSystem->setGlobalSpeed(5.0f);  // Faster for testing
    
    // Initial state
    const Position* pos = positions->get(entity.id);
    const GridPosition* gridPos = gridPositions->get(entity.id);
    EXPECT_EQ(gridPos->x, 5);
    EXPECT_EQ(gridPos->y, 5);
    EXPECT_FLOAT_EQ(pos->x, 5 * 32.0f);
    EXPECT_FLOAT_EQ(pos->y, 5 * 32.0f);
    
    // Trigger right movement via input
    mockInput->simulateKeyPress(KeyCode::Right);
    movementSystem->update(*entityManager, 1.0f);
    
    // Movement should have started
    const GridMovement* movement = gridMovements->get(entity.id);
    EXPECT_TRUE(movement->isMoving);
    EXPECT_EQ(movement->targetX, 6);
    EXPECT_EQ(movement->targetY, 5);
    EXPECT_GT(movement->progress, 0.0f);
    
    // Update until movement completes
    updateUntil([&]() { 
        const GridMovement* m = gridMovements->get(entity.id);
        return !m->isMoving;
    });
    
    // Final state verification
    movement = gridMovements->get(entity.id);
    pos = positions->get(entity.id);
    gridPos = gridPositions->get(entity.id);
    
    EXPECT_FALSE(movement->isMoving);
    EXPECT_EQ(gridPos->x, 6);
    EXPECT_EQ(gridPos->y, 5);
    EXPECT_FLOAT_EQ(pos->x, 6 * 32.0f);
    EXPECT_FLOAT_EQ(pos->y, 5 * 32.0f);
}

/**
 * Test multiple consecutive movements
 */
TEST_F(PhysicsIntegrationTest, ConsecutiveMovements) {
    Entity entity = createPhysicsEntity(0, 0);
    movementSystem->setControlledEntity(entity.id);
    movementSystem->setGlobalSpeed(10.0f);  // Fast movement
    
    // Sequence: Right -> Up -> Left -> Down (square pattern)
    std::vector<int> directions = {KeyCode::Right, KeyCode::Up, KeyCode::Left, KeyCode::Down};
    std::vector<std::pair<int, int>> expectedPositions = {{1, 0}, {1, -1}, {0, -1}, {0, 0}};  // Up decreases Y
    
    for (size_t i = 0; i < directions.size(); ++i) {
        mockInput->reset();
        mockInput->simulateKeyPress(directions[i]);
        movementSystem->update(*entityManager, 1.0f);
        
        // Wait for movement to complete
        updateUntil([&]() {
            const GridMovement* m = gridMovements->get(entity.id);
            return !m->isMoving;
        });
        
        // Verify position
        const GridPosition* gridPos = gridPositions->get(entity.id);
        EXPECT_EQ(gridPos->x, expectedPositions[i].first) 
            << "Failed at step " << i << " (direction " << directions[i] << ")";
        EXPECT_EQ(gridPos->y, expectedPositions[i].second)
            << "Failed at step " << i << " (direction " << directions[i] << ")";
    }
}

/**
 * Test movement with bounds constraints
 */
TEST_F(PhysicsIntegrationTest, MovementWithBounds) {
    Entity entity = createPhysicsEntity(0, 0, 100.0f, true);  // At corner with bounds
    movementSystem->setControlledEntity(entity.id);
    
    // Try to move left (should be blocked)
    mockInput->simulateKeyPress(KeyCode::Left);
    movementSystem->update(*entityManager, 1.0f);
    
    const GridMovement* movement = gridMovements->get(entity.id);
    EXPECT_FALSE(movement->isMoving);  // Should be blocked
    
    // Try to move right (should work)
    mockInput->reset();
    mockInput->simulateKeyPress(KeyCode::Right);
    movementSystem->update(*entityManager, 1.0f);
    
    EXPECT_TRUE(movement->isMoving);  // Should start movement
    EXPECT_EQ(movement->targetX, 1);
    EXPECT_EQ(movement->targetY, 0);
}

/**
 * Test queued movement system for turn-based gameplay
 */
TEST_F(PhysicsIntegrationTest, QueuedMovementSystem) {
    Entity player1 = createPhysicsEntity(2, 2);
    Entity player2 = createPhysicsEntity(8, 8);
    Entity player3 = createPhysicsEntity(15, 15);
    
    movementSystem->setGlobalSpeed(20.0f);  // Fast for testing
    
    // Queue movements for all entities (simulating turn-based commands)
    movementSystem->queueGridMovement(player1.id, 4, 5, false);
    movementSystem->queueGridMovement(player2.id, 6, 10, false);
    movementSystem->queueGridMovement(player3.id, 12, 18, false);
    
    // Verify movements are queued but not started
    const GridMovement* move1 = gridMovements->get(player1.id);
    const GridMovement* move2 = gridMovements->get(player2.id);
    const GridMovement* move3 = gridMovements->get(player3.id);
    
    EXPECT_TRUE(move1->hasPendingMove);
    EXPECT_TRUE(move2->hasPendingMove);
    EXPECT_TRUE(move3->hasPendingMove);
    EXPECT_FALSE(move1->isMoving);
    EXPECT_FALSE(move2->isMoving);
    EXPECT_FALSE(move3->isMoving);
    
    // Execute all queued movements simultaneously
    movementSystem->executeQueuedMovements(*entityManager);
    
    // All entities should now be moving
    EXPECT_TRUE(move1->isMoving);
    EXPECT_TRUE(move2->isMoving);
    EXPECT_TRUE(move3->isMoving);
    
    // Wait for all movements to complete
    updateUntil([&]() {
        return !move1->isMoving && !move2->isMoving && !move3->isMoving;
    });
    
    // Verify final positions
    const GridPosition* pos1 = gridPositions->get(player1.id);
    const GridPosition* pos2 = gridPositions->get(player2.id);
    const GridPosition* pos3 = gridPositions->get(player3.id);
    
    EXPECT_EQ(pos1->x, 4); EXPECT_EQ(pos1->y, 5);
    EXPECT_EQ(pos2->x, 6); EXPECT_EQ(pos2->y, 10);
    EXPECT_EQ(pos3->x, 12); EXPECT_EQ(pos3->y, 18);
}

/**
 * Test physics component interaction (Velocity + Acceleration)
 */
TEST_F(PhysicsIntegrationTest, PhysicsComponentInteraction) {
    Entity entity = createPhysicsEntity(0, 0);
    
    // Set initial velocity and acceleration
    Velocity* vel = const_cast<Velocity*>(velocities->get(entity.id));
    Acceleration* accel = const_cast<Acceleration*>(accelerations->get(entity.id));
    
    vel->dx = 2.0f;
    vel->dy = 1.0f;
    accel->dx = 0.5f;
    accel->dy = -0.1f;
    accel->damping = 0.98f;
    
    // Apply physics for several frames
    for (int i = 0; i < 5; ++i) {
        accel->applyTo(*vel);
    }
    
    // Velocity should have changed based on acceleration and damping
    EXPECT_GT(vel->dx, 2.0f);  // Should have increased due to positive acceleration
    EXPECT_LT(vel->dy, 1.0f);  // Should have decreased due to negative acceleration
    
    // Test magnitude calculation
    float mag = vel->magnitude();
    EXPECT_GT(mag, 0.0f);
    
    // Test normalization
    float originalMag = mag;
    vel->normalize();
    EXPECT_NEAR(vel->magnitude(), 1.0f, 0.001f);
    
    // Test scaling
    vel->scale(originalMag);  // Restore original magnitude
    EXPECT_NEAR(vel->magnitude(), originalMag, 0.001f);
}

/**
 * Test movement constraints application
 */
TEST_F(PhysicsIntegrationTest, MovementConstraintsApplication) {
    Entity entity = createPhysicsEntity(5, 5, 10.0f);  // Max speed 10.0
    
    Velocity* vel = const_cast<Velocity*>(velocities->get(entity.id));
    MovementConstraints* constraint = const_cast<MovementConstraints*>(constraints->get(entity.id));
    
    // Set velocity exceeding max speed
    vel->dx = 15.0f;
    vel->dy = 20.0f;  // Magnitude ~25
    
    // Apply constraints
    constraint->applyTo(*vel);
    
    // Should be clamped to max speed
    EXPECT_NEAR(vel->magnitude(), 10.0f, 0.001f);
    
    // Test diagonal restriction
    constraint->allowDiagonal = false;
    vel->dx = 5.0f;
    vel->dy = 8.0f;  // Y dominates
    
    constraint->applyTo(*vel);
    
    EXPECT_FLOAT_EQ(vel->dx, 0.0f);  // Should be zeroed
    EXPECT_FLOAT_EQ(vel->dy, 8.0f);  // Should be preserved
}

/**
 * Test grid coordinate conversion and interpolation
 */
TEST_F(PhysicsIntegrationTest, GridCoordinateConversion) {
    Entity entity = createPhysicsEntity(3, 4);
    movementSystem->setGridCellSize(64.0f);  // Custom cell size
    
    // Start movement to (5, 7)
    movementSystem->requestGridMovement(entity.id, 5, 7, false);
    
    // Update once to start interpolation
    movementSystem->update(*entityManager, 1.0f);
    
    const GridMovement* movement = gridMovements->get(entity.id);
    const Position* pos = positions->get(entity.id);
    
    // Position should be interpolated between start and target
    float startX = 3 * 64.0f;  // 192
    float startY = 4 * 64.0f;  // 256
    float targetX = 5 * 64.0f; // 320
    float targetY = 7 * 64.0f; // 448
    
    // Position should be between start and target
    EXPECT_GE(pos->x, startX);
    EXPECT_LE(pos->x, targetX);
    EXPECT_GE(pos->y, startY);
    EXPECT_LE(pos->y, targetY);
    
    // Progress should be reflected in position
    float expectedX = startX + (targetX - startX) * movement->progress;
    float expectedY = startY + (targetY - startY) * movement->progress;
    
    EXPECT_NEAR(pos->x, expectedX, 1.0f);  // Small tolerance for floating point
    EXPECT_NEAR(pos->y, expectedY, 1.0f);
}

/**
 * Test system performance with many entities
 */
TEST_F(PhysicsIntegrationTest, ManyEntitiesPerformance) {
    const int entityCount = 50;
    std::vector<Entity> entities;
    
    // Create many entities
    for (int i = 0; i < entityCount; ++i) {
        entities.push_back(createPhysicsEntity(i % 10, i / 10));
    }
    
    // Start movement for half the entities
    for (int i = 0; i < entityCount / 2; ++i) {
        movementSystem->requestGridMovement(entities[i].id, (i + 5) % 10, (i + 3) / 10, false);
    }
    
    // System should handle many entities without crashing
    EXPECT_NO_THROW({
        for (int frame = 0; frame < 10; ++frame) {
            movementSystem->update(*entityManager, 1.0f);
        }
    });
    
    // Verify some entities are moving and some are not
    int movingCount = 0;
    int stationaryCount = 0;
    
    for (const Entity& entity : entities) {
        const GridMovement* movement = gridMovements->get(entity.id);
        if (movement->isMoving || movement->progress > 0.0f) {
            movingCount++;
        } else {
            stationaryCount++;
        }
    }
    
    EXPECT_GT(movingCount, 0);
    EXPECT_GT(stationaryCount, 0);
    EXPECT_EQ(movingCount + stationaryCount, entityCount);
}

/**
 * Test error handling with invalid entities
 */
TEST_F(PhysicsIntegrationTest, InvalidEntityHandling) {
    EntityID invalidId = 999;  // Non-existent entity
    
    // Operations should fail gracefully
    EXPECT_FALSE(movementSystem->requestGridMovement(invalidId, 5, 5, false));
    EXPECT_FALSE(movementSystem->queueGridMovement(invalidId, 3, 3, false));
    EXPECT_FALSE(movementSystem->isEntityMoving(invalidId, *entityManager));
    
    // System should not crash with invalid controlled entity
    movementSystem->setControlledEntity(invalidId);
    EXPECT_NO_THROW(movementSystem->update(*entityManager, 1.0f));
}

/**
 * Test movement interruption and queuing
 */
TEST_F(PhysicsIntegrationTest, MovementInterruptionAndQueuing) {
    Entity entity = createPhysicsEntity(0, 0);
    movementSystem->setGlobalSpeed(2.0f);  // Slower for testing interruption
    
    // Start first movement
    movementSystem->requestGridMovement(entity.id, 5, 5, false);
    movementSystem->update(*entityManager, 1.0f);
    
    const GridMovement* movement = gridMovements->get(entity.id);
    EXPECT_TRUE(movement->isMoving);
    EXPECT_GT(movement->progress, 0.0f);
    EXPECT_LT(movement->progress, 1.0f);  // Should be in progress
    
    // Queue second movement while first is in progress
    movementSystem->queueGridMovement(entity.id, 10, 10, false);
    
    movement = gridMovements->get(entity.id);
    EXPECT_TRUE(movement->hasPendingMove);
    EXPECT_EQ(movement->pendingX, 10);
    EXPECT_EQ(movement->pendingY, 10);
    EXPECT_TRUE(movement->isMoving);  // First movement still active
    
    // Wait for first movement to complete
    updateUntil([&]() {
        const GridMovement* m = gridMovements->get(entity.id);
        return !m->isMoving;
    });
    
    // Start queued movement
    movementSystem->executeQueuedMovements(*entityManager);
    
    // Second movement should now be active
    movement = gridMovements->get(entity.id);
    EXPECT_TRUE(movement->isMoving);
    EXPECT_EQ(movement->targetX, 10);
    EXPECT_EQ(movement->targetY, 10);
    EXPECT_FALSE(movement->hasPendingMove);
}