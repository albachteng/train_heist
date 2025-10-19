#include <gtest/gtest.h>
#include "../include/Physics.hpp"
#include "../../ecs/include/ComponentArray.hpp"
#include "../../ecs/include/EntityManager.hpp"
#include <cmath>

using namespace ECS;

/**
 * Test fixture for Velocity component
 */
class VelocityTest : public ::testing::Test {
protected:
    void SetUp() override {
        velocity = Velocity{};
    }
    
    Velocity velocity;
};

/**
 * Test Velocity default construction and ZII compliance
 */
TEST_F(VelocityTest, DefaultConstruction) {
    Velocity vel;
    
    // Zero Initialization Invariant (ZII)
    EXPECT_FLOAT_EQ(vel.dx, 0.0f);
    EXPECT_FLOAT_EQ(vel.dy, 0.0f);
}

/**
 * Test Velocity parameterized construction
 */
TEST_F(VelocityTest, ParameterizedConstruction) {
    Velocity vel(3.5f, -2.1f);
    
    EXPECT_FLOAT_EQ(vel.dx, 3.5f);
    EXPECT_FLOAT_EQ(vel.dy, -2.1f);
}

/**
 * Test velocity magnitude calculation
 */
TEST_F(VelocityTest, MagnitudeCalculation) {
    // Test zero vector
    Velocity vel(0.0f, 0.0f);
    EXPECT_FLOAT_EQ(vel.magnitude(), 0.0f);
    
    // Test unit vectors
    vel = Velocity(1.0f, 0.0f);
    EXPECT_FLOAT_EQ(vel.magnitude(), 1.0f);
    
    vel = Velocity(0.0f, 1.0f);
    EXPECT_FLOAT_EQ(vel.magnitude(), 1.0f);
    
    // Test 3-4-5 triangle
    vel = Velocity(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(vel.magnitude(), 5.0f);
    
    // Test negative values
    vel = Velocity(-3.0f, -4.0f);
    EXPECT_FLOAT_EQ(vel.magnitude(), 5.0f);
}

/**
 * Test velocity normalization
 */
TEST_F(VelocityTest, Normalization) {
    // Test zero vector (should remain zero)
    Velocity vel(0.0f, 0.0f);
    vel.normalize();
    EXPECT_FLOAT_EQ(vel.dx, 0.0f);
    EXPECT_FLOAT_EQ(vel.dy, 0.0f);
    
    // Test unit vector (should remain unchanged)
    vel = Velocity(1.0f, 0.0f);
    vel.normalize();
    EXPECT_FLOAT_EQ(vel.dx, 1.0f);
    EXPECT_FLOAT_EQ(vel.dy, 0.0f);
    
    // Test normalization of larger vector
    vel = Velocity(3.0f, 4.0f);  // Magnitude 5
    vel.normalize();
    EXPECT_FLOAT_EQ(vel.dx, 0.6f);   // 3/5
    EXPECT_FLOAT_EQ(vel.dy, 0.8f);   // 4/5
    EXPECT_NEAR(vel.magnitude(), 1.0f, 0.001f);
}

/**
 * Test velocity scaling
 */
TEST_F(VelocityTest, Scaling) {
    Velocity vel(2.0f, -3.0f);
    
    // Scale by positive factor
    vel.scale(2.5f);
    EXPECT_FLOAT_EQ(vel.dx, 5.0f);
    EXPECT_FLOAT_EQ(vel.dy, -7.5f);
    
    // Scale by zero
    vel.scale(0.0f);
    EXPECT_FLOAT_EQ(vel.dx, 0.0f);
    EXPECT_FLOAT_EQ(vel.dy, 0.0f);
    
    // Scale by negative factor
    vel = Velocity(1.0f, 2.0f);
    vel.scale(-0.5f);
    EXPECT_FLOAT_EQ(vel.dx, -0.5f);
    EXPECT_FLOAT_EQ(vel.dy, -1.0f);
}

/**
 * Test velocity zero detection
 */
TEST_F(VelocityTest, ZeroDetection) {
    // Exactly zero
    Velocity vel(0.0f, 0.0f);
    EXPECT_TRUE(vel.isZero());
    EXPECT_TRUE(vel.isZero(0.1f));  // With larger epsilon
    
    // Very small values (within default epsilon)
    vel = Velocity(0.0005f, -0.0005f);
    EXPECT_TRUE(vel.isZero());
    
    // Small but above epsilon
    vel = Velocity(0.002f, 0.0f);
    EXPECT_FALSE(vel.isZero());
    
    // Custom epsilon
    vel = Velocity(0.05f, 0.03f);
    EXPECT_FALSE(vel.isZero(0.01f));  // Above epsilon
    EXPECT_TRUE(vel.isZero(0.1f));    // Below epsilon
    
    // Non-zero values
    vel = Velocity(1.0f, 0.5f);
    EXPECT_FALSE(vel.isZero());
}

/**
 * Test fixture for Acceleration component
 */
class AccelerationTest : public ::testing::Test {
protected:
    void SetUp() override {
        acceleration = Acceleration{};
    }
    
    Acceleration acceleration;
};

/**
 * Test Acceleration default construction and ZII compliance
 */
TEST_F(AccelerationTest, DefaultConstruction) {
    Acceleration accel;
    
    // Zero Initialization Invariant (ZII)
    EXPECT_FLOAT_EQ(accel.dx, 0.0f);
    EXPECT_FLOAT_EQ(accel.dy, 0.0f);
    EXPECT_FLOAT_EQ(accel.damping, 1.0f);  // No damping by default
}

/**
 * Test Acceleration parameterized construction
 */
TEST_F(AccelerationTest, ParameterizedConstruction) {
    Acceleration accel(1.5f, -0.8f, 0.95f);
    
    EXPECT_FLOAT_EQ(accel.dx, 1.5f);
    EXPECT_FLOAT_EQ(accel.dy, -0.8f);
    EXPECT_FLOAT_EQ(accel.damping, 0.95f);
}

/**
 * Test acceleration application to velocity
 */
TEST_F(AccelerationTest, VelocityApplication) {
    Velocity vel(2.0f, 3.0f);
    Acceleration accel(0.5f, -1.0f, 1.0f);  // No damping
    
    accel.applyTo(vel);
    
    EXPECT_FLOAT_EQ(vel.dx, 2.5f);  // 2.0 + 0.5
    EXPECT_FLOAT_EQ(vel.dy, 2.0f);  // 3.0 + (-1.0)
}

/**
 * Test acceleration with damping
 */
TEST_F(AccelerationTest, VelocityApplicationWithDamping) {
    Velocity vel(10.0f, -5.0f);
    Acceleration accel(1.0f, 2.0f, 0.9f);  // 10% damping
    
    accel.applyTo(vel);
    
    // First apply acceleration: (10+1, -5+2) = (11, -3)
    // Then apply damping: (11*0.9, -3*0.9) = (9.9, -2.7)
    EXPECT_FLOAT_EQ(vel.dx, 9.9f);
    EXPECT_FLOAT_EQ(vel.dy, -2.7f);
}

/**
 * Test acceleration with full damping (immediate stop)
 */
TEST_F(AccelerationTest, FullDamping) {
    Velocity vel(5.0f, 8.0f);
    Acceleration accel(0.0f, 0.0f, 0.0f);  // Full damping, no acceleration
    
    accel.applyTo(vel);
    
    EXPECT_FLOAT_EQ(vel.dx, 0.0f);
    EXPECT_FLOAT_EQ(vel.dy, 0.0f);
}

/**
 * Test acceleration zero detection
 */
TEST_F(AccelerationTest, ZeroDetection) {
    // Exactly zero
    Acceleration accel(0.0f, 0.0f);
    EXPECT_TRUE(accel.isZero());
    
    // Very small values
    accel = Acceleration(0.0005f, -0.0003f);
    EXPECT_TRUE(accel.isZero());
    
    // Above epsilon
    accel = Acceleration(0.002f, 0.0f);
    EXPECT_FALSE(accel.isZero());
    
    // Non-zero values
    accel = Acceleration(0.1f, 0.05f);
    EXPECT_FALSE(accel.isZero());
}

/**
 * Test fixture for MovementConstraints component
 */
class MovementConstraintsTest : public ::testing::Test {
protected:
    void SetUp() override {
        constraints = MovementConstraints{};
    }
    
    MovementConstraints constraints;
};

/**
 * Test MovementConstraints default construction
 */
TEST_F(MovementConstraintsTest, DefaultConstruction) {
    MovementConstraints constraints;
    
    EXPECT_FLOAT_EQ(constraints.maxSpeed, 1000.0f);
    EXPECT_FALSE(constraints.constrainToGrid);
    EXPECT_TRUE(constraints.allowDiagonal);
    EXPECT_TRUE(constraints.allowContinuous);
    EXPECT_FLOAT_EQ(constraints.gridSize, 32.0f);
}

/**
 * Test maximum speed constraint
 */
TEST_F(MovementConstraintsTest, MaxSpeedConstraint) {
    MovementConstraints constraints;
    constraints.maxSpeed = 5.0f;
    
    // Velocity within limit (should be unchanged)
    Velocity vel(3.0f, 4.0f);  // Magnitude 5.0
    constraints.applyTo(vel);
    EXPECT_FLOAT_EQ(vel.dx, 3.0f);
    EXPECT_FLOAT_EQ(vel.dy, 4.0f);
    
    // Velocity exceeding limit (should be clamped)
    vel = Velocity(6.0f, 8.0f);  // Magnitude 10.0
    constraints.applyTo(vel);
    EXPECT_NEAR(vel.magnitude(), 5.0f, 0.001f);
    // Direction should be preserved: (6,8) normalized = (0.6, 0.8)
    EXPECT_NEAR(vel.dx, 3.0f, 0.001f);  // 0.6 * 5.0
    EXPECT_NEAR(vel.dy, 4.0f, 0.001f);  // 0.8 * 5.0
}

/**
 * Test diagonal movement constraint
 */
TEST_F(MovementConstraintsTest, DiagonalConstraint) {
    MovementConstraints constraints;
    constraints.allowDiagonal = false;
    
    // Horizontal dominates
    Velocity vel(5.0f, 2.0f);
    constraints.applyTo(vel);
    EXPECT_FLOAT_EQ(vel.dx, 5.0f);  // Preserved
    EXPECT_FLOAT_EQ(vel.dy, 0.0f);  // Zeroed
    
    // Vertical dominates
    vel = Velocity(1.0f, 8.0f);
    constraints.applyTo(vel);
    EXPECT_FLOAT_EQ(vel.dx, 0.0f);  // Zeroed
    EXPECT_FLOAT_EQ(vel.dy, 8.0f);  // Preserved
    
    // Equal values (horizontal wins by implementation)
    vel = Velocity(3.0f, 3.0f);
    constraints.applyTo(vel);
    EXPECT_FLOAT_EQ(vel.dx, 3.0f);  // Preserved
    EXPECT_FLOAT_EQ(vel.dy, 0.0f);  // Zeroed
}

/**
 * Test grid snapping constraint
 */
TEST_F(MovementConstraintsTest, GridSnapping) {
    MovementConstraints constraints;
    constraints.constrainToGrid = true;
    constraints.gridSize = 10.0f;
    
    // Values that snap to grid
    Velocity vel(7.0f, 13.0f);  // Should snap to (10, 10)
    constraints.applyTo(vel);
    EXPECT_FLOAT_EQ(vel.dx, 10.0f);
    EXPECT_FLOAT_EQ(vel.dy, 10.0f);
    
    // Negative values
    vel = Velocity(-8.0f, 4.0f);  // Should snap to (-10, 0)
    constraints.applyTo(vel);
    EXPECT_FLOAT_EQ(vel.dx, -10.0f);
    EXPECT_FLOAT_EQ(vel.dy, 0.0f);
    
    // Already aligned
    vel = Velocity(20.0f, -30.0f);
    constraints.applyTo(vel);
    EXPECT_FLOAT_EQ(vel.dx, 20.0f);
    EXPECT_FLOAT_EQ(vel.dy, -30.0f);
}

/**
 * Test combined constraints (speed + diagonal)
 */
TEST_F(MovementConstraintsTest, CombinedConstraints) {
    MovementConstraints constraints;
    constraints.maxSpeed = 10.0f;
    constraints.allowDiagonal = false;
    
    // Large diagonal vector: should remove diagonal, then clamp speed
    Velocity vel(15.0f, 12.0f);  // Horizontal dominates
    constraints.applyTo(vel);
    
    EXPECT_FLOAT_EQ(vel.dx, 10.0f);  // Clamped to max speed
    EXPECT_FLOAT_EQ(vel.dy, 0.0f);   // Diagonal removed
}

/**
 * Test physics components in ComponentArray storage
 */
TEST_F(VelocityTest, ComponentArrayIntegration) {
    ComponentArray<Velocity> velocities;
    ComponentArray<Acceleration> accelerations;
    ComponentArray<MovementConstraints> constraints;

    EntityManager entityManager;
    Entity entity = entityManager.createEntity();
    uint64_t velBit = 0x1;
    uint64_t accelBit = 0x2;
    uint64_t constraintBit = 0x4;

    // Add components
    velocities.add(entity.id, Velocity(5.0f, -3.0f), velBit, entityManager);
    accelerations.add(entity.id, Acceleration(0.1f, 0.2f, 0.98f), accelBit, entityManager);
    constraints.add(entity.id, MovementConstraints(), constraintBit, entityManager);
    
    // Verify storage
    const Velocity* vel = velocities.get(entity.id);
    const Acceleration* accel = accelerations.get(entity.id);
    const MovementConstraints* constraint = constraints.get(entity.id);
    
    ASSERT_NE(vel, nullptr);
    ASSERT_NE(accel, nullptr);
    ASSERT_NE(constraint, nullptr);
    
    EXPECT_FLOAT_EQ(vel->dx, 5.0f);
    EXPECT_FLOAT_EQ(vel->dy, -3.0f);
    
    EXPECT_FLOAT_EQ(accel->dx, 0.1f);
    EXPECT_FLOAT_EQ(accel->dy, 0.2f);
    EXPECT_FLOAT_EQ(accel->damping, 0.98f);
    
    EXPECT_FLOAT_EQ(constraint->maxSpeed, 1000.0f);
}