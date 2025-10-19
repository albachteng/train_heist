#include <gtest/gtest.h>
#include "../include/GridMovement.hpp"
#include "../../ecs/include/ComponentArray.hpp"
#include "../../ecs/include/EntityManager.hpp"

using namespace ECS;

/**
 * Test fixture for GridMovement component
 */
class GridMovementTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Fresh component for each test
        gridMovement = GridMovement{};
    }
    
    GridMovement gridMovement;
};

/**
 * Test default construction and ZII compliance
 */
TEST_F(GridMovementTest, DefaultConstruction) {
    GridMovement movement;
    
    // Verify Zero Initialization Invariant (ZII)
    EXPECT_EQ(movement.targetX, 0);
    EXPECT_EQ(movement.targetY, 0);
    EXPECT_FLOAT_EQ(movement.progress, 0.0f);
    EXPECT_FLOAT_EQ(movement.speed, 1.0f);
    EXPECT_FALSE(movement.isMoving);
    EXPECT_FALSE(movement.hasPendingMove);
    EXPECT_EQ(movement.pendingX, 0);
    EXPECT_EQ(movement.pendingY, 0);
}

/**
 * Test parameterized constructor
 */
TEST_F(GridMovementTest, ParameterizedConstruction) {
    GridMovement movement(5, 10, 2.5f);

    EXPECT_EQ(movement.targetX, 5);
    EXPECT_EQ(movement.targetY, 10);
    EXPECT_FLOAT_EQ(movement.speed, 2.5f);
    EXPECT_FALSE(movement.isMoving);  // Constructor prepares but doesn't start
    EXPECT_FLOAT_EQ(movement.progress, 0.0f);
    EXPECT_FALSE(movement.hasPendingMove);
}

/**
 * Test explicit movement start pattern
 */
TEST_F(GridMovementTest, ExplicitMovementStart) {
    GridMovement movement(5, 10, 2.5f);

    // Constructor prepares movement but doesn't start it
    EXPECT_FALSE(movement.isMoving);
    EXPECT_EQ(movement.targetX, 5);
    EXPECT_EQ(movement.targetY, 10);

    // Explicit start required (normally done via requestGridMovement())
    movement.isMoving = true;
    EXPECT_TRUE(movement.isMoving);

    // Movement can now progress
    movement.progress = 0.5f;
    EXPECT_FALSE(movement.isComplete());

    movement.progress = 1.0f;
    EXPECT_TRUE(movement.isComplete());
}

/**
 * Test movement completion detection
 */
TEST_F(GridMovementTest, MovementCompletion) {
    GridMovement movement(3, 7);
    
    // Initially not complete
    EXPECT_FALSE(movement.isComplete());
    
    // Progress through movement
    movement.progress = 0.5f;
    EXPECT_FALSE(movement.isComplete());
    
    movement.progress = 0.99f;
    EXPECT_FALSE(movement.isComplete());
    
    // Complete at 1.0
    movement.progress = 1.0f;
    EXPECT_TRUE(movement.isComplete());
    
    // Still complete beyond 1.0
    movement.progress = 1.1f;
    EXPECT_TRUE(movement.isComplete());
}

/**
 * Test movement state reset
 */
TEST_F(GridMovementTest, MovementReset) {
    GridMovement movement(8, 12, 3.0f);
    movement.progress = 0.75f;
    movement.hasPendingMove = true;
    movement.pendingX = 20;
    movement.pendingY = 25;
    
    // Reset should only stop movement, preserving target, progress, and queued moves
    movement.reset();

    EXPECT_EQ(movement.targetX, 8);      // Target preserved
    EXPECT_EQ(movement.targetY, 12);     // Target preserved
    EXPECT_FLOAT_EQ(movement.speed, 3.0f);  // Speed preserved
    EXPECT_FLOAT_EQ(movement.progress, 0.75f);  // Progress preserved (for completion verification)
    EXPECT_FALSE(movement.isMoving);     // Movement stopped
    EXPECT_TRUE(movement.hasPendingMove);   // Queued move preserved (for manual execution)
    EXPECT_EQ(movement.pendingX, 20);    // Queued target preserved
    EXPECT_EQ(movement.pendingY, 25);    // Queued target preserved
}

/**
 * Test movement queueing
 */
TEST_F(GridMovementTest, MovementQueueing) {
    GridMovement movement(5, 5);
    
    // Initially no pending move
    EXPECT_FALSE(movement.hasPendingMove);
    
    // Queue a movement
    movement.queueMove(10, 15);
    
    EXPECT_TRUE(movement.hasPendingMove);
    EXPECT_EQ(movement.pendingX, 10);
    EXPECT_EQ(movement.pendingY, 15);
    
    // Original target should be unchanged
    EXPECT_EQ(movement.targetX, 5);
    EXPECT_EQ(movement.targetY, 5);
}

/**
 * Test queued movement execution
 */
TEST_F(GridMovementTest, QueuedMovementExecution) {
    GridMovement movement;
    movement.queueMove(20, 30);
    
    // Start queued movement
    bool started = movement.startQueuedMove();
    
    EXPECT_TRUE(started);
    EXPECT_EQ(movement.targetX, 20);
    EXPECT_EQ(movement.targetY, 30);
    EXPECT_FLOAT_EQ(movement.progress, 0.0f);
    EXPECT_TRUE(movement.isMoving);
    EXPECT_FALSE(movement.hasPendingMove);  // Queue cleared
}

/**
 * Test queued movement when no move pending
 */
TEST_F(GridMovementTest, NoQueuedMovement) {
    GridMovement movement(5, 5);
    
    // No pending move to start
    bool started = movement.startQueuedMove();
    
    EXPECT_FALSE(started);
    EXPECT_EQ(movement.targetX, 5);  // Unchanged
    EXPECT_EQ(movement.targetY, 5);  // Unchanged
}

/**
 * Test component storage in ComponentArray
 */
TEST_F(GridMovementTest, ComponentArrayIntegration) {
    ComponentArray<GridMovement> movements;
    EntityManager entityManager;
    Entity entity1 = entityManager.createEntity();
    Entity entity2 = entityManager.createEntity();

    // Add components
    uint64_t gridMoveBit = 0x1;
    movements.add(entity1.id, GridMovement(10, 20), gridMoveBit, entityManager);
    movements.add(entity2.id, GridMovement(30, 40, 2.0f), gridMoveBit, entityManager);

    // Verify storage
    const GridMovement* move1 = movements.get(entity1.id);
    const GridMovement* move2 = movements.get(entity2.id);

    ASSERT_NE(move1, nullptr);
    ASSERT_NE(move2, nullptr);

    EXPECT_EQ(move1->targetX, 10);
    EXPECT_EQ(move1->targetY, 20);
    EXPECT_FLOAT_EQ(move1->speed, 1.0f);

    EXPECT_EQ(move2->targetX, 30);
    EXPECT_EQ(move2->targetY, 40);
    EXPECT_FLOAT_EQ(move2->speed, 2.0f);
}

/**
 * GridBounds component tests
 */
class GridBoundsTest : public ::testing::Test {
protected:
    void SetUp() override {
        bounds = GridBounds{};
    }
    
    GridBounds bounds;
};

/**
 * Test default GridBounds construction
 */
TEST_F(GridBoundsTest, DefaultConstruction) {
    GridBounds bounds;
    
    // Default 10x10 grid starting at origin
    EXPECT_EQ(bounds.minX, 0);
    EXPECT_EQ(bounds.minY, 0);
    EXPECT_EQ(bounds.maxX, 10);
    EXPECT_EQ(bounds.maxY, 10);
}

/**
 * Test parameterized GridBounds construction
 */
TEST_F(GridBoundsTest, ParameterizedConstruction) {
    GridBounds bounds(-5, -5, 15, 20);
    
    EXPECT_EQ(bounds.minX, -5);
    EXPECT_EQ(bounds.minY, -5);
    EXPECT_EQ(bounds.maxX, 15);
    EXPECT_EQ(bounds.maxY, 20);
}

/**
 * Test bounds validation
 */
TEST_F(GridBoundsTest, BoundsValidation) {
    GridBounds bounds(0, 0, 10, 10);
    
    // Valid coordinates
    EXPECT_TRUE(bounds.isValid(0, 0));   // Min corner
    EXPECT_TRUE(bounds.isValid(10, 10)); // Max corner
    EXPECT_TRUE(bounds.isValid(5, 5));   // Center
    
    // Invalid coordinates
    EXPECT_FALSE(bounds.isValid(-1, 0));  // Left of bounds
    EXPECT_FALSE(bounds.isValid(0, -1));  // Below bounds
    EXPECT_FALSE(bounds.isValid(11, 5));  // Right of bounds
    EXPECT_FALSE(bounds.isValid(5, 11));  // Above bounds
    EXPECT_FALSE(bounds.isValid(-1, -1)); // Below and left
    EXPECT_FALSE(bounds.isValid(15, 15)); // Above and right
}

/**
 * Test coordinate clamping
 */
TEST_F(GridBoundsTest, CoordinateClamping) {
    GridBounds bounds(2, 3, 8, 12);
    
    // Test clamping to minimum bounds
    int x = -5, y = 0;
    bounds.clamp(x, y);
    EXPECT_EQ(x, 2);  // Clamped to minX
    EXPECT_EQ(y, 3);  // Clamped to minY
    
    // Test clamping to maximum bounds
    x = 15, y = 20;
    bounds.clamp(x, y);
    EXPECT_EQ(x, 8);   // Clamped to maxX
    EXPECT_EQ(y, 12);  // Clamped to maxY
    
    // Test no clamping needed
    x = 5, y = 7;
    bounds.clamp(x, y);
    EXPECT_EQ(x, 5);   // Unchanged
    EXPECT_EQ(y, 7);   // Unchanged
    
    // Test mixed clamping
    x = 1, y = 15;  // x too low, y too high
    bounds.clamp(x, y);
    EXPECT_EQ(x, 2);   // Clamped to minX
    EXPECT_EQ(y, 12);  // Clamped to maxY
}

/**
 * Test GridBounds component storage
 */
TEST_F(GridBoundsTest, ComponentArrayIntegration) {
    ComponentArray<GridBounds> boundsArray;
    EntityManager entityManager;
    Entity entity = entityManager.createEntity();
    uint64_t boundsBit = 0x2;

    GridBounds customBounds(-10, -10, 50, 50);
    boundsArray.add(entity.id, customBounds, boundsBit, entityManager);

    const GridBounds* stored = boundsArray.get(entity.id);
    ASSERT_NE(stored, nullptr);

    EXPECT_EQ(stored->minX, -10);
    EXPECT_EQ(stored->minY, -10);
    EXPECT_EQ(stored->maxX, 50);
    EXPECT_EQ(stored->maxY, 50);

    // Test bounds functionality on stored component
    EXPECT_TRUE(stored->isValid(0, 0));
    EXPECT_FALSE(stored->isValid(-15, 0));
    EXPECT_FALSE(stored->isValid(0, 60));
}