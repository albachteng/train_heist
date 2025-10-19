#include "../../include/EntityManager.hpp"
#include "../include/SystemUtils.hpp"
#include <gtest/gtest.h>

using namespace ECS;

class SystemUtilsSimpleTest : public ::testing::Test {
protected:
    void SetUp() override {
        entityManager = std::make_unique<EntityManager>();
    }

    void TearDown() override {
        entityManager.reset();
    }
    
    // Helper function to create and setup an entity with components
    EntityID createEntityWithComponents(uint64_t componentMask) {
        Entity entity = entityManager->createEntity();
        Entity* entityRef = entityManager->getEntityByID(entity.id);
        entityRef->addComponent(componentMask);
        return entity.id;
    }
    
    std::unique_ptr<EntityManager> entityManager;
};

// Test basic forEachEntity functionality
TEST_F(SystemUtilsSimpleTest, BasicForEachEntity) {
    // Create entities with different components
    createEntityWithComponents(0b001); // Component 0
    createEntityWithComponents(0b010); // Component 1
    createEntityWithComponents(0b011); // Components 0 and 1
    
    // Count entities with component 0
    int count = 0;
    SystemUtils::forEachEntity(*entityManager, 0b001, [&count](const Entity& entity) {
        EXPECT_TRUE(entity.hasComponent(0b001));
        count++;
    });
    
    EXPECT_EQ(count, 2); // First and third entities
}

// Test countEntitiesWithComponents
TEST_F(SystemUtilsSimpleTest, BasicCountEntities) {
    createEntityWithComponents(0b001); // Component 0
    createEntityWithComponents(0b001); // Component 0
    createEntityWithComponents(0b010); // Component 1
    
    EXPECT_EQ(SystemUtils::countEntitiesWithComponents(*entityManager, 0b001), 2);
    EXPECT_EQ(SystemUtils::countEntitiesWithComponents(*entityManager, 0b010), 1);
    EXPECT_EQ(SystemUtils::countEntitiesWithComponents(*entityManager, 0b100), 0);
}

// Test hasEntitiesWithComponents
TEST_F(SystemUtilsSimpleTest, BasicHasEntities) {
    // Initially no entities
    EXPECT_FALSE(SystemUtils::hasEntitiesWithComponents(*entityManager, 0b001));
    
    // Create entity with component 0
    createEntityWithComponents(0b001);
    
    EXPECT_TRUE(SystemUtils::hasEntitiesWithComponents(*entityManager, 0b001));
    EXPECT_FALSE(SystemUtils::hasEntitiesWithComponents(*entityManager, 0b010));
}

// Test findFirstEntityWithComponents
TEST_F(SystemUtilsSimpleTest, BasicFindFirstEntity) {
    EntityID id1 = createEntityWithComponents(0b010); // Component 1 only
    EntityID id2 = createEntityWithComponents(0b001); // Component 0 (first match)
    
    Entity* found = SystemUtils::findFirstEntityWithComponents(*entityManager, 0b001);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->id, id2);
    
    Entity* notFound = SystemUtils::findFirstEntityWithComponents(*entityManager, 0b100);
    EXPECT_EQ(notFound, nullptr);
}

// Test that dead entities are properly skipped
TEST_F(SystemUtilsSimpleTest, SkipsDeadEntities) {
    EntityID id1 = createEntityWithComponents(0b001);
    EntityID id2 = createEntityWithComponents(0b001);
    EntityID id3 = createEntityWithComponents(0b001);
    
    // All should be found initially
    EXPECT_EQ(SystemUtils::countEntitiesWithComponents(*entityManager, 0b001), 3);
    
    // Destroy middle entity
    Entity* entityToDestroy = entityManager->getEntityByID(id2);
    entityManager->destroyEntity(*entityToDestroy);
    
    // Should now find only 2
    EXPECT_EQ(SystemUtils::countEntitiesWithComponents(*entityManager, 0b001), 2);
    
    // Verify which entities are processed
    std::vector<EntityID> processedIds;
    SystemUtils::forEachEntity(*entityManager, 0b001, [&](const Entity& entity) {
        processedIds.push_back(entity.id);
    });
    
    EXPECT_EQ(processedIds.size(), 2);
    EXPECT_TRUE(std::find(processedIds.begin(), processedIds.end(), id1) != processedIds.end());
    EXPECT_TRUE(std::find(processedIds.begin(), processedIds.end(), id2) == processedIds.end()); // Dead
    EXPECT_TRUE(std::find(processedIds.begin(), processedIds.end(), id3) != processedIds.end());
}