#include "../../ecs/components/include/Rendering.hpp"
#include "../../ecs/components/include/Transform.hpp"
#include "../../ecs/include/ComponentArray.hpp"
#include "../../ecs/include/ComponentRegistry.hpp"
#include "../../ecs/include/EntityManager.hpp"
#include "../include/MockRenderer.hpp"
#include "../include/RenderSystem.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace ECS;

/**
 * RenderSystem Tests
 *
 * Tests for the RenderSystem which processes entities with visual components
 * and renders them using dependency-injected IRenderer implementations.
 *
 * Key test areas:
 * - Dependency injection of IRenderer
 * - Entity filtering with Position + Sprite/Renderable components
 * - Proper renderer method calls with correct parameters
 * - Frame lifecycle management
 * - System interface compliance (ISystem)
 */

class RenderSystemTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Create fresh instances for each test
    mockRenderer = std::make_unique<MockRenderer>();
    entityManager = std::make_unique<EntityManager>();

    // Create component arrays
    positions = std::make_unique<ComponentArray<Position>>();
    sprites = std::make_unique<ComponentArray<Sprite>>();
    renderables = std::make_unique<ComponentArray<Renderable>>();

    // Create RenderSystem with component array injection
    renderSystem = std::make_unique<RenderSystem>(
        mockRenderer.get(), positions.get(), sprites.get(), renderables.get());

    // Reset mock renderer state
    mockRenderer->reset();
  }

  void TearDown() override {
    renderSystem.reset();
    renderables.reset();
    sprites.reset();
    positions.reset();
    entityManager.reset();
    mockRenderer.reset();
  }

  // Helper method to create entity with Position and Sprite components
  EntityID createSpriteEntity(float x, float y, float z, int textureId,
                              float width, float height) {
    Entity entity = entityManager->createEntity();

    // Get component bits
    uint64_t positionBit = getComponentBit<Position>();
    uint64_t spriteBit = getComponentBit<Sprite>();

    // Add Position component with actual data
    positions->add(entity.id, {x, y, z}, positionBit, *entityManager);

    // Add Sprite component with actual data
    sprites->add(entity.id, {textureId, width, height}, spriteBit,
                 *entityManager);

    return entity.id;
  }

  // Helper method to create entity with Position and Renderable components
  EntityID createRenderableEntity(float x, float y, float z, float width,
                                  float height, float red, float green,
                                  float blue, float alpha = 1.0f) {
    Entity entity = entityManager->createEntity();

    // Get component bits
    uint64_t positionBit = getComponentBit<Position>();
    uint64_t renderableBit = getComponentBit<Renderable>();

    // Add Position component with actual data (z unused for rectangles
    // currently)
    positions->add(entity.id, {x, y, z}, positionBit, *entityManager);

    // Add Renderable component with actual data
    renderables->add(entity.id, {width, height, red, green, blue, alpha},
                     renderableBit, *entityManager);

    return entity.id;
  }

  std::unique_ptr<MockRenderer> mockRenderer;
  std::unique_ptr<EntityManager> entityManager;
  std::unique_ptr<ComponentArray<Position>> positions;
  std::unique_ptr<ComponentArray<Sprite>> sprites;
  std::unique_ptr<ComponentArray<Renderable>> renderables;
  std::unique_ptr<RenderSystem> renderSystem;
};

// Test basic system interface compliance
TEST_F(RenderSystemTest, SystemInterfaceCompliance) {
  // Should implement ISystem interface without crashing
  EXPECT_NO_THROW(renderSystem->getRequiredComponents());
  EXPECT_NO_THROW(renderSystem->getPriority());
  EXPECT_NO_THROW(renderSystem->shouldUpdate(0.016f));

  // Should accept update call without crashing
  EXPECT_NO_THROW(renderSystem->update(0.016f, *entityManager));
}

// Test dependency injection
TEST_F(RenderSystemTest, DependencyInjection) {
  // Should store injected renderer
  EXPECT_EQ(renderSystem->getRenderer(), mockRenderer.get());

  // Should work with different renderer implementations
  MockRenderer anotherRenderer;
  RenderSystem anotherSystem(&anotherRenderer);

  EXPECT_EQ(anotherSystem.getRenderer(), &anotherRenderer);
  EXPECT_NE(anotherSystem.getRenderer(), mockRenderer.get());
}

// Test required components bitmask
TEST_F(RenderSystemTest, RequiredComponents) {
  uint64_t requiredMask = renderSystem->getRequiredComponents();
  uint64_t positionBit = getComponentBit<Position>();

  // Should require Position component at minimum
  EXPECT_TRUE((requiredMask & positionBit) != 0);
  EXPECT_GT(requiredMask, 0u);
}

// Test system priority for rendering order
TEST_F(RenderSystemTest, SystemPriority) {
  int priority = renderSystem->getPriority();

  // Rendering should happen after game logic (higher priority number)
  EXPECT_GT(priority, 1000); // Higher than default systems
}

// Test update frequency
TEST_F(RenderSystemTest, UpdateFrequency) {
  // Rendering should happen every frame
  EXPECT_TRUE(renderSystem->shouldUpdate(0.016f));
  EXPECT_TRUE(renderSystem->shouldUpdate(0.033f));
  EXPECT_TRUE(renderSystem->shouldUpdate(0.001f));
}

// Test frame lifecycle management
TEST_F(RenderSystemTest, FrameLifecycle) {
  renderSystem->update(0.016f, *entityManager);

  // Should call beginFrame and endFrame
  EXPECT_TRUE(mockRenderer->wasMethodCalled("beginFrame"));
  EXPECT_TRUE(mockRenderer->wasMethodCalled("endFrame"));

  // Should call clear for screen clearing
  EXPECT_TRUE(mockRenderer->wasMethodCalled("clear"));

  // Frame methods should be called in correct order: beginFrame → clear →
  // endFrame
  std::vector<std::string> expectedSequence = {"beginFrame", "clear",
                                               "endFrame"};
  EXPECT_TRUE(mockRenderer->verifyCallSequence(expectedSequence));

  // Verify individual call counts
  EXPECT_EQ(mockRenderer->getCallCount("beginFrame"), 1);
  EXPECT_EQ(mockRenderer->getCallCount("endFrame"), 1);
  EXPECT_EQ(mockRenderer->getCallCount("clear"), 1);
}

// Test empty scene handling
TEST_F(RenderSystemTest, EmptyScene) {
  renderSystem->update(0.016f, *entityManager);

  // Should handle empty scene without crashing
  EXPECT_EQ(renderSystem->getLastRenderCount(), 0);

  // Should still call frame methods
  EXPECT_TRUE(mockRenderer->wasMethodCalled("beginFrame"));
  EXPECT_TRUE(mockRenderer->wasMethodCalled("endFrame"));

  // Should not call rendering methods
  EXPECT_FALSE(mockRenderer->wasMethodCalled("renderSprite"));
  EXPECT_FALSE(mockRenderer->wasMethodCalled("renderRect"));
}

// Test sprite entity rendering
TEST_F(RenderSystemTest, SpriteEntityRendering) {
  // Create entity with Position + Sprite
  EntityID entityId =
      createSpriteEntity(100.0f, 200.0f, 1.0f, 42, 64.0f, 48.0f);

  renderSystem->update(0.016f, *entityManager);

  // Should render the sprite entity
  EXPECT_EQ(renderSystem->getLastRenderCount(), 1);
  EXPECT_TRUE(mockRenderer->wasMethodCalled("renderSprite"));
  EXPECT_EQ(mockRenderer->getCallCount("renderSprite"), 1);

  // Should call renderSprite with correct parameters
  EXPECT_EQ(mockRenderer->spriteCalls.size(), 1);
  const auto &call = mockRenderer->spriteCalls[0];
  EXPECT_FLOAT_EQ(call.x, 100.0f);
  EXPECT_FLOAT_EQ(call.y, 200.0f);
  EXPECT_FLOAT_EQ(call.z, 1.0f);
  EXPECT_FLOAT_EQ(call.width, 64.0f);
  EXPECT_FLOAT_EQ(call.height, 48.0f);
  EXPECT_EQ(call.textureId, 42);
}

// Test renderable entity rendering
TEST_F(RenderSystemTest, RenderableEntityRendering) {
  // Create entity with Position + Renderable
  EntityID entityId = createRenderableEntity(50.0f, 75.0f, 0.5f, 32.0f, 24.0f,
                                             0.8f, 0.4f, 0.2f, 0.9f);

  renderSystem->update(0.016f, *entityManager);

  // Should render the renderable entity
  EXPECT_EQ(renderSystem->getLastRenderCount(), 1);
  EXPECT_TRUE(mockRenderer->wasMethodCalled("renderRect"));
  EXPECT_EQ(mockRenderer->getCallCount("renderRect"), 1);

  // Should call renderRect with correct parameters
  EXPECT_EQ(mockRenderer->rectCalls.size(), 1);
  const auto &call = mockRenderer->rectCalls[0];
  EXPECT_FLOAT_EQ(call.x, 50.0f);
  EXPECT_FLOAT_EQ(call.y, 75.0f);
  EXPECT_FLOAT_EQ(call.width, 32.0f);
  EXPECT_FLOAT_EQ(call.height, 24.0f);
  EXPECT_FLOAT_EQ(call.red, 0.8f);
  EXPECT_FLOAT_EQ(call.green, 0.4f);
  EXPECT_FLOAT_EQ(call.blue, 0.2f);
  EXPECT_FLOAT_EQ(call.alpha, 0.9f);
}

// Test multiple entity rendering
TEST_F(RenderSystemTest, MultipleEntityRendering) {
  // Create mixed entities
  EntityID sprite1 = createSpriteEntity(10.0f, 20.0f, 0.0f, 1, 32.0f, 32.0f);
  EntityID renderable1 = createRenderableEntity(30.0f, 40.0f, 0.0f, 16.0f,
                                                16.0f, 1.0f, 0.0f, 0.0f);
  EntityID sprite2 = createSpriteEntity(50.0f, 60.0f, 0.0f, 2, 48.0f, 48.0f);

  renderSystem->update(0.016f, *entityManager);

  // Should render all entities
  EXPECT_EQ(renderSystem->getLastRenderCount(), 3);

  // Should call both rendering methods
  EXPECT_EQ(mockRenderer->getCallCount("renderSprite"), 2);
  EXPECT_EQ(mockRenderer->getCallCount("renderRect"), 1);

  // Verify all calls were recorded
  EXPECT_EQ(mockRenderer->spriteCalls.size(), 2);
  EXPECT_EQ(mockRenderer->rectCalls.size(), 1);
}

// Test entity filtering - entities without Position should be ignored
TEST_F(RenderSystemTest, EntityFilteringMissingPosition) {
  // Create entity with only Sprite component (no Position component)
  Entity entity = entityManager->createEntity();
  Entity *entityRef = entityManager->getEntityByID(entity.id);

  // Add only Sprite component bitmask (missing Position)
  uint64_t spriteBit = getComponentBit<Sprite>();
  entityRef->addComponent(spriteBit);

  renderSystem->update(0.016f, *entityManager);

  // Should not render entity without Position component
  EXPECT_EQ(renderSystem->getLastRenderCount(), 0);
  EXPECT_FALSE(mockRenderer->wasMethodCalled("renderSprite"));
  EXPECT_FALSE(mockRenderer->wasMethodCalled("renderRect"));
}

// Test entity filtering - entities without visual components should be ignored
TEST_F(RenderSystemTest, EntityFilteringMissingVisualComponent) {
  // Create entity with only Position component (no Sprite or Renderable
  // components)
  Entity entity = entityManager->createEntity();
  Entity *entityRef = entityManager->getEntityByID(entity.id);

  // Add only Position component bitmask (missing Sprite and Renderable)
  uint64_t positionBit = getComponentBit<Position>();
  entityRef->addComponent(positionBit);

  renderSystem->update(0.016f, *entityManager);

  // Should not render entity without visual components (Sprite or Renderable)
  EXPECT_EQ(renderSystem->getLastRenderCount(), 0);
  EXPECT_FALSE(mockRenderer->wasMethodCalled("renderSprite"));
  EXPECT_FALSE(mockRenderer->wasMethodCalled("renderRect"));
}

// Test entity with both Sprite and Renderable components
TEST_F(RenderSystemTest, EntityWithBothVisualComponents) {
  // Create entity with Position + Sprite + Renderable
  Entity entity = entityManager->createEntity();

  // Add actual component data for Position, Sprite, and Renderable
  uint64_t positionBit = getComponentBit<Position>();
  uint64_t spriteBit = getComponentBit<Sprite>();
  uint64_t renderableBit = getComponentBit<Renderable>();

  // Add actual component data
  positions->add(entity.id, {100.0f, 100.0f, 0.0f}, positionBit, *entityManager);
  sprites->add(entity.id, {1, 32.0f, 32.0f}, spriteBit, *entityManager);
  renderables->add(entity.id, {32.0f, 32.0f, 1.0f, 0.0f, 0.0f, 1.0f}, renderableBit, *entityManager);

  renderSystem->update(0.016f, *entityManager);

  // Should render both sprite and renderable (implementation choice)
  // This tests how system handles entities with multiple visual components
  EXPECT_GT(renderSystem->getLastRenderCount(), 0);

  // Should call both rendering methods since entity has both components
  bool spriteRendered = mockRenderer->wasMethodCalled("renderSprite");
  bool rectRendered = mockRenderer->wasMethodCalled("renderRect");
  EXPECT_TRUE(spriteRendered || rectRendered);
}

// Test multiple update calls
TEST_F(RenderSystemTest, MultipleUpdateCalls) {
  EntityID entityId = createSpriteEntity(100.0f, 100.0f, 0.0f, 1, 32.0f, 32.0f);

  // First update
  renderSystem->update(0.016f, *entityManager);
  EXPECT_EQ(renderSystem->getLastRenderCount(), 1);

  // Reset mock renderer
  mockRenderer->reset();

  // Second update
  renderSystem->update(0.016f, *entityManager);
  EXPECT_EQ(renderSystem->getLastRenderCount(), 1);

  // Should call frame methods again
  EXPECT_TRUE(mockRenderer->wasMethodCalled("beginFrame"));
  EXPECT_TRUE(mockRenderer->wasMethodCalled("endFrame"));
}

// Test Z-order handling (depth sorting)
TEST_F(RenderSystemTest, ZOrderHandling) {
  // Create entities at different Z depths
  EntityID back =
      createSpriteEntity(100.0f, 100.0f, 0.0f, 1, 32.0f, 32.0f); // Back
  EntityID front =
      createSpriteEntity(110.0f, 110.0f, 2.0f, 2, 32.0f, 32.0f); // Front
  EntityID middle =
      createSpriteEntity(105.0f, 105.0f, 1.0f, 3, 32.0f, 32.0f); // Middle

  renderSystem->update(0.016f, *entityManager);

  // Should render all entities
  EXPECT_EQ(renderSystem->getLastRenderCount(), 3);
  EXPECT_EQ(mockRenderer->spriteCalls.size(), 3);

  // Z-order sorting is implementation detail, but all should be rendered
  std::vector<float> zValues;
  for (const auto &call : mockRenderer->spriteCalls) {
    zValues.push_back(call.z);
  }

  // Should have all Z values present
  EXPECT_TRUE(std::find(zValues.begin(), zValues.end(), 0.0f) != zValues.end());
  EXPECT_TRUE(std::find(zValues.begin(), zValues.end(), 1.0f) != zValues.end());
  EXPECT_TRUE(std::find(zValues.begin(), zValues.end(), 2.0f) != zValues.end());
}

// Test edge cases and robustness
TEST_F(RenderSystemTest, EdgeCases) {
  // Zero-size sprites and renderables should still be processed
  EntityID zeroSprite = createSpriteEntity(0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f);
  EntityID zeroRenderable =
      createRenderableEntity(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

  renderSystem->update(0.016f, *entityManager);

  // Should handle edge cases without crashing
  EXPECT_EQ(renderSystem->getLastRenderCount(), 2);

  // Should make appropriate renderer calls
  EXPECT_EQ(mockRenderer->getCallCount("renderSprite"), 1);
  EXPECT_EQ(mockRenderer->getCallCount("renderRect"), 1);
}

// Test system with null renderer (error handling)
TEST_F(RenderSystemTest, NullRendererHandling) {
  // Create system with null renderer
  RenderSystem nullSystem(nullptr);

  // Should handle null renderer gracefully (implementation choice)
  // Could throw exception or handle gracefully - depends on design decision
  EXPECT_NO_THROW(nullSystem.getRenderer());

  // Update with null renderer should either throw or handle gracefully
  // For now, test that it doesn't crash the program
  EXPECT_NO_THROW(nullSystem.update(0.016f, *entityManager));
}

// ========================================================================
// Sprite Component Integration Tests
// These tests verify RenderSystem uses actual component data from arrays
// ========================================================================

class RenderSystemSpriteComponentTest : public ::testing::Test {
protected:
  void SetUp() override {
    mockRenderer = std::make_unique<MockRenderer>();
    entityManager = std::make_unique<EntityManager>();

    // Create component arrays
    positions = std::make_unique<ComponentArray<Position>>();
    sprites = std::make_unique<ComponentArray<Sprite>>();
    renderables = std::make_unique<ComponentArray<Renderable>>();

    // Create render system with component array dependency injection
    renderSystem = std::make_unique<RenderSystem>(
        mockRenderer.get(), positions.get(), sprites.get(), renderables.get());

    mockRenderer->reset();
  }

  void TearDown() override {
    renderSystem.reset();
    renderables.reset();
    sprites.reset();
    positions.reset();
    entityManager.reset();
    mockRenderer.reset();
  }

  std::unique_ptr<MockRenderer> mockRenderer;
  std::unique_ptr<EntityManager> entityManager;
  std::unique_ptr<ComponentArray<Position>> positions;
  std::unique_ptr<ComponentArray<Sprite>> sprites;
  std::unique_ptr<ComponentArray<Renderable>> renderables;
  std::unique_ptr<RenderSystem> renderSystem;
};

// Test that RenderSystem uses actual Position component data
TEST_F(RenderSystemSpriteComponentTest, UsesActualPositionData) {
  // Create entity with specific position
  Entity entity = entityManager->createEntity();

  // Add Position component with specific values
  uint64_t posBit = getComponentBit<Position>();
  positions->add(entity.id, {123.5f, 456.7f, 2.0f}, posBit, *entityManager);

  // Add Sprite component
  uint64_t spriteBit = getComponentBit<Sprite>();
  sprites->add(entity.id, {1, 64.0f, 48.0f}, spriteBit, *entityManager);

  // Render the entity
  renderSystem->update(0.016f, *entityManager);

  // Verify renderer received the actual position data (NOT placeholder values)
  ASSERT_EQ(mockRenderer->getSpriteCallCount(), 1);
  const auto &spriteCall = mockRenderer->getSpriteCall(0);
  EXPECT_FLOAT_EQ(spriteCall.x, 123.5f)
      << "Should use actual Position.x, not placeholder";
  EXPECT_FLOAT_EQ(spriteCall.y, 456.7f)
      << "Should use actual Position.y, not placeholder";
  EXPECT_FLOAT_EQ(spriteCall.z, 2.0f)
      << "Should use actual Position.z, not placeholder";
}

// Test that RenderSystem uses actual Sprite component data
TEST_F(RenderSystemSpriteComponentTest, UsesActualSpriteData) {
  // Create entity with specific sprite data
  Entity entity = entityManager->createEntity();

  // Add Position component
  uint64_t posBit = getComponentBit<Position>();
  positions->add(entity.id, {10.0f, 20.0f, 0.0f}, posBit, *entityManager);

  // Add Sprite component with specific texture and dimensions
  uint64_t spriteBit = getComponentBit<Sprite>();
  sprites->add(entity.id, {99, 128.0f, 96.0f}, spriteBit, *entityManager);

  // Render the entity
  renderSystem->update(0.016f, *entityManager);

  // Verify renderer received the actual sprite data (NOT placeholder values)
  ASSERT_EQ(mockRenderer->getSpriteCallCount(), 1);
  const auto &spriteCall = mockRenderer->getSpriteCall(0);
  EXPECT_EQ(spriteCall.textureId, 99)
      << "Should use actual Sprite.textureId, not placeholder";
  EXPECT_FLOAT_EQ(spriteCall.width, 128.0f)
      << "Should use actual Sprite.width, not placeholder";
  EXPECT_FLOAT_EQ(spriteCall.height, 96.0f)
      << "Should use actual Sprite.height, not placeholder";
}

// Test rendering multiple sprites with different component data
TEST_F(RenderSystemSpriteComponentTest, RendersMultipleSpritesWithUniqueData) {
  // Create first entity with specific data
  Entity entity1 = entityManager->createEntity();
  uint64_t posBit = getComponentBit<Position>();
  uint64_t spriteBit = getComponentBit<Sprite>();
  positions->add(entity1.id, {50.0f, 100.0f, 0.0f}, posBit, *entityManager);
  sprites->add(entity1.id, {10, 32.0f, 32.0f}, spriteBit, *entityManager);

  // Create second entity with different data
  Entity entity2 = entityManager->createEntity();
  positions->add(entity2.id, {200.0f, 300.0f, 1.0f}, posBit, *entityManager);
  sprites->add(entity2.id, {20, 64.0f, 64.0f}, spriteBit, *entityManager);

  // Create third entity with yet different data
  Entity entity3 = entityManager->createEntity();
  positions->add(entity3.id, {400.0f, 500.0f, 2.0f}, posBit, *entityManager);
  sprites->add(entity3.id, {30, 128.0f, 128.0f}, spriteBit, *entityManager);

  // Render all entities
  renderSystem->update(0.016f, *entityManager);

  // Should have rendered 3 sprites
  ASSERT_EQ(mockRenderer->getSpriteCallCount(), 3);

  // Verify first sprite uses entity1's data
  const auto &call1 = mockRenderer->getSpriteCall(0);
  EXPECT_FLOAT_EQ(call1.x, 50.0f);
  EXPECT_FLOAT_EQ(call1.y, 100.0f);
  EXPECT_EQ(call1.textureId, 10);
  EXPECT_FLOAT_EQ(call1.width, 32.0f);

  // Verify second sprite uses entity2's data
  const auto &call2 = mockRenderer->getSpriteCall(1);
  EXPECT_FLOAT_EQ(call2.x, 200.0f);
  EXPECT_FLOAT_EQ(call2.y, 300.0f);
  EXPECT_EQ(call2.textureId, 20);
  EXPECT_FLOAT_EQ(call2.width, 64.0f);

  // Verify third sprite uses entity3's data
  const auto &call3 = mockRenderer->getSpriteCall(2);
  EXPECT_FLOAT_EQ(call3.x, 400.0f);
  EXPECT_FLOAT_EQ(call3.y, 500.0f);
  EXPECT_EQ(call3.textureId, 30);
  EXPECT_FLOAT_EQ(call3.width, 128.0f);
}

// Test that entities without components are skipped
TEST_F(RenderSystemSpriteComponentTest, SkipsEntitiesWithoutComponents) {
  // Create entity with only Position (no Sprite)
  Entity entity1 = entityManager->createEntity();
  uint64_t posBit = getComponentBit<Position>();
  positions->add(entity1.id, {10.0f, 20.0f, 0.0f}, posBit, *entityManager);

  // Create entity with only Sprite (no Position)
  Entity entity2 = entityManager->createEntity();
  uint64_t spriteBit = getComponentBit<Sprite>();
  sprites->add(entity2.id, {1, 32.0f, 32.0f}, spriteBit, *entityManager);

  // Create entity with both Position and Sprite
  Entity entity3 = entityManager->createEntity();
  positions->add(entity3.id, {100.0f, 200.0f, 0.0f}, posBit, *entityManager);
  sprites->add(entity3.id, {2, 64.0f, 64.0f}, spriteBit, *entityManager);

  // Render - should only render entity3
  renderSystem->update(0.016f, *entityManager);

  // Should have rendered only the entity with both components
  EXPECT_EQ(mockRenderer->getSpriteCallCount(), 1);
  const auto &call = mockRenderer->getSpriteCall(0);
  EXPECT_FLOAT_EQ(call.x, 100.0f);
  EXPECT_EQ(call.textureId, 2);
}

// Test that Renderable components also use actual data
TEST_F(RenderSystemSpriteComponentTest, UsesActualRenderableData) {
  // Create entity with specific renderable data
  Entity entity = entityManager->createEntity();

  // Add Position component
  uint64_t posBit = getComponentBit<Position>();
  positions->add(entity.id, {75.5f, 125.5f, 1.5f}, posBit, *entityManager);

  // Add Renderable component with specific color and dimensions
  uint64_t renderableBit = getComponentBit<Renderable>();
  renderables->add(entity.id, {80.0f, 60.0f, 0.8f, 0.6f, 0.4f, 0.9f},
                   renderableBit, *entityManager);

  // Render the entity
  renderSystem->update(0.016f, *entityManager);

  // Verify renderer received the actual renderable data
  ASSERT_EQ(mockRenderer->getRectCallCount(), 1);
  const auto &rectCall = mockRenderer->getRectCall(0);
  EXPECT_FLOAT_EQ(rectCall.x, 75.5f) << "Should use actual Position.x";
  EXPECT_FLOAT_EQ(rectCall.y, 125.5f) << "Should use actual Position.y";
  EXPECT_FLOAT_EQ(rectCall.width, 80.0f)
      << "Should use actual Renderable.width";
  EXPECT_FLOAT_EQ(rectCall.height, 60.0f)
      << "Should use actual Renderable.height";
  EXPECT_FLOAT_EQ(rectCall.red, 0.8f) << "Should use actual Renderable.red";
  EXPECT_FLOAT_EQ(rectCall.green, 0.6f) << "Should use actual Renderable.green";
  EXPECT_FLOAT_EQ(rectCall.blue, 0.4f) << "Should use actual Renderable.blue";
  EXPECT_FLOAT_EQ(rectCall.alpha, 0.9f) << "Should use actual Renderable.alpha";
}

// ============================================================================
// Z-ORDERING / DEPTH SORTING TESTS
// ============================================================================

// Test that sprite entities are rendered in Z-order (back to front)
TEST_F(RenderSystemTest, SpriteEntitiesRenderedInZOrder) {
  // Create sprites with different z values (intentionally out of order)
  EntityID spriteBack = createSpriteEntity(100.0f, 100.0f, 0.0f, 1, 32.0f, 32.0f);   // z=0.0 (back)
  EntityID spriteFront = createSpriteEntity(200.0f, 200.0f, 2.0f, 2, 32.0f, 32.0f);  // z=2.0 (front)
  EntityID spriteMiddle = createSpriteEntity(150.0f, 150.0f, 1.0f, 3, 32.0f, 32.0f); // z=1.0 (middle)

  renderSystem->update(0.016f, *entityManager);

  // Should render all 3 sprites
  ASSERT_EQ(mockRenderer->getSpriteCallCount(), 3) << "Should render all 3 sprites";

  // Verify rendering order: back to front (z=0.0, z=1.0, z=2.0)
  const auto& call0 = mockRenderer->getSpriteCall(0);
  const auto& call1 = mockRenderer->getSpriteCall(1);
  const auto& call2 = mockRenderer->getSpriteCall(2);

  EXPECT_FLOAT_EQ(call0.z, 0.0f) << "First sprite should have z=0.0 (back)";
  EXPECT_FLOAT_EQ(call1.z, 1.0f) << "Second sprite should have z=1.0 (middle)";
  EXPECT_FLOAT_EQ(call2.z, 2.0f) << "Third sprite should have z=2.0 (front)";

  // Also verify the x positions match (sanity check)
  EXPECT_FLOAT_EQ(call0.x, 100.0f) << "First rendered should be spriteBack";
  EXPECT_FLOAT_EQ(call1.x, 150.0f) << "Second rendered should be spriteMiddle";
  EXPECT_FLOAT_EQ(call2.x, 200.0f) << "Third rendered should be spriteFront";
}

// Test that renderable entities are rendered in Z-order
TEST_F(RenderSystemTest, RenderableEntitiesRenderedInZOrder) {
  // Create renderables with different z values (out of order)
  EntityID rectFront = createRenderableEntity(300.0f, 300.0f, 5.0f, 16.0f, 16.0f, 1.0f, 0.0f, 0.0f);   // z=5.0 (front)
  EntityID rectBack = createRenderableEntity(100.0f, 100.0f, -1.0f, 16.0f, 16.0f, 0.0f, 1.0f, 0.0f);   // z=-1.0 (back)
  EntityID rectMiddle = createRenderableEntity(200.0f, 200.0f, 2.0f, 16.0f, 16.0f, 0.0f, 0.0f, 1.0f);  // z=2.0 (middle)

  renderSystem->update(0.016f, *entityManager);

  // Should render all 3 rectangles
  ASSERT_EQ(mockRenderer->getRectCallCount(), 3) << "Should render all 3 rectangles";

  // Verify rendering order: z=-1.0, z=2.0, z=5.0
  const auto& call0 = mockRenderer->getRectCall(0);
  const auto& call1 = mockRenderer->getRectCall(1);
  const auto& call2 = mockRenderer->getRectCall(2);

  EXPECT_FLOAT_EQ(call0.x, 100.0f) << "First rendered should be rectBack (z=-1.0)";
  EXPECT_FLOAT_EQ(call1.x, 200.0f) << "Second rendered should be rectMiddle (z=2.0)";
  EXPECT_FLOAT_EQ(call2.x, 300.0f) << "Third rendered should be rectFront (z=5.0)";

  // Verify colors match expected order
  EXPECT_FLOAT_EQ(call0.green, 1.0f) << "First should be green (rectBack)";
  EXPECT_FLOAT_EQ(call1.blue, 1.0f) << "Second should be blue (rectMiddle)";
  EXPECT_FLOAT_EQ(call2.red, 1.0f) << "Third should be red (rectFront)";
}

// Test mixed sprites and renderables rendered in correct Z-order
TEST_F(RenderSystemTest, MixedEntitiesRenderedInZOrder) {
  // Create mix of sprites and renderables at different z values
  EntityID sprite1 = createSpriteEntity(100.0f, 100.0f, 1.0f, 1, 32.0f, 32.0f);        // z=1.0
  EntityID rect1 = createRenderableEntity(200.0f, 200.0f, 0.0f, 16.0f, 16.0f, 1.0f, 0.0f, 0.0f);  // z=0.0
  EntityID sprite2 = createSpriteEntity(300.0f, 300.0f, 3.0f, 2, 32.0f, 32.0f);        // z=3.0
  EntityID rect2 = createRenderableEntity(400.0f, 400.0f, 2.0f, 16.0f, 16.0f, 0.0f, 1.0f, 0.0f);  // z=2.0

  renderSystem->update(0.016f, *entityManager);

  // Total render count should be 4
  EXPECT_EQ(renderSystem->getLastRenderCount(), 4);

  // Verify sprites rendered in Z-order
  ASSERT_EQ(mockRenderer->getSpriteCallCount(), 2);
  const auto& spriteCall0 = mockRenderer->getSpriteCall(0);
  const auto& spriteCall1 = mockRenderer->getSpriteCall(1);

  EXPECT_FLOAT_EQ(spriteCall0.z, 1.0f) << "First sprite should be z=1.0";
  EXPECT_FLOAT_EQ(spriteCall1.z, 3.0f) << "Second sprite should be z=3.0";

  // Verify rects rendered in Z-order
  ASSERT_EQ(mockRenderer->getRectCallCount(), 2);
  const auto& rectCall0 = mockRenderer->getRectCall(0);
  const auto& rectCall1 = mockRenderer->getRectCall(1);

  EXPECT_FLOAT_EQ(rectCall0.x, 200.0f) << "First rect should be z=0.0";
  EXPECT_FLOAT_EQ(rectCall1.x, 400.0f) << "Second rect should be z=2.0";
}

// Test entities with equal Z values maintain stable order
TEST_F(RenderSystemTest, EqualZValuesStableOrder) {
  // Create entities with same Z value
  EntityID sprite1 = createSpriteEntity(100.0f, 100.0f, 1.0f, 1, 32.0f, 32.0f);
  EntityID sprite2 = createSpriteEntity(200.0f, 200.0f, 1.0f, 2, 32.0f, 32.0f);
  EntityID sprite3 = createSpriteEntity(300.0f, 300.0f, 1.0f, 3, 32.0f, 32.0f);

  renderSystem->update(0.016f, *entityManager);

  // Should render all 3 sprites
  ASSERT_EQ(mockRenderer->getSpriteCallCount(), 3);

  // With stable sort, order should match creation order when z values are equal
  const auto& call0 = mockRenderer->getSpriteCall(0);
  const auto& call1 = mockRenderer->getSpriteCall(1);
  const auto& call2 = mockRenderer->getSpriteCall(2);

  EXPECT_FLOAT_EQ(call0.x, 100.0f) << "First should be sprite1";
  EXPECT_FLOAT_EQ(call1.x, 200.0f) << "Second should be sprite2";
  EXPECT_FLOAT_EQ(call2.x, 300.0f) << "Third should be sprite3";
}

// Test negative Z values work correctly
TEST_F(RenderSystemTest, NegativeZValues) {
  // Create sprites with negative Z values
  EntityID spriteNeg2 = createSpriteEntity(100.0f, 100.0f, -2.0f, 1, 32.0f, 32.0f);  // z=-2.0
  EntityID spritePos1 = createSpriteEntity(200.0f, 200.0f, 1.0f, 2, 32.0f, 32.0f);   // z=1.0
  EntityID spriteNeg1 = createSpriteEntity(150.0f, 150.0f, -1.0f, 3, 32.0f, 32.0f);  // z=-1.0
  EntityID spriteZero = createSpriteEntity(175.0f, 175.0f, 0.0f, 4, 32.0f, 32.0f);   // z=0.0

  renderSystem->update(0.016f, *entityManager);

  ASSERT_EQ(mockRenderer->getSpriteCallCount(), 4);

  // Verify correct order: z=-2.0, z=-1.0, z=0.0, z=1.0
  const auto& call0 = mockRenderer->getSpriteCall(0);
  const auto& call1 = mockRenderer->getSpriteCall(1);
  const auto& call2 = mockRenderer->getSpriteCall(2);
  const auto& call3 = mockRenderer->getSpriteCall(3);

  EXPECT_FLOAT_EQ(call0.z, -2.0f) << "First should be z=-2.0";
  EXPECT_FLOAT_EQ(call1.z, -1.0f) << "Second should be z=-1.0";
  EXPECT_FLOAT_EQ(call2.z, 0.0f) << "Third should be z=0.0";
  EXPECT_FLOAT_EQ(call3.z, 1.0f) << "Fourth should be z=1.0";
}
