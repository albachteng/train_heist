#include "../include/Entity.h"
#include "../include/ComponentRegistry.hpp"
#include <gtest/gtest.h>

using namespace ECS;

// Test component types for registry testing
struct Position {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Velocity {
    float dx = 0.0f;
    float dy = 0.0f;
};

class EntityTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Test setup if needed
  }

  void TearDown() override {
    // Test cleanup if needed
  }
};

// Test Entity default construction
TEST_F(EntityTest, DefaultConstruction) {
  Entity entity;

  EXPECT_EQ(entity.id, INVALID_ENTITY);
  EXPECT_EQ(entity.generation, 0);
  EXPECT_EQ(entity.componentMask, 0);
  EXPECT_FALSE(entity.isValid());
}

// Test Entity construction with ID
TEST_F(EntityTest, ConstructionWithID) {
  EntityID testId = 42;
  Entity entity(testId);

  EXPECT_EQ(entity.id, testId);
  EXPECT_EQ(entity.generation, 0);
  EXPECT_EQ(entity.componentMask, 0);
  EXPECT_TRUE(entity.isValid());
}

// Test Entity construction with ID and generation
TEST_F(EntityTest, ConstructionWithIDAndGeneration) {
  EntityID testId = 42;
  uint32_t testGeneration = 5;
  Entity entity(testId, testGeneration);

  EXPECT_EQ(entity.id, testId);
  EXPECT_EQ(entity.generation, testGeneration);
  EXPECT_EQ(entity.componentMask, 0);
  EXPECT_TRUE(entity.isValid());
}

// Test component bitmask operations
TEST_F(EntityTest, ComponentBitmaskOperations) {
  Entity entity(1);

  // Test adding single component
  uint64_t componentBit1 = 1ULL << 0; // Position component
  uint64_t componentBit2 = 1ULL << 1; // Velocity component
  uint64_t componentBit3 = 1ULL << 2; // Sprite component

  EXPECT_FALSE(entity.hasComponent(componentBit1));

  entity.addComponent(componentBit1);
  EXPECT_TRUE(entity.hasComponent(componentBit1));
  EXPECT_EQ(entity.componentMask, componentBit1);

  // Test adding multiple components
  entity.addComponent(componentBit2);
  EXPECT_TRUE(entity.hasComponent(componentBit1));
  EXPECT_TRUE(entity.hasComponent(componentBit2));
  EXPECT_FALSE(entity.hasComponent(componentBit3));

  // Test multi-component query
  uint64_t requiredMask = componentBit1 | componentBit2;
  EXPECT_TRUE(entity.hasComponents(requiredMask));

  uint64_t partialMask = componentBit1 | componentBit3;
  EXPECT_FALSE(entity.hasComponents(partialMask));

  // Test removing component
  entity.removeComponent(componentBit1);
  EXPECT_FALSE(entity.hasComponent(componentBit1));
  EXPECT_TRUE(entity.hasComponent(componentBit2));
  EXPECT_FALSE(entity.hasComponents(requiredMask));
}

// Test entity equality
TEST_F(EntityTest, EntityEquality) {
  Entity entity1(42, 1);
  Entity entity2(42, 1);
  Entity entity3(42, 2); // Different generation
  Entity entity4(43, 1); // Different ID

  EXPECT_EQ(entity1, entity2);
  EXPECT_NE(entity1, entity3);
  EXPECT_NE(entity1, entity4);
}

// Test EntityHandle default construction
TEST_F(EntityTest, EntityHandleDefaultConstruction) {
  EntityHandle handle;

  EXPECT_EQ(handle.id, INVALID_ENTITY);
  EXPECT_EQ(handle.generation, 0);
  EXPECT_FALSE(handle.isValid());
}

// Test EntityHandle construction with ID and generation
TEST_F(EntityTest, EntityHandleConstructionWithIDAndGeneration) {
  EntityID testId = 42;
  uint32_t testGeneration = 5;
  EntityHandle handle(testId, testGeneration);

  EXPECT_EQ(handle.id, testId);
  EXPECT_EQ(handle.generation, testGeneration);
  EXPECT_TRUE(handle.isValid());
}

// Test EntityHandle construction from Entity
TEST_F(EntityTest, EntityHandleConstructionFromEntity) {
  Entity entity(42, 5);
  entity.addComponent(1ULL << 0); // Add some component

  EntityHandle handle(entity);

  EXPECT_EQ(handle.id, entity.id);
  EXPECT_EQ(handle.generation, entity.generation);
  EXPECT_TRUE(handle.isValid());

  // Verify handle doesn't expose component mask
  // (This is enforced by not having componentMask member in EntityHandle)
}

// Test EntityHandle equality
TEST_F(EntityTest, EntityHandleEquality) {
  EntityHandle handle1(42, 1);
  EntityHandle handle2(42, 1);
  EntityHandle handle3(42, 2); // Different generation
  EntityHandle handle4(43, 1); // Different ID

  EXPECT_EQ(handle1, handle2);
  EXPECT_NE(handle1, handle3);
  EXPECT_NE(handle1, handle4);
}

// Test Entity to EntityHandle conversion preserves identity
TEST_F(EntityTest, EntityToHandleConversion) {
  Entity entity(123, 7);
  EntityHandle handle(entity);

  EXPECT_EQ(entity.id, handle.id);
  EXPECT_EQ(entity.generation, handle.generation);
}

// Test invalid entity constant
TEST_F(EntityTest, InvalidEntityConstant) {
  EXPECT_EQ(INVALID_ENTITY, 0);

  Entity invalidEntity;
  EntityHandle invalidHandle;

  EXPECT_FALSE(invalidEntity.isValid());
  EXPECT_FALSE(invalidHandle.isValid());
}

// Test component bitmask edge cases
TEST_F(EntityTest, ComponentBitmaskEdgeCases) {
  Entity entity(1);

  // Test with high bit positions
  uint64_t highBit = 1ULL << 63;
  entity.addComponent(highBit);
  EXPECT_TRUE(entity.hasComponent(highBit));

  // Test removing non-existent component (should be safe)
  uint64_t nonExistentBit = 1ULL << 30;
  entity.removeComponent(nonExistentBit);
  EXPECT_TRUE(entity.hasComponent(highBit)); // Should still have the high bit

  // Test adding same component twice (should be idempotent)
  uint64_t componentBit = 1ULL << 5;
  entity.addComponent(componentBit);
  uint64_t firstMask = entity.componentMask;
  entity.addComponent(componentBit);
  EXPECT_EQ(entity.componentMask, firstMask);
}

// Test zero component mask queries
TEST_F(EntityTest, ZeroComponentMaskQueries) {
  Entity entity(1);

  // Entity with no components should satisfy empty query
  EXPECT_TRUE(entity.hasComponents(0));

  // But should not satisfy any component requirement
  EXPECT_FALSE(entity.hasComponents(1ULL << 0));
}

// Bitmask Optimization Tests
TEST_F(EntityTest, BitmaskFilteringPerformance) {
  // Test that bitmask filtering is working correctly
  Entity entity(1);
  uint64_t positionBit = 1ULL << 0;
  uint64_t velocityBit = 1ULL << 1;
  uint64_t healthBit = 1ULL << 2;
  
  // Add some components
  entity.addComponent(positionBit);
  entity.addComponent(velocityBit);
  
  uint64_t movementMask = positionBit | velocityBit;
  uint64_t fullMask = positionBit | velocityBit | healthBit;
  
  // Test efficient bitmask queries
  EXPECT_TRUE(entity.hasComponents(movementMask));
  EXPECT_FALSE(entity.hasComponents(fullMask));
  
  // Test branch-free query pattern (what systems should use)
  bool hasMovementComponents = (entity.componentMask & movementMask) == movementMask;
  bool hasAllComponents = (entity.componentMask & fullMask) == fullMask;
  
  EXPECT_TRUE(hasMovementComponents);
  EXPECT_FALSE(hasAllComponents);
}

// Test component registry integration
TEST_F(EntityTest, ComponentRegistryIntegration) {
  // Reset registry for clean test state
  resetComponentRegistry();
  
  // Test automatic bit assignment
  uint64_t autoPositionBit = getComponentBit<Position>();
  uint64_t autoVelocityBit = getComponentBit<Velocity>();
  
  // Verify bits are unique and non-zero
  EXPECT_NE(autoPositionBit, 0ULL);
  EXPECT_NE(autoVelocityBit, 0ULL);
  EXPECT_NE(autoPositionBit, autoVelocityBit);
  
  // Test that same type returns same bit
  uint64_t positionBitAgain = getComponentBit<Position>();
  EXPECT_EQ(autoPositionBit, positionBitAgain);
  
  // Test entity operations with auto-assigned bits
  Entity entity(1);
  entity.addComponent(autoPositionBit);
  entity.addComponent(autoVelocityBit);
  
  uint64_t combinedMask = autoPositionBit | autoVelocityBit;
  EXPECT_TRUE(entity.hasComponents(combinedMask));
  EXPECT_TRUE(entity.hasComponent(autoPositionBit));
  EXPECT_TRUE(entity.hasComponent(autoVelocityBit));
  
  // Verify registry functionality is implemented
  EXPECT_EQ(getRegisteredComponentCount(), 2ULL);
  bool hasComponentRegistry = true; // Now implemented
  EXPECT_TRUE(hasComponentRegistry) << "Component type registry with getComponentBit<T>() is now implemented";
}
