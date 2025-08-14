#include "../include/ComponentArray.hpp"
#include <gtest/gtest.h>

using namespace ECS;

// Test component structures
struct Position {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct Velocity {
    float dx = 0.0f;
    float dy = 0.0f;
    
    bool operator==(const Velocity& other) const {
        return dx == other.dx && dy == other.dy;
    }
};

class ComponentArrayTest : public ::testing::Test {
protected:
    void SetUp() override {
        entity1 = Entity(1, 0);
        entity2 = Entity(2, 0); 
        entity3 = Entity(3, 0);
        
        positionBit = 1ULL << 0;
        velocityBit = 1ULL << 1;
    }
    
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
    
    positions.add(entity1.id, pos, positionBit, entity1);
    
    EXPECT_FALSE(positions.empty());
    EXPECT_EQ(positions.size(), 1);
    EXPECT_TRUE(positions.has(entity1.id));
    EXPECT_TRUE(entity1.hasComponent(positionBit));
    
    Position* retrieved = positions.get(entity1.id);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(*retrieved, pos);
}

// Test adding multiple components
TEST_F(ComponentArrayTest, AddMultipleComponents) {
    ComponentArray<Position> positions;
    Position pos1{1.0f, 2.0f, 3.0f};
    Position pos2{4.0f, 5.0f, 6.0f};
    Position pos3{7.0f, 8.0f, 9.0f};
    
    positions.add(entity1.id, pos1, positionBit, entity1);
    positions.add(entity2.id, pos2, positionBit, entity2);
    positions.add(entity3.id, pos3, positionBit, entity3);
    
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
    
    positions.add(entity1.id, pos1, positionBit, entity1);
    EXPECT_EQ(*positions.get(entity1.id), pos1);
    EXPECT_EQ(positions.size(), 1);
    
    // Adding again should update, not duplicate
    positions.add(entity1.id, pos2, positionBit, entity1);
    EXPECT_EQ(*positions.get(entity1.id), pos2);
    EXPECT_EQ(positions.size(), 1);
}

// Test const get method
TEST_F(ComponentArrayTest, ConstGet) {
    ComponentArray<Position> positions;
    Position pos{1.0f, 2.0f, 3.0f};
    
    positions.add(entity1.id, pos, positionBit, entity1);
    
    const ComponentArray<Position>& constPositions = positions;
    const Position* retrieved = constPositions.get(entity1.id);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(*retrieved, pos);
    
    // Test non-existent entity
    const Position* notFound = constPositions.get(999);
    EXPECT_EQ(notFound, nullptr);
}

// Test removing components
TEST_F(ComponentArrayTest, RemoveComponent) {
    ComponentArray<Position> positions;
    Position pos{1.0f, 2.0f, 3.0f};
    
    positions.add(entity1.id, pos, positionBit, entity1);
    EXPECT_TRUE(positions.has(entity1.id));
    EXPECT_TRUE(entity1.hasComponent(positionBit));
    
    positions.remove(entity1.id, positionBit, entity1);
    
    EXPECT_FALSE(positions.has(entity1.id));
    EXPECT_FALSE(entity1.hasComponent(positionBit));
    EXPECT_TRUE(positions.empty());
    EXPECT_EQ(positions.get(entity1.id), nullptr);
}

// Test swap-remove behavior (dense array maintenance)
TEST_F(ComponentArrayTest, SwapRemoveBehavior) {
    ComponentArray<Position> positions;
    Position pos1{1.0f, 2.0f, 3.0f};
    Position pos2{4.0f, 5.0f, 6.0f};
    Position pos3{7.0f, 8.0f, 9.0f};
    
    positions.add(entity1.id, pos1, positionBit, entity1);
    positions.add(entity2.id, pos2, positionBit, entity2);
    positions.add(entity3.id, pos3, positionBit, entity3);
    
    // Remove middle element (entity2)
    positions.remove(entity2.id, positionBit, entity2);
    
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
    positions.remove(entity1.id, positionBit, entity1);
    
    EXPECT_TRUE(positions.empty());
    EXPECT_FALSE(entity1.hasComponent(positionBit));
}

// Test clear functionality
TEST_F(ComponentArrayTest, ClearArray) {
    ComponentArray<Position> positions;
    Position pos1{1.0f, 2.0f, 3.0f};
    Position pos2{4.0f, 5.0f, 6.0f};
    
    positions.add(entity1.id, pos1, positionBit, entity1);
    positions.add(entity2.id, pos2, positionBit, entity2);
    
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
    positions.add(entity1.id, pos, positionBit, entity1);
    EXPECT_EQ(positions.size(), 1);
    EXPECT_EQ(*positions.get(entity1.id), pos);
}

// Test index-based access
TEST_F(ComponentArrayTest, IndexBasedAccess) {
    ComponentArray<Position> positions;
    Position pos1{1.0f, 2.0f, 3.0f};
    Position pos2{4.0f, 5.0f, 6.0f};
    
    positions.add(entity1.id, pos1, positionBit, entity1);
    positions.add(entity2.id, pos2, positionBit, entity2);
    
    EXPECT_EQ(positions.getByIndex(0), pos1);
    EXPECT_EQ(positions.getByIndex(1), pos2);
    EXPECT_EQ(positions.getEntityByIndex(0), entity1.id);
    EXPECT_EQ(positions.getEntityByIndex(1), entity2.id);
    
    // Test const version
    const ComponentArray<Position>& constPositions = positions;
    EXPECT_EQ(constPositions.getByIndex(0), pos1);
    EXPECT_EQ(constPositions.getByIndex(1), pos2);
}

// Test iteration support
TEST_F(ComponentArrayTest, IterationSupport) {
    ComponentArray<Position> positions;
    Position pos1{1.0f, 2.0f, 3.0f};
    Position pos2{4.0f, 5.0f, 6.0f};
    
    positions.add(entity1.id, pos1, positionBit, entity1);
    positions.add(entity2.id, pos2, positionBit, entity2);
    
    const auto& components = positions.getComponents();
    const auto& entityIDs = positions.getEntityIDs();
    
    EXPECT_EQ(components.size(), 2);
    EXPECT_EQ(entityIDs.size(), 2);
    
    // Check that components and entity IDs are properly aligned
    for (size_t i = 0; i < components.size(); ++i) {
        EntityID entityId = entityIDs[i];
        const Position* stored = positions.get(entityId);
        EXPECT_EQ(components[i], *stored);
    }
}

// Test with different component type
TEST_F(ComponentArrayTest, DifferentComponentType) {
    ComponentArray<Velocity> velocities;
    Velocity vel{5.0f, -3.0f};
    
    velocities.add(entity1.id, vel, velocityBit, entity1);
    
    EXPECT_TRUE(velocities.has(entity1.id));
    EXPECT_TRUE(entity1.hasComponent(velocityBit));
    EXPECT_EQ(*velocities.get(entity1.id), vel);
}

// Test entity with multiple component types
TEST_F(ComponentArrayTest, MultipleComponentTypes) {
    ComponentArray<Position> positions;
    ComponentArray<Velocity> velocities;
    
    Position pos{1.0f, 2.0f, 3.0f};
    Velocity vel{5.0f, -3.0f};
    
    positions.add(entity1.id, pos, positionBit, entity1);
    velocities.add(entity1.id, vel, velocityBit, entity1);
    
    EXPECT_TRUE(entity1.hasComponent(positionBit));
    EXPECT_TRUE(entity1.hasComponent(velocityBit));
    EXPECT_TRUE(entity1.hasComponents(positionBit | velocityBit));
    
    EXPECT_EQ(*positions.get(entity1.id), pos);
    EXPECT_EQ(*velocities.get(entity1.id), vel);
}