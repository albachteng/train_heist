#include "../../include/EntityManager.hpp"
#include "../include/ISystem.hpp"
#include "../include/SystemManager.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <memory>

using namespace ECS;

// Static execution tracking for priority order testing
static std::vector<int> executionOrder;
static int executionCounter = 0;

// Mock system for testing
class MockSystem : public ISystem {
private:
    uint64_t requiredComponents;
    int priority;
    mutable int updateCallCount = 0;
    mutable float lastDeltaTime = 0.0f;
    mutable EntityManager* lastEntityManager = nullptr;
    mutable std::vector<EntityID> processedEntityIDs;
    bool shouldUpdateFlag = true;
    int systemId; // Unique identifier for this system
    
public:
    MockSystem(uint64_t components, int prio = 1000, int id = 0) 
        : requiredComponents(components), priority(prio), systemId(id) {}
    
    void update(float deltaTime, EntityManager& entityManager) override {
        updateCallCount++;
        lastDeltaTime = deltaTime;
        lastEntityManager = &entityManager;
        
        // Record execution order for priority testing
        executionOrder.push_back(systemId);
        
        // Process entities with required components for filtering testing
        processedEntityIDs.clear();
        std::vector<Entity> entities = entityManager.getAllEntitiesForIteration();
        for (const Entity& entity : entities) {
            if (entityManager.isValid(entity) && 
                (entity.componentMask & requiredComponents) == requiredComponents) {
                processedEntityIDs.push_back(entity.id);
            }
        }
    }
    
    uint64_t getRequiredComponents() const override {
        return requiredComponents;
    }
    
    int getPriority() const override {
        return priority;
    }
    
    bool shouldUpdate(float deltaTime) const override {
        (void)deltaTime;
        return shouldUpdateFlag;
    }
    
    // Test accessors
    int getUpdateCallCount() const { return updateCallCount; }
    float getLastDeltaTime() const { return lastDeltaTime; }
    EntityManager* getLastEntityManager() const { return lastEntityManager; }
    const std::vector<EntityID>& getProcessedEntityIDs() const { return processedEntityIDs; }
    int getSystemId() const { return systemId; }
    void setShouldUpdate(bool flag) { shouldUpdateFlag = flag; }
};

class SystemManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        entityManager = std::make_unique<EntityManager>();
        systemManager = std::make_unique<SystemManager>(); // No longer takes EntityManager
        
        // Clear static execution tracking
        executionOrder.clear();
        executionCounter = 0;
    }

    void TearDown() override {
        systemManager.reset();
        entityManager.reset();
        
        // Clear static execution tracking
        executionOrder.clear();
        executionCounter = 0;
    }
    
    std::unique_ptr<EntityManager> entityManager;
    std::unique_ptr<SystemManager> systemManager;
};

// Test system registration
TEST_F(SystemManagerTest, RegisterSystem) {
    auto mockSystem = std::make_unique<MockSystem>(0b001); // Requires component bit 0
    MockSystem* systemPtr = mockSystem.get();
    
    systemManager->registerSystem(std::move(mockSystem));
    
    // System should be registered but not updated yet
    EXPECT_EQ(systemPtr->getUpdateCallCount(), 0);
}

// Test system update execution
TEST_F(SystemManagerTest, UpdateSystems) {
    auto mockSystem1 = std::make_unique<MockSystem>(0b001);
    auto mockSystem2 = std::make_unique<MockSystem>(0b010);
    
    MockSystem* system1Ptr = mockSystem1.get();
    MockSystem* system2Ptr = mockSystem2.get();
    
    systemManager->registerSystem(std::move(mockSystem1));
    systemManager->registerSystem(std::move(mockSystem2));
    
    float deltaTime = 0.016f; // 60 FPS
    systemManager->updateAll(deltaTime, *entityManager); // Now pass EntityManager
    
    // Both systems should have been updated with correct parameters
    EXPECT_EQ(system1Ptr->getUpdateCallCount(), 1);
    EXPECT_EQ(system2Ptr->getUpdateCallCount(), 1);
    EXPECT_FLOAT_EQ(system1Ptr->getLastDeltaTime(), deltaTime);
    EXPECT_FLOAT_EQ(system2Ptr->getLastDeltaTime(), deltaTime);
    EXPECT_EQ(system1Ptr->getLastEntityManager(), entityManager.get());
    EXPECT_EQ(system2Ptr->getLastEntityManager(), entityManager.get());
}

// Test system priority ordering
TEST_F(SystemManagerTest, SystemPriorityOrdering) {
    // Create systems with different priorities and unique IDs
    auto highPrioritySystem = std::make_unique<MockSystem>(0b001, 100, 1);   // Lower value = higher priority, ID=1
    auto lowPrioritySystem = std::make_unique<MockSystem>(0b010, 2000, 3);   // Higher value = lower priority, ID=3
    auto mediumPrioritySystem = std::make_unique<MockSystem>(0b100, 1000, 2); // Default priority, ID=2
    
    MockSystem* highPtr = highPrioritySystem.get();
    MockSystem* mediumPtr = mediumPrioritySystem.get();
    MockSystem* lowPtr = lowPrioritySystem.get();
    
    // Register in random order to test that registration order doesn't affect execution order
    systemManager->registerSystem(std::move(lowPrioritySystem));
    systemManager->registerSystem(std::move(highPrioritySystem));
    systemManager->registerSystem(std::move(mediumPrioritySystem));
    
    // Create entities to trigger system updates
    Entity entity1 = entityManager->createEntity();
    entity1.addComponent(0b001); // Matches high priority system
    
    Entity entity2 = entityManager->createEntity(); 
    entity2.addComponent(0b010); // Matches low priority system
    
    Entity entity3 = entityManager->createEntity();
    entity3.addComponent(0b100); // Matches medium priority system
    
    systemManager->updateAll(0.016f, *entityManager);
    
    // All systems should have been called
    EXPECT_EQ(highPtr->getUpdateCallCount(), 1);
    EXPECT_EQ(mediumPtr->getUpdateCallCount(), 1);
    EXPECT_EQ(lowPtr->getUpdateCallCount(), 1);
    
    // Verify execution order: High (ID=1) → Medium (ID=2) → Low (ID=3)
    ASSERT_EQ(executionOrder.size(), 3);
    EXPECT_EQ(executionOrder[0], 1); // High priority system (priority 100)
    EXPECT_EQ(executionOrder[1], 2); // Medium priority system (priority 1000) 
    EXPECT_EQ(executionOrder[2], 3); // Low priority system (priority 2000)
}

// Test system conditional updates
TEST_F(SystemManagerTest, ConditionalSystemUpdates) {
    auto mockSystem = std::make_unique<MockSystem>(0b001);
    MockSystem* systemPtr = mockSystem.get();
    
    systemManager->registerSystem(std::move(mockSystem));
    
    // First update - should update normally
    systemManager->updateAll(0.016f, *entityManager);
    EXPECT_EQ(systemPtr->getUpdateCallCount(), 1);
    
    // Set system to not update
    systemPtr->setShouldUpdate(false);
    systemManager->updateAll(0.016f, *entityManager);
    EXPECT_EQ(systemPtr->getUpdateCallCount(), 1); // Should still be 1, not 2
    
    // Re-enable updates
    systemPtr->setShouldUpdate(true);
    systemManager->updateAll(0.016f, *entityManager);
    EXPECT_EQ(systemPtr->getUpdateCallCount(), 2); // Now should be 2
}

// Test multiple update cycles
TEST_F(SystemManagerTest, MultipleUpdateCycles) {
    auto mockSystem = std::make_unique<MockSystem>(0b001);
    MockSystem* systemPtr = mockSystem.get();
    
    systemManager->registerSystem(std::move(mockSystem));
    
    // Multiple update cycles
    for (int i = 1; i <= 5; ++i) {
        systemManager->updateAll(0.016f, *entityManager);
        EXPECT_EQ(systemPtr->getUpdateCallCount(), i);
    }
}

// Test clearing all systems
TEST_F(SystemManagerTest, ClearSystems) {
    auto mockSystem1 = std::make_unique<MockSystem>(0b001);
    auto mockSystem2 = std::make_unique<MockSystem>(0b010);
    
    systemManager->registerSystem(std::move(mockSystem1));
    systemManager->registerSystem(std::move(mockSystem2));
    
    systemManager->clearSystems();
    
    // After clearing, updateAll should not crash and no systems should run
    systemManager->updateAll(0.016f, *entityManager);
    
    // Test passes if no crash occurs
    SUCCEED();
}

// Test system count tracking
TEST_F(SystemManagerTest, SystemCount) {
    EXPECT_EQ(systemManager->getSystemCount(), 0);
    
    systemManager->registerSystem(std::make_unique<MockSystem>(0b001));
    EXPECT_EQ(systemManager->getSystemCount(), 1);
    
    systemManager->registerSystem(std::make_unique<MockSystem>(0b010));
    EXPECT_EQ(systemManager->getSystemCount(), 2);
    
    systemManager->clearSystems();
    EXPECT_EQ(systemManager->getSystemCount(), 0);
}

// Test entity filtering (systems only process entities with required components)
TEST_F(SystemManagerTest, EntityComponentFiltering) {
    auto system = std::make_unique<MockSystem>(0b011); // Requires components 0 and 1
    MockSystem* systemPtr = system.get();
    
    systemManager->registerSystem(std::move(system));
    
    // Create entities with different component combinations
    Entity entity1 = entityManager->createEntity();
    EntityID entity1ID = entity1.id;
    Entity* entity1Ref = entityManager->getEntityByID(entity1ID);
    entity1Ref->addComponent(0b001); // Only component 0 - should not match
    
    Entity entity2 = entityManager->createEntity();
    EntityID entity2ID = entity2.id;
    Entity* entity2Ref = entityManager->getEntityByID(entity2ID);
    entity2Ref->addComponent(0b010); // Only component 1 - should not match
    
    Entity entity3 = entityManager->createEntity();
    EntityID entity3ID = entity3.id;
    Entity* entity3Ref = entityManager->getEntityByID(entity3ID);
    entity3Ref->addComponent(0b011); // Both components 0 and 1 - should match
    
    Entity entity4 = entityManager->createEntity();
    EntityID entity4ID = entity4.id;
    Entity* entity4Ref = entityManager->getEntityByID(entity4ID);
    entity4Ref->addComponent(0b111); // Components 0, 1, and 2 - should match (has required components)
    
    systemManager->updateAll(0.016f, *entityManager);
    
    // System should have been updated
    EXPECT_EQ(systemPtr->getUpdateCallCount(), 1);
    
    // Verify that only entities with required components were processed
    const std::vector<EntityID>& processedIDs = systemPtr->getProcessedEntityIDs();
    
    // Should process exactly 2 entities (entity3 and entity4)
    EXPECT_EQ(processedIDs.size(), 2);
    
    // Check that the correct entities were processed
    EXPECT_TRUE(std::find(processedIDs.begin(), processedIDs.end(), entity3ID) != processedIDs.end());
    EXPECT_TRUE(std::find(processedIDs.begin(), processedIDs.end(), entity4ID) != processedIDs.end());
    
    // Check that incorrect entities were NOT processed
    EXPECT_TRUE(std::find(processedIDs.begin(), processedIDs.end(), entity1ID) == processedIDs.end());
    EXPECT_TRUE(std::find(processedIDs.begin(), processedIDs.end(), entity2ID) == processedIDs.end());
}

// Test that systems can be tested independently with different EntityManagers
TEST_F(SystemManagerTest, SystemIndependentTesting) {
    auto system = std::make_unique<MockSystem>(0b001);
    MockSystem* systemPtr = system.get();
    
    systemManager->registerSystem(std::move(system));
    
    // Create a separate EntityManager for testing
    EntityManager testEntityManager;
    Entity testEntity = testEntityManager.createEntity();
    Entity* testEntityRef = testEntityManager.getEntityByID(testEntity.id);
    testEntityRef->addComponent(0b001);
    
    float testDeltaTime = 0.1f;
    systemManager->updateAll(testDeltaTime, testEntityManager);
    
    // System should have been called with the test EntityManager
    EXPECT_EQ(systemPtr->getUpdateCallCount(), 1);
    EXPECT_FLOAT_EQ(systemPtr->getLastDeltaTime(), testDeltaTime);
    EXPECT_EQ(systemPtr->getLastEntityManager(), &testEntityManager);
    EXPECT_NE(systemPtr->getLastEntityManager(), entityManager.get()); // Not the original
}

// Test that the same system can work with multiple EntityManagers in sequence
TEST_F(SystemManagerTest, MultipleEntityManagerSupport) {
    auto system = std::make_unique<MockSystem>(0b001);
    MockSystem* systemPtr = system.get();
    
    systemManager->registerSystem(std::move(system));
    
    // Update with first EntityManager
    systemManager->updateAll(0.016f, *entityManager);
    EXPECT_EQ(systemPtr->getLastEntityManager(), entityManager.get());
    EXPECT_EQ(systemPtr->getUpdateCallCount(), 1);
    
    // Create and update with second EntityManager
    EntityManager secondEntityManager;
    systemManager->updateAll(0.032f, secondEntityManager);
    EXPECT_EQ(systemPtr->getLastEntityManager(), &secondEntityManager);
    EXPECT_EQ(systemPtr->getUpdateCallCount(), 2); // Called twice
    EXPECT_FLOAT_EQ(systemPtr->getLastDeltaTime(), 0.032f); // Last delta time
}