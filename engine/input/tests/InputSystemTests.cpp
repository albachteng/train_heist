#include "../include/InputSystem.hpp"
#include "MockInputManager.hpp"
#include "../../ecs/include/EntityManager.hpp"
#include <gtest/gtest.h>
#include <memory>

class InputSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockInput = std::make_unique<ECS::MockInputManager>();
        inputSystem = std::make_unique<ECS::InputSystem>(mockInput.get());
        entityManager = std::make_unique<ECS::EntityManager>();
    }
    
    void TearDown() override {
        inputSystem.reset();
        mockInput.reset();
        entityManager.reset();
    }
    
    ECS::Entity createInputEntity() {
        auto entity = entityManager->createEntity();
        entityManager->addComponent<ECS::InputHandler>(entity, {});
        entityManager->addComponent<ECS::InputState>(entity, {});
        entityManager->addComponent<ECS::Controllable>(entity, {});
        return entity;
    }
    
    std::unique_ptr<ECS::MockInputManager> mockInput;
    std::unique_ptr<ECS::InputSystem> inputSystem;
    std::unique_ptr<ECS::EntityManager> entityManager;
};

// Test basic system properties
TEST_F(InputSystemTest, SystemProperties) {
    EXPECT_EQ(inputSystem->getPriority(), 10); // High priority
    EXPECT_TRUE(inputSystem->shouldUpdate(0.016f)); // Always update
    
    // Should require InputHandler, InputState, and Controllable components
    auto requiredMask = inputSystem->getRequiredComponents();
    EXPECT_NE(requiredMask, 0);
}

// Test system processes input correctly
TEST_F(InputSystemTest, BasicInputProcessing) {
    auto entity = createInputEntity();
    
    // Simulate arrow key press
    mockInput->pressKey(ECS::Keys::Up);
    mockInput->pressKey(ECS::Keys::Left);
    
    inputSystem->update(0.016f, *entityManager);
    
    auto* state = entityManager->getComponent<ECS::InputState>(entity);
    ASSERT_NE(state, nullptr);
    
    EXPECT_TRUE(state->moveUp);
    EXPECT_FALSE(state->moveDown);
    EXPECT_TRUE(state->moveLeft);
    EXPECT_FALSE(state->moveRight);
    EXPECT_TRUE(state->upPressed);
    EXPECT_TRUE(state->leftPressed);
}

// Test action input processing
TEST_F(InputSystemTest, ActionInputProcessing) {
    auto entity = createInputEntity();
    
    // Simulate action key press
    mockInput->pressKey(ECS::Keys::Space);
    mockInput->pressKey(ECS::Keys::Escape);
    
    inputSystem->update(0.016f, *entityManager);
    
    auto* state = entityManager->getComponent<ECS::InputState>(entity);
    ASSERT_NE(state, nullptr);
    
    EXPECT_TRUE(state->action);
    EXPECT_TRUE(state->cancel);
    EXPECT_TRUE(state->actionPressed);
    EXPECT_TRUE(state->cancelPressed);
}

// Test custom key mappings
TEST_F(InputSystemTest, CustomKeyMappings) {
    auto entity = createInputEntity();
    
    // Change key mappings
    auto* handler = entityManager->getComponent<ECS::InputHandler>(entity);
    ASSERT_NE(handler, nullptr);
    handler->upKey = ECS::Keys::W;
    handler->leftKey = ECS::Keys::A;
    handler->actionKey = ECS::Keys::E;
    
    // Press the new keys
    mockInput->pressKey(ECS::Keys::W);
    mockInput->pressKey(ECS::Keys::A);
    mockInput->pressKey(ECS::Keys::E);
    
    inputSystem->update(0.016f, *entityManager);
    
    auto* state = entityManager->getComponent<ECS::InputState>(entity);
    ASSERT_NE(state, nullptr);
    
    EXPECT_TRUE(state->moveUp);
    EXPECT_TRUE(state->moveLeft);
    EXPECT_TRUE(state->action);
    EXPECT_TRUE(state->upPressed);
    EXPECT_TRUE(state->leftPressed);
    EXPECT_TRUE(state->actionPressed);
}

// Test input blocking
TEST_F(InputSystemTest, InputBlocking) {
    auto entity = createInputEntity();
    
    auto* handler = entityManager->getComponent<ECS::InputHandler>(entity);
    ASSERT_NE(handler, nullptr);
    
    // Block movement input
    handler->blockMovement = true;
    
    mockInput->pressKey(ECS::Keys::Up);
    mockInput->pressKey(ECS::Keys::Space); // Action should still work
    
    inputSystem->update(0.016f, *entityManager);
    
    auto* state = entityManager->getComponent<ECS::InputState>(entity);
    ASSERT_NE(state, nullptr);
    
    EXPECT_FALSE(state->moveUp); // Movement blocked
    EXPECT_FALSE(state->upPressed);
    EXPECT_TRUE(state->action); // Action not blocked
    EXPECT_TRUE(state->actionPressed);
}

// Test action blocking
TEST_F(InputSystemTest, ActionBlocking) {
    auto entity = createInputEntity();
    
    auto* handler = entityManager->getComponent<ECS::InputHandler>(entity);
    ASSERT_NE(handler, nullptr);
    
    // Block action input
    handler->blockActions = true;
    
    mockInput->pressKey(ECS::Keys::Up); // Movement should still work
    mockInput->pressKey(ECS::Keys::Space);
    
    inputSystem->update(0.016f, *entityManager);
    
    auto* state = entityManager->getComponent<ECS::InputState>(entity);
    ASSERT_NE(state, nullptr);
    
    EXPECT_TRUE(state->moveUp); // Movement not blocked
    EXPECT_TRUE(state->upPressed);
    EXPECT_FALSE(state->action); // Action blocked
    EXPECT_FALSE(state->actionPressed);
}

// Test input acceptance toggle
TEST_F(InputSystemTest, InputAcceptance) {
    auto entity = createInputEntity();
    
    auto* handler = entityManager->getComponent<ECS::InputHandler>(entity);
    ASSERT_NE(handler, nullptr);
    
    // Disable input acceptance
    handler->acceptsInput = false;
    
    mockInput->pressKey(ECS::Keys::Up);
    mockInput->pressKey(ECS::Keys::Space);
    
    inputSystem->update(0.016f, *entityManager);
    
    auto* state = entityManager->getComponent<ECS::InputState>(entity);
    ASSERT_NE(state, nullptr);
    
    // All input should be blocked
    EXPECT_FALSE(state->moveUp);
    EXPECT_FALSE(state->action);
    EXPECT_FALSE(state->upPressed);
    EXPECT_FALSE(state->actionPressed);
}

// Test controllable entity toggle
TEST_F(InputSystemTest, ControllableToggle) {
    auto entity = createInputEntity();
    
    auto* controllable = entityManager->getComponent<ECS::Controllable>(entity);
    ASSERT_NE(controllable, nullptr);
    
    // Disable controllable
    controllable->enabled = false;
    
    mockInput->pressKey(ECS::Keys::Up);
    
    inputSystem->update(0.016f, *entityManager);
    
    auto* state = entityManager->getComponent<ECS::InputState>(entity);
    ASSERT_NE(state, nullptr);
    
    // Input should not be processed for disabled entities
    EXPECT_FALSE(state->moveUp);
    EXPECT_FALSE(state->upPressed);
}

// Test multiple entities
TEST_F(InputSystemTest, MultipleEntities) {
    auto entity1 = createInputEntity();
    auto entity2 = createInputEntity();
    
    // Different key mappings for each entity
    auto* handler1 = entityManager->getComponent<ECS::InputHandler>(entity1);
    auto* handler2 = entityManager->getComponent<ECS::InputHandler>(entity2);
    ASSERT_NE(handler1, nullptr);
    ASSERT_NE(handler2, nullptr);
    
    handler2->upKey = ECS::Keys::W;
    handler2->actionKey = ECS::Keys::E;
    
    // Press keys for both entities
    mockInput->pressKey(ECS::Keys::Up);    // Entity 1
    mockInput->pressKey(ECS::Keys::W);     // Entity 2
    mockInput->pressKey(ECS::Keys::Space); // Entity 1
    mockInput->pressKey(ECS::Keys::E);     // Entity 2
    
    inputSystem->update(0.016f, *entityManager);
    
    auto* state1 = entityManager->getComponent<ECS::InputState>(entity1);
    auto* state2 = entityManager->getComponent<ECS::InputState>(entity2);
    ASSERT_NE(state1, nullptr);
    ASSERT_NE(state2, nullptr);
    
    // Both entities should respond to their respective keys
    EXPECT_TRUE(state1->moveUp);
    EXPECT_TRUE(state1->action);
    EXPECT_TRUE(state2->moveUp);
    EXPECT_TRUE(state2->action);
}

// Test key release detection
TEST_F(InputSystemTest, KeyReleaseDetection) {
    auto entity = createInputEntity();
    
    // Press and release a key
    mockInput->pressKey(ECS::Keys::Up);
    inputSystem->update(0.016f, *entityManager);
    
    auto* state = entityManager->getComponent<ECS::InputState>(entity);
    ASSERT_NE(state, nullptr);
    
    EXPECT_TRUE(state->moveUp);
    EXPECT_TRUE(state->upPressed);
    
    // Release the key
    mockInput->releaseKey(ECS::Keys::Up);
    inputSystem->update(0.016f, *entityManager);
    
    EXPECT_FALSE(state->moveUp);
    EXPECT_FALSE(state->upPressed); // Press events are cleared after one frame
}

// Test input manager update is called
TEST_F(InputSystemTest, InputManagerUpdate) {
    auto entity = createInputEntity();
    
    // Set up a key press
    mockInput->pressKey(ECS::Keys::Up);
    
    // First frame - should see the press
    inputSystem->update(0.016f, *entityManager);
    auto* state = entityManager->getComponent<ECS::InputState>(entity);
    EXPECT_TRUE(state->upPressed);
    
    // Second frame - press event should be cleared by input manager update
    inputSystem->update(0.016f, *entityManager);
    EXPECT_FALSE(state->upPressed); // Press events only last one frame
    EXPECT_TRUE(state->moveUp); // But held state remains
}

// Test system handles entities without all required components
TEST_F(InputSystemTest, MissingComponents) {
    // Create entity with only some components
    auto entity = entityManager->createEntity();
    entityManager->addComponent<ECS::InputHandler>(entity, {});
    // Missing InputState and Controllable
    
    // This should not crash
    EXPECT_NO_THROW(inputSystem->update(0.016f, *entityManager));
}

// Test invalid input manager handling
TEST_F(InputSystemTest, InvalidInputManager) {
    EXPECT_THROW(ECS::InputSystem(nullptr), std::invalid_argument);
}

// Test player ID support
TEST_F(InputSystemTest, PlayerIdSupport) {
    auto entity1 = createInputEntity();
    auto entity2 = createInputEntity();
    
    auto* controllable1 = entityManager->getComponent<ECS::Controllable>(entity1);
    auto* controllable2 = entityManager->getComponent<ECS::Controllable>(entity2);
    ASSERT_NE(controllable1, nullptr);
    ASSERT_NE(controllable2, nullptr);
    
    controllable1->playerId = 0;
    controllable2->playerId = 1;
    
    mockInput->pressKey(ECS::Keys::Up);
    inputSystem->update(0.016f, *entityManager);
    
    // Both should respond (multiplayer functionality would be handled at game level)
    auto* state1 = entityManager->getComponent<ECS::InputState>(entity1);
    auto* state2 = entityManager->getComponent<ECS::InputState>(entity2);
    EXPECT_TRUE(state1->moveUp);
    EXPECT_TRUE(state2->moveUp);
}

// Test all movement directions
TEST_F(InputSystemTest, AllMovementDirections) {
    auto entity = createInputEntity();
    
    // Press all arrow keys
    mockInput->pressKey(ECS::Keys::Up);
    mockInput->pressKey(ECS::Keys::Down);
    mockInput->pressKey(ECS::Keys::Left);
    mockInput->pressKey(ECS::Keys::Right);
    
    inputSystem->update(0.016f, *entityManager);
    
    auto* state = entityManager->getComponent<ECS::InputState>(entity);
    ASSERT_NE(state, nullptr);
    
    EXPECT_TRUE(state->moveUp);
    EXPECT_TRUE(state->moveDown);
    EXPECT_TRUE(state->moveLeft);
    EXPECT_TRUE(state->moveRight);
    EXPECT_TRUE(state->upPressed);
    EXPECT_TRUE(state->downPressed);
    EXPECT_TRUE(state->leftPressed);
    EXPECT_TRUE(state->rightPressed);
}