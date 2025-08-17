#include "../include/RenderSystem.hpp"
#include "../include/MockRenderer.hpp"
#include "../../ecs/include/EntityManager.hpp"
#include "../../ecs/components/include/Transform.hpp"
#include "../../ecs/components/include/Rendering.hpp"
#include "../../ecs/include/ComponentRegistry.hpp"
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
        renderSystem = std::make_unique<RenderSystem>(mockRenderer.get());
        
        // Reset mock renderer state
        mockRenderer->reset();
    }
    
    void TearDown() override {
        renderSystem.reset();
        entityManager.reset();
        mockRenderer.reset();
    }
    
    // Helper method to create entity with Position and Sprite components
    EntityID createSpriteEntity(float x, float y, float z, int textureId, float width, float height) {
        Entity entity = entityManager->createEntity();
        Entity* entityRef = entityManager->getEntityByID(entity.id);
        
        // Add component bitmasks for Position and Sprite
        uint64_t positionBit = getComponentBit<Position>();
        uint64_t spriteBit = getComponentBit<Sprite>();
        entityRef->addComponent(positionBit | spriteBit);
        
        // NOTE: Actual component data (x, y, z, textureId, etc.) would be stored 
        // in ComponentArrays, but that's not set up in these tests yet
        (void)x; (void)y; (void)z; (void)textureId; (void)width; (void)height;
        
        return entity.id;
    }
    
    // Helper method to create entity with Position and Renderable components
    EntityID createRenderableEntity(float x, float y, float z, float width, float height, 
                                   float red, float green, float blue, float alpha = 1.0f) {
        Entity entity = entityManager->createEntity();
        Entity* entityRef = entityManager->getEntityByID(entity.id);
        
        // Add component bitmasks for Position and Renderable
        uint64_t positionBit = getComponentBit<Position>();
        uint64_t renderableBit = getComponentBit<Renderable>();
        entityRef->addComponent(positionBit | renderableBit);
        
        // NOTE: Actual component data would be stored in ComponentArrays
        (void)x; (void)y; (void)z; (void)width; (void)height;
        (void)red; (void)green; (void)blue; (void)alpha;
        
        return entity.id;
    }
    
    std::unique_ptr<MockRenderer> mockRenderer;
    std::unique_ptr<EntityManager> entityManager;
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
    EXPECT_GT(priority, 1000);  // Higher than default systems
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
    
    // Frame methods should be called in correct order
    EXPECT_EQ(mockRenderer->getCallCount("beginFrame"), 1);
    EXPECT_EQ(mockRenderer->getCallCount("endFrame"), 1);
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
    EntityID entityId = createSpriteEntity(100.0f, 200.0f, 1.0f, 42, 64.0f, 48.0f);
    
    renderSystem->update(0.016f, *entityManager);
    
    // Should render the sprite entity
    EXPECT_EQ(renderSystem->getLastRenderCount(), 1);
    EXPECT_TRUE(mockRenderer->wasMethodCalled("renderSprite"));
    EXPECT_EQ(mockRenderer->getCallCount("renderSprite"), 1);
    
    // Should call renderSprite with correct parameters
    EXPECT_EQ(mockRenderer->spriteCalls.size(), 1);
    const auto& call = mockRenderer->spriteCalls[0];
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
    const auto& call = mockRenderer->rectCalls[0];
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
    EntityID renderable1 = createRenderableEntity(30.0f, 40.0f, 0.0f, 16.0f, 16.0f, 1.0f, 0.0f, 0.0f);
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
    // STUB: Create entity with only Sprite (no Position) - for Red phase
    Entity entity = entityManager->createEntity();
    EntityID entityId = entity.id;
    
    // STUB: Component management will be implemented in Green phase
    (void)entityId;
    
    renderSystem->update(0.016f, *entityManager);
    
    // Should not render entity without Position
    EXPECT_EQ(renderSystem->getLastRenderCount(), 0);
    EXPECT_FALSE(mockRenderer->wasMethodCalled("renderSprite"));
    EXPECT_FALSE(mockRenderer->wasMethodCalled("renderRect"));
}

// Test entity filtering - entities without visual components should be ignored  
TEST_F(RenderSystemTest, EntityFilteringMissingVisualComponent) {
    // STUB: Create entity with only Position (no Sprite or Renderable) - for Red phase
    Entity entity = entityManager->createEntity();
    EntityID entityId = entity.id;
    
    // STUB: Component management will be implemented in Green phase
    (void)entityId;
    
    renderSystem->update(0.016f, *entityManager);
    
    // Should not render entity without visual components
    EXPECT_EQ(renderSystem->getLastRenderCount(), 0);
    EXPECT_FALSE(mockRenderer->wasMethodCalled("renderSprite"));
    EXPECT_FALSE(mockRenderer->wasMethodCalled("renderRect"));
}

// Test entity with both Sprite and Renderable components
TEST_F(RenderSystemTest, EntityWithBothVisualComponents) {
    // Create entity with Position + Sprite + Renderable
    Entity entity = entityManager->createEntity();
    Entity* entityRef = entityManager->getEntityByID(entity.id);
    
    // Add component bitmasks for Position, Sprite, and Renderable
    uint64_t positionBit = getComponentBit<Position>();
    uint64_t spriteBit = getComponentBit<Sprite>();
    uint64_t renderableBit = getComponentBit<Renderable>();
    entityRef->addComponent(positionBit | spriteBit | renderableBit);
    
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
    EntityID back = createSpriteEntity(100.0f, 100.0f, 0.0f, 1, 32.0f, 32.0f);   // Back
    EntityID front = createSpriteEntity(110.0f, 110.0f, 2.0f, 2, 32.0f, 32.0f);  // Front
    EntityID middle = createSpriteEntity(105.0f, 105.0f, 1.0f, 3, 32.0f, 32.0f); // Middle
    
    renderSystem->update(0.016f, *entityManager);
    
    // Should render all entities
    EXPECT_EQ(renderSystem->getLastRenderCount(), 3);
    EXPECT_EQ(mockRenderer->spriteCalls.size(), 3);
    
    // Z-order sorting is implementation detail, but all should be rendered
    std::vector<float> zValues;
    for (const auto& call : mockRenderer->spriteCalls) {
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
    EntityID zeroRenderable = createRenderableEntity(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    
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