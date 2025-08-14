#include "../include/EntityManager.hpp"
#include <gtest/gtest.h>

using namespace ECS;

class EntityManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean state for each test
    }

    void TearDown() override {
        // Cleanup after each test
    }
};

// Test entity creation
TEST_F(EntityManagerTest, CreateEntity) {
    EntityManager manager;
    
    Entity entity = manager.createEntity();
    
    EXPECT_NE(entity.id, INVALID_ENTITY);
    EXPECT_EQ(entity.generation, 0);
    EXPECT_EQ(entity.componentMask, 0);
    EXPECT_TRUE(entity.isValid());
}

// Test multiple entity creation with unique IDs
TEST_F(EntityManagerTest, CreateMultipleEntities) {
    EntityManager manager;
    
    Entity entity1 = manager.createEntity();
    Entity entity2 = manager.createEntity();
    Entity entity3 = manager.createEntity();
    
    EXPECT_NE(entity1.id, entity2.id);
    EXPECT_NE(entity2.id, entity3.id);
    EXPECT_NE(entity1.id, entity3.id);
    
    EXPECT_TRUE(manager.isValid(entity1));
    EXPECT_TRUE(manager.isValid(entity2));
    EXPECT_TRUE(manager.isValid(entity3));
}

// Test entity destruction
TEST_F(EntityManagerTest, DestroyEntity) {
    EntityManager manager;
    
    Entity entity = manager.createEntity();
    EXPECT_TRUE(manager.isValid(entity));
    
    manager.destroyEntity(entity);
    EXPECT_FALSE(manager.isValid(entity));
}

// Test generation counter increments on reuse
TEST_F(EntityManagerTest, GenerationIncrementsOnReuse) {
    EntityManager manager;
    
    Entity entity1 = manager.createEntity();
    EntityID firstId = entity1.id;
    uint32_t firstGeneration = entity1.generation;
    
    manager.destroyEntity(entity1);
    EXPECT_FALSE(manager.isValid(entity1));
    
    Entity entity2 = manager.createEntity();
    
    // Should reuse the same ID but increment generation
    EXPECT_EQ(entity2.id, firstId);
    EXPECT_EQ(entity2.generation, firstGeneration + 1);
    EXPECT_TRUE(manager.isValid(entity2));
    EXPECT_FALSE(manager.isValid(entity1)); // Old generation should be invalid
}

// Test entity handle creation
TEST_F(EntityManagerTest, CreateEntityHandle) {
    EntityManager manager;
    
    Entity entity = manager.createEntity();
    EntityHandle handle = manager.createHandle(entity);
    
    EXPECT_EQ(handle.id, entity.id);
    EXPECT_EQ(handle.generation, entity.generation);
    EXPECT_TRUE(handle.isValid());
}

// Test handle validation
TEST_F(EntityManagerTest, ValidateHandle) {
    EntityManager manager;
    
    Entity entity = manager.createEntity();
    EntityHandle handle = manager.createHandle(entity);
    
    EXPECT_TRUE(manager.isValid(handle));
    
    manager.destroyEntity(entity);
    EXPECT_FALSE(manager.isValid(handle)); // Handle should be invalid after entity destruction
}

// Test getting entity from handle
TEST_F(EntityManagerTest, GetEntityFromHandle) {
    EntityManager manager;
    
    Entity entity = manager.createEntity();
    EntityHandle handle = manager.createHandle(entity);
    
    Entity* retrieved = manager.getEntity(handle);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->id, entity.id);
    EXPECT_EQ(retrieved->generation, entity.generation);
    
    manager.destroyEntity(entity);
    Entity* invalidRetrieved = manager.getEntity(handle);
    EXPECT_EQ(invalidRetrieved, nullptr); // Should return null for invalid handle
}

// Test generation isolation - new generations don't inherit old component data
TEST_F(EntityManagerTest, GenerationIsolationComponentMask) {
    EntityManager manager;
    
    // Create entity and modify its component mask
    Entity entity1 = manager.createEntity();
    EntityID reusedId = entity1.id;
    uint32_t firstGeneration = entity1.generation;
    
    // Simulate adding components by setting bitmask bits
    uint64_t componentBits = (1ULL << 0) | (1ULL << 5) | (1ULL << 12); // Position, Health, Weapon
    entity1.addComponent(componentBits);
    EXPECT_EQ(entity1.componentMask, componentBits);
    
    // Destroy the entity
    manager.destroyEntity(entity1);
    EXPECT_FALSE(manager.isValid(entity1));
    
    // Create a new entity (should reuse the same ID but increment generation)
    Entity entity2 = manager.createEntity();
    
    // Verify ID reuse and generation increment
    EXPECT_EQ(entity2.id, reusedId);
    EXPECT_EQ(entity2.generation, firstGeneration + 1);
    
    // Critical test: new entity should have clean component mask
    EXPECT_EQ(entity2.componentMask, 0ULL) << "New generation should start with empty component mask";
    EXPECT_FALSE(entity2.hasComponent(1ULL << 0)) << "New generation should not have Position component";
    EXPECT_FALSE(entity2.hasComponent(1ULL << 5)) << "New generation should not have Health component";
    EXPECT_FALSE(entity2.hasComponent(1ULL << 12)) << "New generation should not have Weapon component";
    EXPECT_FALSE(entity2.hasComponents(componentBits)) << "New generation should not have any old components";
    
    // Verify old entity handle is invalid after destruction
    EntityHandle oldHandle(entity1.id, entity1.generation);
    EXPECT_FALSE(manager.isValid(oldHandle)) << "Old generation handle should be invalid";
    
    // Verify new entity handle is valid
    EntityHandle newHandle = manager.createHandle(entity2);
    EXPECT_TRUE(manager.isValid(newHandle)) << "New generation handle should be valid";
}