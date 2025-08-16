#include "../include/Rendering.hpp"
#include "../../include/ComponentRegistry.hpp"
#include <gtest/gtest.h>

using namespace ECS;

/**
 * Rendering Component Tests
 * 
 * Tests for Sprite and Renderable components used in the rendering system.
 * These components follow ECS design principles:
 * - POD structs with no behavior
 * - Zero-initialized by default (ZII compliant)
 * - Cache-friendly for SoA storage
 * - Work with dependency-injected IRenderer interface
 */

class RenderingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Component registry automatically handles component registration
    }

    void TearDown() override {
        // No cleanup needed for component registry
    }
};

// Test Sprite component
TEST_F(RenderingTest, SpriteDefaultConstruction) {
    Sprite sprite;
    
    // ZII compliance - all values should be zero-initialized
    EXPECT_EQ(sprite.textureId, 0);
    EXPECT_FLOAT_EQ(sprite.width, 0.0f);
    EXPECT_FLOAT_EQ(sprite.height, 0.0f);
}

TEST_F(RenderingTest, SpriteValueConstruction) {
    Sprite sprite{42, 64.0f, 32.0f};
    
    EXPECT_EQ(sprite.textureId, 42);
    EXPECT_FLOAT_EQ(sprite.width, 64.0f);
    EXPECT_FLOAT_EQ(sprite.height, 32.0f);
}

TEST_F(RenderingTest, SpriteEquality) {
    Sprite sprite1{1, 50.0f, 75.0f};
    Sprite sprite2{1, 50.0f, 75.0f};
    Sprite sprite3{1, 50.0f, 80.0f};
    
    EXPECT_EQ(sprite1, sprite2);
    EXPECT_NE(sprite1, sprite3);
}

TEST_F(RenderingTest, SpritePartialInitialization) {
    // Test that we can initialize just some fields
    Sprite sprite{5};  // Only textureId, others should be zero
    
    EXPECT_EQ(sprite.textureId, 5);
    EXPECT_FLOAT_EQ(sprite.width, 0.0f);
    EXPECT_FLOAT_EQ(sprite.height, 0.0f);
}

TEST_F(RenderingTest, SpriteNegativeValues) {
    // Components should accept negative values (for flipping, etc.)
    Sprite sprite{-1, -10.0f, -20.0f};
    
    EXPECT_EQ(sprite.textureId, -1);
    EXPECT_FLOAT_EQ(sprite.width, -10.0f);
    EXPECT_FLOAT_EQ(sprite.height, -20.0f);
}

// Test Renderable component
TEST_F(RenderingTest, RenderableDefaultConstruction) {
    Renderable renderable;
    
    // ZII compliance - dimensions and RGB should be zero, alpha should be 1.0 (opaque)
    EXPECT_FLOAT_EQ(renderable.width, 0.0f);
    EXPECT_FLOAT_EQ(renderable.height, 0.0f);
    EXPECT_FLOAT_EQ(renderable.red, 0.0f);
    EXPECT_FLOAT_EQ(renderable.green, 0.0f);
    EXPECT_FLOAT_EQ(renderable.blue, 0.0f);
    EXPECT_FLOAT_EQ(renderable.alpha, 1.0f);  // Default to opaque
}

TEST_F(RenderingTest, RenderableValueConstruction) {
    Renderable renderable{100.0f, 200.0f, 0.8f, 0.4f, 0.2f, 0.9f};
    
    EXPECT_FLOAT_EQ(renderable.width, 100.0f);
    EXPECT_FLOAT_EQ(renderable.height, 200.0f);
    EXPECT_FLOAT_EQ(renderable.red, 0.8f);
    EXPECT_FLOAT_EQ(renderable.green, 0.4f);
    EXPECT_FLOAT_EQ(renderable.blue, 0.2f);
    EXPECT_FLOAT_EQ(renderable.alpha, 0.9f);
}

TEST_F(RenderingTest, RenderableEquality) {
    Renderable r1{50.0f, 50.0f, 1.0f, 0.0f, 0.0f, 1.0f};  // Red square
    Renderable r2{50.0f, 50.0f, 1.0f, 0.0f, 0.0f, 1.0f};  // Same red square
    Renderable r3{50.0f, 50.0f, 0.0f, 1.0f, 0.0f, 1.0f};  // Green square
    
    EXPECT_EQ(r1, r2);
    EXPECT_NE(r1, r3);
}

TEST_F(RenderingTest, RenderablePartialInitialization) {
    // Test that we can initialize just dimensions
    Renderable renderable{25.0f, 35.0f};  // Only width/height, colors should default
    
    EXPECT_FLOAT_EQ(renderable.width, 25.0f);
    EXPECT_FLOAT_EQ(renderable.height, 35.0f);
    EXPECT_FLOAT_EQ(renderable.red, 0.0f);
    EXPECT_FLOAT_EQ(renderable.green, 0.0f);
    EXPECT_FLOAT_EQ(renderable.blue, 0.0f);
    EXPECT_FLOAT_EQ(renderable.alpha, 1.0f);
}

TEST_F(RenderingTest, RenderableColorRange) {
    // Test valid color ranges (0.0 - 1.0)
    Renderable renderable{10.0f, 10.0f, 0.0f, 0.5f, 1.0f, 0.75f};
    
    EXPECT_FLOAT_EQ(renderable.red, 0.0f);      // Minimum
    EXPECT_FLOAT_EQ(renderable.green, 0.5f);    // Middle
    EXPECT_FLOAT_EQ(renderable.blue, 1.0f);     // Maximum
    EXPECT_FLOAT_EQ(renderable.alpha, 0.75f);   // Partial transparency
}

TEST_F(RenderingTest, RenderableOutOfRangeColors) {
    // Components should accept out-of-range values (clamping is renderer's job)
    Renderable renderable{10.0f, 10.0f, -0.5f, 1.5f, 2.0f, -1.0f};
    
    EXPECT_FLOAT_EQ(renderable.red, -0.5f);
    EXPECT_FLOAT_EQ(renderable.green, 1.5f);
    EXPECT_FLOAT_EQ(renderable.blue, 2.0f);
    EXPECT_FLOAT_EQ(renderable.alpha, -1.0f);
}

// Test component registry integration
TEST_F(RenderingTest, ComponentRegistryIntegration) {
    // Test that components can be registered and retrieved
    // Components should have unique bit positions
    uint64_t spriteBit = getComponentBit<Sprite>();
    uint64_t renderableBit = getComponentBit<Renderable>();
    
    EXPECT_NE(spriteBit, renderableBit);
    EXPECT_GT(spriteBit, 0u);
    EXPECT_GT(renderableBit, 0u);
    
    // Same component type should return same bit
    uint64_t spriteBit2 = getComponentBit<Sprite>();
    EXPECT_EQ(spriteBit, spriteBit2);
}

// Test ZII compliance (Zero Is Initialization)
TEST_F(RenderingTest, ZIICompliance) {
    // Test that zero-initialized components are valid
    Sprite zeroSprite{};
    Renderable zeroRenderable{};
    
    // Zero sprites should be valid (invisible/no texture)
    EXPECT_EQ(zeroSprite.textureId, 0);
    EXPECT_EQ(zeroSprite.width, 0.0f);
    EXPECT_EQ(zeroSprite.height, 0.0f);
    
    // Zero renderables should be valid (invisible colored shape, but alpha=1.0)
    EXPECT_EQ(zeroRenderable.width, 0.0f);
    EXPECT_EQ(zeroRenderable.height, 0.0f);
    EXPECT_EQ(zeroRenderable.red, 0.0f);
    EXPECT_EQ(zeroRenderable.green, 0.0f);
    EXPECT_EQ(zeroRenderable.blue, 0.0f);
    EXPECT_EQ(zeroRenderable.alpha, 1.0f);  // Opaque by default
}

// Test typical usage patterns
TEST_F(RenderingTest, TypicalSpriteUsage) {
    // Player character sprite
    Sprite playerSprite{10, 32.0f, 48.0f};  // TextureID 10, 32x48 pixels
    
    EXPECT_EQ(playerSprite.textureId, 10);
    EXPECT_FLOAT_EQ(playerSprite.width, 32.0f);
    EXPECT_FLOAT_EQ(playerSprite.height, 48.0f);
    
    // UI element sprite
    Sprite buttonSprite{100, 120.0f, 40.0f};  // TextureID 100, 120x40 pixels
    
    EXPECT_EQ(buttonSprite.textureId, 100);
    EXPECT_FLOAT_EQ(buttonSprite.width, 120.0f);
    EXPECT_FLOAT_EQ(buttonSprite.height, 40.0f);
}

TEST_F(RenderingTest, TypicalRenderableUsage) {
    // Red debug rectangle
    Renderable debugRect{10.0f, 10.0f, 1.0f, 0.0f, 0.0f, 0.8f};  // Semi-transparent red
    
    EXPECT_FLOAT_EQ(debugRect.width, 10.0f);
    EXPECT_FLOAT_EQ(debugRect.height, 10.0f);
    EXPECT_FLOAT_EQ(debugRect.red, 1.0f);
    EXPECT_FLOAT_EQ(debugRect.green, 0.0f);
    EXPECT_FLOAT_EQ(debugRect.blue, 0.0f);
    EXPECT_FLOAT_EQ(debugRect.alpha, 0.8f);
    
    // Green health bar background
    Renderable healthBar{100.0f, 8.0f, 0.0f, 0.8f, 0.0f, 1.0f};  // Opaque green
    
    EXPECT_FLOAT_EQ(healthBar.width, 100.0f);
    EXPECT_FLOAT_EQ(healthBar.height, 8.0f);
    EXPECT_FLOAT_EQ(healthBar.red, 0.0f);
    EXPECT_FLOAT_EQ(healthBar.green, 0.8f);
    EXPECT_FLOAT_EQ(healthBar.blue, 0.0f);
    EXPECT_FLOAT_EQ(healthBar.alpha, 1.0f);
}

// Test edge cases and robustness
TEST_F(RenderingTest, EdgeCases) {
    // Zero-dimension sprites and renderables should be valid
    Sprite tinySprite{1, 0.0f, 0.0f};
    Renderable tinyRenderable{0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    
    EXPECT_EQ(tinySprite.textureId, 1);
    EXPECT_FLOAT_EQ(tinySprite.width, 0.0f);
    EXPECT_FLOAT_EQ(tinySprite.height, 0.0f);
    
    EXPECT_FLOAT_EQ(tinyRenderable.width, 0.0f);
    EXPECT_FLOAT_EQ(tinyRenderable.height, 0.0f);
    
    // Very large dimensions should be accepted
    Sprite hugeSprite{999, 9999.0f, 9999.0f};
    
    EXPECT_EQ(hugeSprite.textureId, 999);
    EXPECT_FLOAT_EQ(hugeSprite.width, 9999.0f);
    EXPECT_FLOAT_EQ(hugeSprite.height, 9999.0f);
}

// Test component combinations (entities can have both)
TEST_F(RenderingTest, ComponentCombinations) {
    // An entity could theoretically have both Sprite and Renderable
    // (e.g., sprite with colored overlay effect)
    Sprite sprite{5, 64.0f, 64.0f};
    Renderable overlay{64.0f, 64.0f, 1.0f, 0.0f, 0.0f, 0.3f};  // Red tint overlay
    
    // They should have different component bits
    uint64_t spriteBit = getComponentBit<Sprite>();
    uint64_t renderableBit = getComponentBit<Renderable>();
    
    EXPECT_NE(spriteBit, renderableBit);
    
    // Both components should maintain their values independently
    EXPECT_EQ(sprite.textureId, 5);
    EXPECT_FLOAT_EQ(sprite.width, 64.0f);
    EXPECT_FLOAT_EQ(sprite.height, 64.0f);
    
    EXPECT_FLOAT_EQ(overlay.width, 64.0f);
    EXPECT_FLOAT_EQ(overlay.height, 64.0f);
    EXPECT_FLOAT_EQ(overlay.red, 1.0f);
    EXPECT_FLOAT_EQ(overlay.alpha, 0.3f);
}