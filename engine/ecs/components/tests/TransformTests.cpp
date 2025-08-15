#include "../include/Transform.hpp"
#include "../include/TransformUtils.hpp"
#include "../../include/ComponentRegistry.hpp"
#include <gtest/gtest.h>
#include <cmath>

using namespace ECS;

class TransformTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Component registry automatically handles component registration
    }

    void TearDown() override {
        // No cleanup needed for component registry
    }
};

// Test Position component
TEST_F(TransformTest, PositionDefaultConstruction) {
    Position pos;
    
    EXPECT_FLOAT_EQ(pos.x, 0.0f);
    EXPECT_FLOAT_EQ(pos.y, 0.0f);
    EXPECT_FLOAT_EQ(pos.z, 0.0f);
}

TEST_F(TransformTest, PositionValueConstruction) {
    Position pos{1.5f, 2.5f, 3.5f};
    
    EXPECT_FLOAT_EQ(pos.x, 1.5f);
    EXPECT_FLOAT_EQ(pos.y, 2.5f);
    EXPECT_FLOAT_EQ(pos.z, 3.5f);
}

TEST_F(TransformTest, PositionEquality) {
    Position pos1{1.0f, 2.0f, 3.0f};
    Position pos2{1.0f, 2.0f, 3.0f};
    Position pos3{1.0f, 2.0f, 4.0f};
    
    EXPECT_EQ(pos1, pos2);
    EXPECT_NE(pos1, pos3);
}

// Test Rotation component
TEST_F(TransformTest, RotationDefaultConstruction) {
    Rotation rot;
    
    EXPECT_FLOAT_EQ(rot.angle, 0.0f);
}

TEST_F(TransformTest, RotationValueConstruction) {
    Rotation rot{1.57f}; // ~90 degrees
    
    EXPECT_FLOAT_EQ(rot.angle, 1.57f);
}

TEST_F(TransformTest, RotationEquality) {
    Rotation rot1{1.57f};
    Rotation rot2{1.57f};
    Rotation rot3{3.14f};
    
    EXPECT_EQ(rot1, rot2);
    EXPECT_NE(rot1, rot3);
}

// Test Scale component
TEST_F(TransformTest, ScaleDefaultConstruction) {
    Scale scale;
    
    EXPECT_FLOAT_EQ(scale.x, 1.0f);
    EXPECT_FLOAT_EQ(scale.y, 1.0f);
}

TEST_F(TransformTest, ScaleValueConstruction) {
    Scale scale{2.0f, 1.5f};
    
    EXPECT_FLOAT_EQ(scale.x, 2.0f);
    EXPECT_FLOAT_EQ(scale.y, 1.5f);
}

TEST_F(TransformTest, ScaleEquality) {
    Scale scale1{2.0f, 1.5f};
    Scale scale2{2.0f, 1.5f};
    Scale scale3{1.0f, 1.0f};
    
    EXPECT_EQ(scale1, scale2);
    EXPECT_NE(scale1, scale3);
}

// Test GridPosition component
TEST_F(TransformTest, GridPositionDefaultConstruction) {
    GridPosition grid;
    
    EXPECT_EQ(grid.x, 0);
    EXPECT_EQ(grid.y, 0);
}

TEST_F(TransformTest, GridPositionValueConstruction) {
    GridPosition grid{5, 3};
    
    EXPECT_EQ(grid.x, 5);
    EXPECT_EQ(grid.y, 3);
}

TEST_F(TransformTest, GridPositionEquality) {
    GridPosition grid1{5, 3};
    GridPosition grid2{5, 3};
    GridPosition grid3{2, 1};
    
    EXPECT_EQ(grid1, grid2);
    EXPECT_NE(grid1, grid3);
}

// Test component registry integration
TEST_F(TransformTest, ComponentRegistryIntegration) {
    uint64_t posBit = getComponentBit<Position>();
    uint64_t rotBit = getComponentBit<Rotation>();
    uint64_t scaleBit = getComponentBit<Scale>();
    uint64_t gridBit = getComponentBit<GridPosition>();
    
    // Each component should have a unique bit
    EXPECT_NE(posBit, 0);
    EXPECT_NE(rotBit, 0);
    EXPECT_NE(scaleBit, 0);
    EXPECT_NE(gridBit, 0);
    
    // All bits should be different
    EXPECT_NE(posBit, rotBit);
    EXPECT_NE(posBit, scaleBit);
    EXPECT_NE(posBit, gridBit);
    EXPECT_NE(rotBit, scaleBit);
    EXPECT_NE(rotBit, gridBit);
    EXPECT_NE(scaleBit, gridBit);
    
    // Calling again should return the same bits (consistency)
    EXPECT_EQ(posBit, getComponentBit<Position>());
    EXPECT_EQ(rotBit, getComponentBit<Rotation>());
    EXPECT_EQ(scaleBit, getComponentBit<Scale>());
    EXPECT_EQ(gridBit, getComponentBit<GridPosition>());
}

// Test coordinate system conversions
TEST_F(TransformTest, GridToWorldConversion) {
    GridPosition grid{3, 2};
    Position world = TransformUtils::gridToWorld(grid, 32.0f, 24.0f);
    
    // Should be centered on the tile
    EXPECT_FLOAT_EQ(world.x, 3.0f * 32.0f + 16.0f); // Centered in tile
    EXPECT_FLOAT_EQ(world.y, 2.0f * 24.0f + 12.0f); // Centered in tile
    EXPECT_FLOAT_EQ(world.z, 0.0f);
}

TEST_F(TransformTest, WorldToGridConversion) {
    Position world{100.0f, 75.0f, 0.0f};
    GridPosition grid = TransformUtils::worldToGrid(world, 32.0f, 24.0f);
    
    EXPECT_EQ(grid.x, 3); // 100 / 32 = 3.125 -> 3
    EXPECT_EQ(grid.y, 3); // 75 / 24 = 3.125 -> 3
}

TEST_F(TransformTest, GridToIsometricConversion) {
    GridPosition grid{2, 1};
    Position iso = TransformUtils::gridToIsometric(grid, 64.0f, 32.0f);
    
    // Isometric projection: screen_x = (grid_x - grid_y) * tile_width/2
    // screen_y = (grid_x + grid_y) * tile_height/2
    float expectedX = (2 - 1) * 32.0f; // (2-1) * 64/2 = 32
    float expectedY = (2 + 1) * 16.0f; // (2+1) * 32/2 = 48
    
    EXPECT_FLOAT_EQ(iso.x, expectedX);
    EXPECT_FLOAT_EQ(iso.y, expectedY);
}

// Test distance calculations
TEST_F(TransformTest, DistanceCalculation) {
    Position pos1{0.0f, 0.0f, 0.0f};
    Position pos2{3.0f, 4.0f, 0.0f};
    
    float distance = TransformUtils::distance(pos1, pos2);
    EXPECT_FLOAT_EQ(distance, 5.0f); // 3-4-5 triangle
}

TEST_F(TransformTest, DistanceSquaredCalculation) {
    Position pos1{0.0f, 0.0f, 0.0f};
    Position pos2{3.0f, 4.0f, 0.0f};
    
    float distSq = TransformUtils::distanceSquared(pos1, pos2);
    EXPECT_FLOAT_EQ(distSq, 25.0f); // 3² + 4² = 25
}

TEST_F(TransformTest, ManhattanDistance) {
    GridPosition grid1{2, 3};
    GridPosition grid2{5, 7};
    
    int dist = TransformUtils::manhattanDistance(grid1, grid2);
    EXPECT_EQ(dist, 7); // |5-2| + |7-3| = 3 + 4 = 7
}

// Test angle utilities
TEST_F(TransformTest, AngleNormalization) {
    float angle1 = TransformUtils::normalizeAngle(7.0f); // > 2π
    float angle2 = TransformUtils::normalizeAngle(-1.0f); // negative
    
    EXPECT_GE(angle1, 0.0f);
    EXPECT_LT(angle1, 2.0f * 3.14159265359f);
    EXPECT_GE(angle2, 0.0f);
    EXPECT_LT(angle2, 2.0f * 3.14159265359f);
}

TEST_F(TransformTest, DegreesRadiansConversion) {
    float radians = TransformUtils::degreesToRadians(90.0f);
    float degrees = TransformUtils::radiansToDegrees(radians);
    
    EXPECT_NEAR(radians, 3.14159265359f / 2.0f, 0.001f);
    EXPECT_NEAR(degrees, 90.0f, 0.001f);
}

// Test scale utilities
TEST_F(TransformTest, UniformScaling) {
    Scale scale{2.0f, 3.0f};
    TransformUtils::applyUniformScale(scale, 1.5f);
    
    EXPECT_FLOAT_EQ(scale.x, 3.0f); // 2.0 * 1.5
    EXPECT_FLOAT_EQ(scale.y, 4.5f); // 3.0 * 1.5
}

// Test interpolation
TEST_F(TransformTest, LinearInterpolation) {
    Position start{0.0f, 0.0f, 0.0f};
    Position end{10.0f, 20.0f, 30.0f};
    
    Position mid = TransformUtils::lerp(start, end, 0.5f);
    EXPECT_FLOAT_EQ(mid.x, 5.0f);
    EXPECT_FLOAT_EQ(mid.y, 10.0f);
    EXPECT_FLOAT_EQ(mid.z, 15.0f);
    
    Position quarter = TransformUtils::lerp(start, end, 0.25f);
    EXPECT_FLOAT_EQ(quarter.x, 2.5f);
    EXPECT_FLOAT_EQ(quarter.y, 5.0f);
    EXPECT_FLOAT_EQ(quarter.z, 7.5f);
}

// Test approximate equality
TEST_F(TransformTest, ApproximateEquality) {
    Position pos1{1.0f, 2.0f, 3.0f};
    Position pos2{1.0001f, 2.0001f, 3.0001f};
    Position pos3{1.1f, 2.1f, 3.1f};
    
    EXPECT_TRUE(TransformUtils::approximately(pos1, pos2, 0.001f));
    EXPECT_FALSE(TransformUtils::approximately(pos1, pos3, 0.001f));
}

// Test ZII compliance (Zero-Initialized Invariant)
TEST_F(TransformTest, ZIICompliance) {
    // Test that default-constructed components have expected values
    Position pos;
    Rotation rot;
    Scale scale;
    GridPosition grid;
    
    // Position and Rotation should be zero-initialized
    EXPECT_FLOAT_EQ(pos.x, 0.0f);
    EXPECT_FLOAT_EQ(pos.y, 0.0f);
    EXPECT_FLOAT_EQ(pos.z, 0.0f);
    EXPECT_FLOAT_EQ(rot.angle, 0.0f);
    
    // Scale should default to 1.0 (visible) not 0.0 (invisible)
    EXPECT_FLOAT_EQ(scale.x, 1.0f);
    EXPECT_FLOAT_EQ(scale.y, 1.0f);
    
    // GridPosition should be zero-initialized
    EXPECT_EQ(grid.x, 0);
    EXPECT_EQ(grid.y, 0);
}

// Test component combinations for common patterns
TEST_F(TransformTest, CommonComponentCombinations) {
    // Test common transform component combinations
    uint64_t positionBit = getComponentBit<Position>();
    uint64_t rotationBit = getComponentBit<Rotation>();
    uint64_t scaleBit = getComponentBit<Scale>();
    uint64_t gridBit = getComponentBit<GridPosition>();
    
    // Common combinations
    uint64_t staticSprite = positionBit | scaleBit;
    uint64_t rotatingSprite = positionBit | rotationBit | scaleBit;
    uint64_t gridEntity = gridBit | positionBit;
    uint64_t fullTransform = positionBit | rotationBit | scaleBit | gridBit;
    
    // Verify combinations have expected bits set
    EXPECT_EQ(staticSprite & positionBit, positionBit);
    EXPECT_EQ(staticSprite & scaleBit, scaleBit);
    EXPECT_EQ(staticSprite & rotationBit, 0); // Should not have rotation
    
    EXPECT_EQ(rotatingSprite & positionBit, positionBit);
    EXPECT_EQ(rotatingSprite & rotationBit, rotationBit);
    EXPECT_EQ(rotatingSprite & scaleBit, scaleBit);
    
    EXPECT_EQ(gridEntity & gridBit, gridBit);
    EXPECT_EQ(gridEntity & positionBit, positionBit);
    
    EXPECT_EQ(fullTransform & positionBit, positionBit);
    EXPECT_EQ(fullTransform & rotationBit, rotationBit);
    EXPECT_EQ(fullTransform & scaleBit, scaleBit);
    EXPECT_EQ(fullTransform & gridBit, gridBit);
}