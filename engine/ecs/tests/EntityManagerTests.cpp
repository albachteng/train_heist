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

// Test mark dead and reuse approach - dead entities remain in storage but are invalid
TEST_F(EntityManagerTest, MarkDeadAndReuseApproach) {
    EntityManager manager;
    
    // Create several entities
    Entity entity1 = manager.createEntity();
    Entity entity2 = manager.createEntity(); 
    Entity entity3 = manager.createEntity();
    
    size_t initialEntityCount = manager.getActiveEntityCount();
    EXPECT_EQ(initialEntityCount, 3);
    
    // Destroy entity2 (middle entity)
    manager.destroyEntity(entity2);
    
    // Active count should decrease but total storage should remain
    EXPECT_EQ(manager.getActiveEntityCount(), 2);
    EXPECT_EQ(manager.getTotalEntityCount(), 3); // New method: total including dead entities
    
    // entity2 should be marked as dead but still retrievable by ID for validation
    EXPECT_FALSE(manager.isValid(entity2));
    EXPECT_FALSE(manager.isAlive(entity2.id));  // New method: check if entity slot is alive (should be false for dead entity)
    
    // Other entities should remain valid
    EXPECT_TRUE(manager.isValid(entity1));
    EXPECT_TRUE(manager.isValid(entity3));
}

// Test entity reuse from dead pool
TEST_F(EntityManagerTest, ReuseDeadEntitySlots) {
    EntityManager manager;
    
    // Create several entities with different IDs, then destroy some to create reuse pool
    std::vector<Entity> entities;
    for (int i = 0; i < 5; ++i) {
        entities.push_back(manager.createEntity());
    }
    
    // Verify all entities have different IDs
    EXPECT_NE(entities[0].id, entities[1].id);
    EXPECT_NE(entities[1].id, entities[2].id);
    EXPECT_EQ(manager.getActiveEntityCount(), 5);
    EXPECT_EQ(manager.getTotalEntityCount(), 5);
    
    // Destroy entities 1, 2, and 4 to create gaps
    std::vector<EntityID> destroyedIds = {entities[1].id, entities[2].id, entities[4].id};
    manager.destroyEntity(entities[1]);
    manager.destroyEntity(entities[2]);
    manager.destroyEntity(entities[4]);
    
    EXPECT_EQ(manager.getActiveEntityCount(), 2);
    EXPECT_EQ(manager.getTotalEntityCount(), 5); // Storage doesn't shrink
    EXPECT_EQ(manager.getDeadEntityCount(), 3); // 3 dead entity IDs available for reuse
    
    // Create new entities - should reuse dead slots
    std::vector<Entity> newEntities;
    for (int i = 0; i < 2; ++i) {
        Entity entity = manager.createEntity();
        newEntities.push_back(entity);
        
        // Should reuse one of the destroyed IDs but with incremented generation
        EXPECT_TRUE(std::find(destroyedIds.begin(), destroyedIds.end(), entity.id) != destroyedIds.end());
        EXPECT_GT(entity.generation, 0); // Should have incremented generation
    }
    
    EXPECT_EQ(manager.getActiveEntityCount(), 4); // 2 original + 2 new
    EXPECT_EQ(manager.getDeadEntityCount(), 1); // 1 dead entity ID still available for reuse
    
    // All new entities should be valid
    for (const Entity& entity : newEntities) {
        EXPECT_TRUE(manager.isValid(entity));
    }
    
    // Original surviving entities should still be valid
    EXPECT_TRUE(manager.isValid(entities[0]));
    EXPECT_TRUE(manager.isValid(entities[3]));
}

// Test entity lookup by ID returns current generation
TEST_F(EntityManagerTest, EntityLookupReturnsCurrentGeneration) {
    EntityManager manager;
    
    // Create entity
    Entity entity1 = manager.createEntity();
    EntityID reusedId = entity1.id;
    uint32_t firstGeneration = entity1.generation;
    
    // Should be able to lookup current entity by ID
    Entity* retrieved1 = manager.getEntityByID(reusedId); // New method: get current entity by ID
    ASSERT_NE(retrieved1, nullptr);
    EXPECT_EQ(retrieved1->id, reusedId);
    EXPECT_EQ(retrieved1->generation, firstGeneration);
    
    // Destroy entity
    manager.destroyEntity(entity1);
    
    // Lookup by ID should return nullptr (entity is dead)
    Entity* retrievedDead = manager.getEntityByID(reusedId);
    EXPECT_EQ(retrievedDead, nullptr);
    
    // Create new entity (should reuse the ID)
    Entity entity2 = manager.createEntity();
    EXPECT_EQ(entity2.id, reusedId);
    EXPECT_EQ(entity2.generation, firstGeneration + 1);
    
    // Lookup should now return the new generation
    Entity* retrieved2 = manager.getEntityByID(reusedId);
    ASSERT_NE(retrieved2, nullptr);
    EXPECT_EQ(retrieved2->id, reusedId);
    EXPECT_EQ(retrieved2->generation, firstGeneration + 1);
}

// Test that dead entities can be iterated over and skipped by systems
TEST_F(EntityManagerTest, DeadEntityIterationPattern) {
    EntityManager manager;
    
    // Create entities with gaps (some dead, some alive)
    Entity entity1 = manager.createEntity();
    Entity entity2 = manager.createEntity();
    Entity entity3 = manager.createEntity();
    Entity entity4 = manager.createEntity();
    
    // Destroy middle entities
    manager.destroyEntity(entity2);
    manager.destroyEntity(entity3);
    
    // Get all entities for system iteration (including dead ones)
    std::vector<Entity> allEntities = manager.getAllEntitiesForIteration(); // New method
    EXPECT_EQ(allEntities.size(), 4); // Should include dead entities
    
    // Count valid entities during iteration (what systems would do)
    int validCount = 0;
    for (const Entity& entity : allEntities) {
        if (manager.isValid(entity)) {
            validCount++;
        }
    }
    EXPECT_EQ(validCount, 2); // Only entity1 and entity4 should be valid
    
    // Test that specific entities have expected validity
    EXPECT_TRUE(manager.isValid(entity1));
    EXPECT_FALSE(manager.isValid(entity2)); // Dead
    EXPECT_FALSE(manager.isValid(entity3)); // Dead  
    EXPECT_TRUE(manager.isValid(entity4));
}

// Test entity storage growth and compaction behavior
TEST_F(EntityManagerTest, EntityStorageGrowthBehavior) {
    EntityManager manager;
    
    // Create many entities to test growth
    std::vector<Entity> entities;
    const size_t numEntities = 100;
    
    for (size_t i = 0; i < numEntities; ++i) {
        entities.push_back(manager.createEntity());
    }
    
    EXPECT_EQ(manager.getActiveEntityCount(), numEntities);
    EXPECT_EQ(manager.getTotalEntityCount(), numEntities);
    
    // Destroy half the entities
    for (size_t i = 0; i < numEntities / 2; ++i) {
        manager.destroyEntity(entities[i]);
    }
    
    EXPECT_EQ(manager.getActiveEntityCount(), numEntities / 2);
    EXPECT_EQ(manager.getTotalEntityCount(), numEntities); // Storage doesn't shrink
    EXPECT_EQ(manager.getDeadEntityCount(), numEntities / 2);
    
    // Create new entities - should reuse dead slots
    for (size_t i = 0; i < 10; ++i) {
        Entity newEntity = manager.createEntity();
        EXPECT_TRUE(manager.isValid(newEntity));
    }
    
    EXPECT_EQ(manager.getActiveEntityCount(), (numEntities / 2) + 10);
    EXPECT_EQ(manager.getDeadEntityCount(), (numEntities / 2) - 10);
}

// Test that generation increment doesn't accidentally invalidate unrelated entities
TEST_F(EntityManagerTest, GenerationIncrementDoesNotInvalidateUnrelatedEntities) {
    EntityManager manager;
    
    // Create three entities with consecutive IDs
    Entity entity1 = manager.createEntity();
    Entity entity2 = manager.createEntity();
    Entity entity3 = manager.createEntity();
    
    EntityID id1 = entity1.id;
    EntityID id2 = entity2.id;
    EntityID id3 = entity3.id;
    
    // Verify all entities are valid
    EXPECT_TRUE(manager.isValid(entity1));
    EXPECT_TRUE(manager.isValid(entity2));
    EXPECT_TRUE(manager.isValid(entity3));
    EXPECT_EQ(entity1.generation, 0);
    EXPECT_EQ(entity2.generation, 0);
    EXPECT_EQ(entity3.generation, 0);
    
    // Destroy only the middle entity
    manager.destroyEntity(entity2);
    
    // entity1 and entity3 should still be valid with same generations
    EXPECT_TRUE(manager.isValid(entity1));
    EXPECT_FALSE(manager.isValid(entity2)); // Destroyed
    EXPECT_TRUE(manager.isValid(entity3));
    
    // Create new entity - should reuse entity2's ID but with incremented generation
    Entity newEntity = manager.createEntity();
    EXPECT_EQ(newEntity.id, id2); // Should reuse ID2
    EXPECT_EQ(newEntity.generation, 1); // Should have incremented generation
    
    // CRITICAL TEST: entity1 and entity3 should still be valid with original generations
    EXPECT_TRUE(manager.isValid(entity1));
    EXPECT_TRUE(manager.isValid(entity3));
    EXPECT_EQ(entity1.generation, 0); // Should not have changed
    EXPECT_EQ(entity3.generation, 0); // Should not have changed
    
    // Old entity2 reference should still be invalid
    EXPECT_FALSE(manager.isValid(entity2));
    
    // New entity should be valid
    EXPECT_TRUE(manager.isValid(newEntity));
    
    // Verify we can still get entities by their original IDs
    Entity* retrieved1 = manager.getEntityByID(id1);
    Entity* retrieved3 = manager.getEntityByID(id3);
    ASSERT_NE(retrieved1, nullptr);
    ASSERT_NE(retrieved3, nullptr);
    EXPECT_EQ(retrieved1->generation, 0);
    EXPECT_EQ(retrieved3->generation, 0);
    
    // Verify the reused slot has the new generation
    Entity* retrievedReused = manager.getEntityByID(id2);
    ASSERT_NE(retrievedReused, nullptr);
    EXPECT_EQ(retrievedReused->generation, 1);
    EXPECT_EQ(retrievedReused->id, id2);
}