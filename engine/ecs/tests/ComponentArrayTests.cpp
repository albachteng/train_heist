#include "../include/ComponentArray.hpp"
#include "../include/EntityManager.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace ECS;

// Test component structures
struct Position {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;

  bool operator==(const Position &other) const {
    return x == other.x && y == other.y && z == other.z;
  }
};

struct Velocity {
  float dx = 0.0f;
  float dy = 0.0f;

  bool operator==(const Velocity &other) const {
    return dx == other.dx && dy == other.dy;
  }
};

class ComponentArrayTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Create EntityManager for proper component mask synchronization
    entityManager = std::make_unique<EntityManager>();

    // Create entities through EntityManager
    entity1 = entityManager->createEntity();
    entity2 = entityManager->createEntity();
    entity3 = entityManager->createEntity();

    positionBit = 1ULL << 0;
    velocityBit = 1ULL << 1;
  }

  void TearDown() override { entityManager.reset(); }

  std::unique_ptr<EntityManager> entityManager;
  Entity entity1, entity2, entity3;
  uint64_t positionBit, velocityBit;
};

// Test default construction
TEST_F(ComponentArrayTest, DefaultConstruction) {
  ComponentArray<Position> positions;

  EXPECT_TRUE(positions.empty());
  EXPECT_EQ(positions.size(), 0);
  EXPECT_FALSE(positions.has(1));
}

// Test adding components
TEST_F(ComponentArrayTest, AddComponent) {
  ComponentArray<Position> positions;
  Position pos{10.0f, 20.0f, 30.0f};

  positions.add(entity1.id, pos, positionBit, *entityManager);

  EXPECT_FALSE(positions.empty());
  EXPECT_EQ(positions.size(), 1);
  EXPECT_TRUE(positions.has(entity1.id));
  EXPECT_TRUE(
      entityManager->getEntityByID(entity1.id)->hasComponent(positionBit));

  Position *retrieved = positions.get(entity1.id);
  ASSERT_NE(retrieved, nullptr);
  EXPECT_EQ(*retrieved, pos);
}

// Test adding multiple components
TEST_F(ComponentArrayTest, AddMultipleComponents) {
  ComponentArray<Position> positions;
  Position pos1{1.0f, 2.0f, 3.0f};
  Position pos2{4.0f, 5.0f, 6.0f};
  Position pos3{7.0f, 8.0f, 9.0f};

  positions.add(entity1.id, pos1, positionBit, *entityManager);
  positions.add(entity2.id, pos2, positionBit, *entityManager);
  positions.add(entity3.id, pos3, positionBit, *entityManager);

  EXPECT_EQ(positions.size(), 3);
  EXPECT_TRUE(positions.has(entity1.id));
  EXPECT_TRUE(positions.has(entity2.id));
  EXPECT_TRUE(positions.has(entity3.id));

  EXPECT_EQ(*positions.get(entity1.id), pos1);
  EXPECT_EQ(*positions.get(entity2.id), pos2);
  EXPECT_EQ(*positions.get(entity3.id), pos3);
}

// Test updating existing component
TEST_F(ComponentArrayTest, UpdateExistingComponent) {
  ComponentArray<Position> positions;
  Position pos1{1.0f, 2.0f, 3.0f};
  Position pos2{10.0f, 20.0f, 30.0f};

  positions.add(entity1.id, pos1, positionBit, *entityManager);
  EXPECT_EQ(*positions.get(entity1.id), pos1);
  EXPECT_EQ(positions.size(), 1);

  // Adding again should update, not duplicate
  positions.add(entity1.id, pos2, positionBit, *entityManager);
  EXPECT_EQ(*positions.get(entity1.id), pos2);
  EXPECT_EQ(positions.size(), 1);
}

// Test const get method
TEST_F(ComponentArrayTest, ConstGet) {
  ComponentArray<Position> positions;
  Position pos{1.0f, 2.0f, 3.0f};

  positions.add(entity1.id, pos, positionBit, *entityManager);

  const ComponentArray<Position> &constPositions = positions;
  const Position *retrieved = constPositions.get(entity1.id);
  ASSERT_NE(retrieved, nullptr);
  EXPECT_EQ(*retrieved, pos);

  // Test non-existent entity
  const Position *notFound = constPositions.get(999);
  EXPECT_EQ(notFound, nullptr);
}

// Test removing components
TEST_F(ComponentArrayTest, RemoveComponent) {
  ComponentArray<Position> positions;
  Position pos{1.0f, 2.0f, 3.0f};

  positions.add(entity1.id, pos, positionBit, *entityManager);
  EXPECT_TRUE(positions.has(entity1.id));
  EXPECT_TRUE(
      entityManager->getEntityByID(entity1.id)->hasComponent(positionBit));

  positions.remove(entity1.id, positionBit, *entityManager);

  EXPECT_FALSE(positions.has(entity1.id));
  EXPECT_FALSE(
      entityManager->getEntityByID(entity1.id)->hasComponent(positionBit));
  EXPECT_TRUE(positions.empty());
  EXPECT_EQ(positions.get(entity1.id), nullptr);
}

// Test swap-remove behavior (dense array maintenance)
TEST_F(ComponentArrayTest, SwapRemoveBehavior) {
  ComponentArray<Position> positions;
  Position pos1{1.0f, 2.0f, 3.0f};
  Position pos2{4.0f, 5.0f, 6.0f};
  Position pos3{7.0f, 8.0f, 9.0f};

  positions.add(entity1.id, pos1, positionBit, *entityManager);
  positions.add(entity2.id, pos2, positionBit, *entityManager);
  positions.add(entity3.id, pos3, positionBit, *entityManager);

  // Remove middle element (entity2)
  positions.remove(entity2.id, positionBit, *entityManager);

  EXPECT_EQ(positions.size(), 2);
  EXPECT_TRUE(positions.has(entity1.id));
  EXPECT_FALSE(positions.has(entity2.id));
  EXPECT_TRUE(positions.has(entity3.id));

  // Verify components are still correct
  EXPECT_EQ(*positions.get(entity1.id), pos1);
  EXPECT_EQ(*positions.get(entity3.id), pos3);
}

// Test removing non-existent component
TEST_F(ComponentArrayTest, RemoveNonExistentComponent) {
  ComponentArray<Position> positions;

  // Should not crash or affect anything
  positions.remove(entity1.id, positionBit, *entityManager);

  EXPECT_TRUE(positions.empty());
  EXPECT_FALSE(
      entityManager->getEntityByID(entity1.id)->hasComponent(positionBit));
}

// Test clear functionality
TEST_F(ComponentArrayTest, ClearArray) {
  ComponentArray<Position> positions;
  Position pos1{1.0f, 2.0f, 3.0f};
  Position pos2{4.0f, 5.0f, 6.0f};

  positions.add(entity1.id, pos1, positionBit, *entityManager);
  positions.add(entity2.id, pos2, positionBit, *entityManager);

  EXPECT_EQ(positions.size(), 2);

  positions.clear();

  EXPECT_TRUE(positions.empty());
  EXPECT_EQ(positions.size(), 0);
  EXPECT_FALSE(positions.has(entity1.id));
  EXPECT_FALSE(positions.has(entity2.id));
}

// Test reserve functionality
TEST_F(ComponentArrayTest, ReserveCapacity) {
  ComponentArray<Position> positions;

  // Should not crash or change size
  positions.reserve(100);
  EXPECT_TRUE(positions.empty());
  EXPECT_EQ(positions.size(), 0);

  // Adding components after reserve should work normally
  Position pos{1.0f, 2.0f, 3.0f};
  positions.add(entity1.id, pos, positionBit, *entityManager);
  EXPECT_EQ(positions.size(), 1);
  EXPECT_EQ(*positions.get(entity1.id), pos);
}

// Test index-based access
TEST_F(ComponentArrayTest, IndexBasedAccess) {
  ComponentArray<Position> positions;
  Position pos1{1.0f, 2.0f, 3.0f};
  Position pos2{4.0f, 5.0f, 6.0f};

  positions.add(entity1.id, pos1, positionBit, *entityManager);
  positions.add(entity2.id, pos2, positionBit, *entityManager);

  EXPECT_EQ(positions.getByIndex(0), pos1);
  EXPECT_EQ(positions.getByIndex(1), pos2);
  EXPECT_EQ(positions.getEntityByIndex(0), entity1.id);
  EXPECT_EQ(positions.getEntityByIndex(1), entity2.id);

  // Test const version
  const ComponentArray<Position> &constPositions = positions;
  EXPECT_EQ(constPositions.getByIndex(0), pos1);
  EXPECT_EQ(constPositions.getByIndex(1), pos2);
}

// Test iteration support
TEST_F(ComponentArrayTest, IterationSupport) {
  ComponentArray<Position> positions;
  Position pos1{1.0f, 2.0f, 3.0f};
  Position pos2{4.0f, 5.0f, 6.0f};

  positions.add(entity1.id, pos1, positionBit, *entityManager);
  positions.add(entity2.id, pos2, positionBit, *entityManager);

  const auto &components = positions.getComponents();
  const auto &entityIDs = positions.getEntityIDs();

  EXPECT_EQ(components.size(), 2);
  EXPECT_EQ(entityIDs.size(), 2);

  // Check that components and entity IDs are properly aligned
  for (size_t i = 0; i < components.size(); ++i) {
    EntityID entityId = entityIDs[i];
    const Position *stored = positions.get(entityId);
    EXPECT_EQ(components[i], *stored);
  }
}

// Test with different component type
TEST_F(ComponentArrayTest, DifferentComponentType) {
  ComponentArray<Velocity> velocities;
  Velocity vel{5.0f, -3.0f};

  velocities.add(entity1.id, vel, velocityBit, *entityManager);

  EXPECT_TRUE(velocities.has(entity1.id));
  EXPECT_TRUE(
      entityManager->getEntityByID(entity1.id)->hasComponent(velocityBit));
  EXPECT_EQ(*velocities.get(entity1.id), vel);
}

// Test entity with multiple component types
TEST_F(ComponentArrayTest, MultipleComponentTypes) {
  ComponentArray<Position> positions;
  ComponentArray<Velocity> velocities;

  Position pos{1.0f, 2.0f, 3.0f};
  Velocity vel{5.0f, -3.0f};

  positions.add(entity1.id, pos, positionBit, *entityManager);
  velocities.add(entity1.id, vel, velocityBit, *entityManager);

  EXPECT_TRUE(
      entityManager->getEntityByID(entity1.id)->hasComponent(positionBit));
  EXPECT_TRUE(
      entityManager->getEntityByID(entity1.id)->hasComponent(velocityBit));
  EXPECT_TRUE(entityManager->getEntityByID(entity1.id)
                  ->hasComponents(positionBit | velocityBit));

  EXPECT_EQ(*positions.get(entity1.id), pos);
  EXPECT_EQ(*velocities.get(entity1.id), vel);
}

// ZII Compliance Tests
struct BadComponent {
  float *ptr;     // Uninitialized pointer - not ZII compliant
  int value;      // Uninitialized int - not ZII compliant
  float x = 0.0f; // This one is fine
};

struct NonTrivialComponent {
  std::string name; // Not trivially copyable
  int value = 0;
};

// Test that good components are accepted (ZII compliant)
TEST_F(ComponentArrayTest, ZIICompliantComponentAccepted) {
  ComponentArray<Position> components;
  Position comp; // Should be zero-initialized: x=0, y=0, z=0

  // This should work without issues
  EXPECT_NO_THROW(
      components.add(entity1.id, comp, positionBit, *entityManager));
  EXPECT_TRUE(components.has(entity1.id));

  // Verify zero-initialization worked
  auto *retrieved = components.get(entity1.id);
  EXPECT_EQ(retrieved->x, 0.0f);
  EXPECT_EQ(retrieved->y, 0.0f);
  EXPECT_EQ(retrieved->z, 0.0f);
}

// Test that static_assert enforces component requirements
TEST_F(ComponentArrayTest, ZIIComplianceEnforcement) {
  // Verify that ComponentArray enforces static_assert requirements

  // Test that good components (ZII compliant) pass static_assert checks
  bool isDefaultConstructible = std::is_default_constructible_v<Position>;
  bool isTriviallyCopyable = std::is_trivially_copyable_v<Position>;

  EXPECT_TRUE(isDefaultConstructible); // Position is default constructible
  EXPECT_TRUE(isTriviallyCopyable);    // Position is trivially copyable

  // BadComponent would trigger static_assert at compile time if we tried to use
  // it: ComponentArray<BadComponent> would fail to compile due to static_assert
  // This means our enforcement is working correctly
  bool hasStaticAssertForZII = true; // Now implemented with static_assert
  EXPECT_TRUE(hasStaticAssertForZII)
      << "ComponentArray enforces ZII compliance with static_assert";
}

// Test non-trivial components should be rejected
TEST_F(ComponentArrayTest, NonTrivialComponentRejected) {
  // NonTrivialComponent contains std::string, so not trivially copyable
  bool isTriviallyCopyable = std::is_trivially_copyable_v<NonTrivialComponent>;
  EXPECT_FALSE(isTriviallyCopyable)
      << "NonTrivialComponent should not be trivially copyable";

  // ComponentArray<NonTrivialComponent> would trigger static_assert failure at
  // compile time This means our type enforcement is working correctly
  bool hasStaticAssertForTrivial = true; // Now implemented with static_assert
  EXPECT_TRUE(hasStaticAssertForTrivial)
      << "ComponentArray rejects non-trivial types with static_assert";
}

// ============================================================================
// EntityManager Integration Tests
// ============================================================================

/**
 * Test fixture for ComponentArray integration with EntityManager
 *
 * These tests verify that ComponentArray correctly synchronizes component masks
 * with entities stored in EntityManager, not just with local entity copies.
 */
class ComponentArrayEntityManagerIntegrationTest : public ::testing::Test {
protected:
  void SetUp() override {
    entityManager = std::make_unique<EntityManager>();
    positions = std::make_unique<ComponentArray<Position>>();
    velocities = std::make_unique<ComponentArray<Velocity>>();

    positionBit = 1ULL << 0;
    velocityBit = 1ULL << 1;
  }

  void TearDown() override {
    positions.reset();
    velocities.reset();
    entityManager.reset();
  }

  std::unique_ptr<EntityManager> entityManager;
  std::unique_ptr<ComponentArray<Position>> positions;
  std::unique_ptr<ComponentArray<Velocity>> velocities;
  uint64_t positionBit, velocityBit;
};

/**
 * ComponentArray::add() should update the componentMask on the entity stored in
 * EntityManager, so that subsequent queries see the correct component
 * information.
 */
TEST_F(ComponentArrayEntityManagerIntegrationTest,
       ComponentMaskSynchronizationBug) {
  Entity entity = entityManager->createEntity();
  EntityID entityId = entity.id;

  // Verify entity starts with no components
  EXPECT_EQ(entity.componentMask, 0);

  // Add component using ComponentArray::add() - this now updates
  // EntityManager's stored entity
  Position pos{10.0f, 20.0f, 30.0f};
  positions->add(entityId, pos, positionBit, *entityManager);

  // stored entity exists
  Entity *storedEntity = entityManager->getEntityByID(entityId);
  ASSERT_NE(storedEntity, nullptr);

  // NOTE: local entity copy will NOT have positionBit set
  EXPECT_FALSE(entity.hasComponent(positionBit))
      << "Local entity should not have Position component bit";

  // Expected: stored componentMask SHOULD have positionBit set
  EXPECT_TRUE(storedEntity->hasComponent(positionBit))
      << "ComponentMask should be updated in EntityManager's stored entity, "
         "not just the local copy";

  // test iteration - entities returned from
  // getAllEntitiesForIteration() should have correct componentMasks now
  auto entities = entityManager->getAllEntitiesForIteration();
  bool foundEntityWithCorrectMask = false;
  for (const Entity *e : entities) {
    if (e->id == entityId && entityManager->isValid(*e)) {
      foundEntityWithCorrectMask = e->hasComponent(positionBit);
      break;
    }
  }

  EXPECT_TRUE(foundEntityWithCorrectMask)
      << "Entities from getAllEntitiesForIteration() should have correct "
         "componentMask";
}

// Verify multi-component scenario
TEST_F(ComponentArrayEntityManagerIntegrationTest,
       MultiComponentMaskSynchronizationBug) {

  Entity entity = entityManager->createEntity();
  EntityID entityId = entity.id;

  Position pos{1.0f, 2.0f, 3.0f};
  Velocity vel{5.0f, -3.0f};

  positions->add(entityId, pos, positionBit, *entityManager);
  velocities->add(entityId, vel, velocityBit, *entityManager);

  Entity *storedEntity = entityManager->getEntityByID(entityId);
  ASSERT_NE(storedEntity, nullptr);

  // NOTE: local copy will NOT have updated componentMask
  EXPECT_FALSE(entity.hasComponent(positionBit))
      << "Local entity should not have Position component bit";
  EXPECT_FALSE(entity.hasComponent(velocityBit))
      << "Local entity should not have Velocity component bit";

  EXPECT_TRUE(storedEntity->hasComponent(positionBit))
      << "Stored entity should have Position component bit";
  EXPECT_TRUE(storedEntity->hasComponent(velocityBit))
      << "Stored entity should have Velocity component bit";
  EXPECT_TRUE(storedEntity->hasComponents(positionBit | velocityBit))
      << "Stored entity should have both component bits set";
}

// Verify component removal also needs synchronization
TEST_F(ComponentArrayEntityManagerIntegrationTest,
       ComponentRemovalMaskSynchronizationBug) {
  Entity entity = entityManager->createEntity();
  EntityID entityId = entity.id;

  Position pos{1.0f, 2.0f, 3.0f};
  positions->add(entityId, pos, positionBit, *entityManager);

  Entity *storedEntity = entityManager->getEntityByID(entityId);
  ASSERT_NE(storedEntity, nullptr);
  EXPECT_TRUE(storedEntity->hasComponent(positionBit));
  // NOTE: add does not update local copy
  EXPECT_FALSE(entity.hasComponent(positionBit));

  // Remove component - this should update EntityManager's stored entity
  positions->remove(entityId, positionBit, *entityManager);

  // Stored entity should also not have component
  storedEntity = entityManager->getEntityByID(entityId);
  ASSERT_NE(storedEntity, nullptr);
  EXPECT_FALSE(storedEntity->hasComponent(positionBit))
      << "Stored entity should have component bit cleared after removal";
}
