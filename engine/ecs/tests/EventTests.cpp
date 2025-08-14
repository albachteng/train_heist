#include "../include/Event.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace ECS;

// Test payload types
struct MovePayload {
    float dx = 0.0f;
    float dy = 0.0f;
    
    MovePayload() = default;
    MovePayload(float x, float y) : dx(x), dy(y) {}
    
    bool operator==(const MovePayload& other) const {
        return dx == other.dx && dy == other.dy;
    }
    
    bool operator!=(const MovePayload& other) const {
        return !(*this == other);
    }
};

struct DamagePayload {
    int amount = 0;
    EntityID attacker = INVALID_ENTITY;
    
    DamagePayload() = default;
    DamagePayload(int dmg, EntityID attackerId) : amount(dmg), attacker(attackerId) {}
    
    bool operator==(const DamagePayload& other) const {
        return amount == other.amount && attacker == other.attacker;
    }
    
    bool operator!=(const DamagePayload& other) const {
        return !(*this == other);
    }
};

class EventTest : public ::testing::Test {
protected:
    void SetUp() override {
        entity1 = 42;
        entity2 = 123;
    }
    
    EntityID entity1, entity2;
};

// Test EmptyPayload
TEST_F(EventTest, EmptyPayload) {
    EmptyPayload payload1;
    EmptyPayload payload2;
    
    EXPECT_EQ(payload1, payload2);
    EXPECT_FALSE(payload1 != payload2);
}

// Test Event default construction
TEST_F(EventTest, EventDefaultConstruction) {
    Event<EmptyPayload> event;
    
    EXPECT_EQ(event.source, INVALID_ENTITY);
    // EmptyPayload default construction should work
}

// Test Event construction with source and payload
TEST_F(EventTest, EventConstructionWithSourceAndPayload) {
    MovePayload move{5.0f, -3.0f};
    Event<MovePayload> event(entity1, move);
    
    EXPECT_EQ(event.source, entity1);
    EXPECT_EQ(event.payload, move);
}

// Test Event construction with payload only
TEST_F(EventTest, EventConstructionWithPayloadOnly) {
    MovePayload move{2.0f, 4.0f};
    Event<MovePayload> event(move);
    
    EXPECT_EQ(event.source, INVALID_ENTITY);
    EXPECT_EQ(event.payload, move);
}

// Test Event equality
TEST_F(EventTest, EventEquality) {
    MovePayload move{1.0f, 2.0f};
    Event<MovePayload> event1(entity1, move);
    Event<MovePayload> event2(entity1, move);
    Event<MovePayload> event3(entity2, move);
    Event<MovePayload> event4(entity1, MovePayload{3.0f, 4.0f});
    
    EXPECT_EQ(event1, event2);
    EXPECT_NE(event1, event3);  // Different source
    EXPECT_NE(event1, event4);  // Different payload
}

// Test EventQueue default construction
TEST_F(EventTest, EventQueueDefaultConstruction) {
    EventQueue<MovePayload> queue;
    
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);
}

// Test pushing events
TEST_F(EventTest, PushEvents) {
    EventQueue<MovePayload> queue;
    MovePayload move{1.0f, 2.0f};
    
    // Push complete event
    Event<MovePayload> event(entity1, move);
    queue.push(event);
    
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 1);
    
    // Push with source and payload
    queue.push(entity2, MovePayload{3.0f, 4.0f});
    EXPECT_EQ(queue.size(), 2);
    
    // Push with payload only
    queue.push(MovePayload{5.0f, 6.0f});
    EXPECT_EQ(queue.size(), 3);
}

// Test emplace events
TEST_F(EventTest, EmplaceEvents) {
    EventQueue<MovePayload> queue;
    
    // Emplace with source
    queue.emplace(entity1, 1.0f, 2.0f);
    EXPECT_EQ(queue.size(), 1);
    
    // Emplace without source
    queue.emplace(3.0f, 4.0f);
    EXPECT_EQ(queue.size(), 2);
    
    auto events = queue.popAll();
    EXPECT_EQ(events[0].source, entity1);
    EXPECT_EQ(events[0].payload, MovePayload(1.0f, 2.0f));
    EXPECT_EQ(events[1].source, INVALID_ENTITY);
    EXPECT_EQ(events[1].payload, MovePayload(3.0f, 4.0f));
}

// Test popAll functionality
TEST_F(EventTest, PopAll) {
    EventQueue<MovePayload> queue;
    
    queue.push(entity1, MovePayload{1.0f, 2.0f});
    queue.push(entity2, MovePayload{3.0f, 4.0f});
    
    auto events = queue.popAll();
    
    // Queue should be empty after popAll
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);
    
    // Should have received both events
    EXPECT_EQ(events.size(), 2);
    EXPECT_EQ(events[0].source, entity1);
    EXPECT_EQ(events[0].payload, MovePayload(1.0f, 2.0f));
    EXPECT_EQ(events[1].source, entity2);
    EXPECT_EQ(events[1].payload, MovePayload(3.0f, 4.0f));
}

// Test peek functionality
TEST_F(EventTest, Peek) {
    EventQueue<MovePayload> queue;
    
    queue.push(entity1, MovePayload{1.0f, 2.0f});
    queue.push(entity2, MovePayload{3.0f, 4.0f});
    
    const auto& events = queue.peek();
    
    // Queue should still have events after peek
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 2);
    
    // Should see both events
    EXPECT_EQ(events.size(), 2);
    EXPECT_EQ(events[0].source, entity1);
    EXPECT_EQ(events[0].payload, MovePayload(1.0f, 2.0f));
}

// Test clear functionality
TEST_F(EventTest, Clear) {
    EventQueue<MovePayload> queue;
    
    queue.push(entity1, MovePayload{1.0f, 2.0f});
    queue.push(entity2, MovePayload{3.0f, 4.0f});
    
    EXPECT_EQ(queue.size(), 2);
    
    queue.clear();
    
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);
}

// Test reserve functionality
TEST_F(EventTest, Reserve) {
    EventQueue<MovePayload> queue;
    
    // Should not crash or change size
    queue.reserve(100);
    EXPECT_TRUE(queue.empty());
    
    // Adding events after reserve should work normally
    queue.push(entity1, MovePayload{1.0f, 2.0f});
    EXPECT_EQ(queue.size(), 1);
}

// Test process functionality
TEST_F(EventTest, Process) {
    EventQueue<MovePayload> queue;
    
    queue.push(entity1, MovePayload{1.0f, 2.0f});
    queue.push(entity2, MovePayload{3.0f, 4.0f});
    
    int processedCount = 0;
    float totalDx = 0.0f;
    
    queue.process([&](const Event<MovePayload>& event) {
        processedCount++;
        totalDx += event.payload.dx;
    });
    
    // Events should still be in queue after process
    EXPECT_EQ(queue.size(), 2);
    EXPECT_EQ(processedCount, 2);
    EXPECT_EQ(totalDx, 4.0f); // 1.0 + 3.0
}

// Test processAndClear functionality
TEST_F(EventTest, ProcessAndClear) {
    EventQueue<MovePayload> queue;
    
    queue.push(entity1, MovePayload{1.0f, 2.0f});
    queue.push(entity2, MovePayload{3.0f, 4.0f});
    
    int processedCount = 0;
    float totalDx = 0.0f;
    
    queue.processAndClear([&](const Event<MovePayload>& event) {
        processedCount++;
        totalDx += event.payload.dx;
    });
    
    // Queue should be empty after processAndClear
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(processedCount, 2);
    EXPECT_EQ(totalDx, 4.0f);
}

// Test different payload types
TEST_F(EventTest, DifferentPayloadTypes) {
    EventQueue<DamagePayload> damageQueue;
    EventQueue<EmptyPayload> simpleQueue;
    
    // Damage events
    damageQueue.push(entity1, DamagePayload{25, entity2});
    damageQueue.push(entity2, DamagePayload{10, entity1});
    
    // Simple events
    simpleQueue.push(entity1, EmptyPayload{});
    simpleQueue.push(EmptyPayload{});
    
    EXPECT_EQ(damageQueue.size(), 2);
    EXPECT_EQ(simpleQueue.size(), 2);
    
    auto damageEvents = damageQueue.popAll();
    auto simpleEvents = simpleQueue.popAll();
    
    EXPECT_EQ(damageEvents[0].payload.amount, 25);
    EXPECT_EQ(damageEvents[0].payload.attacker, entity2);
    
    EXPECT_EQ(simpleEvents[1].source, INVALID_ENTITY);
}

// Test SimpleEvent and SimpleEventQueue aliases
TEST_F(EventTest, SimpleEventAliases) {
    SimpleEventQueue queue;
    
    queue.push(SimpleEvent{entity1, EmptyPayload{}});
    queue.push(entity2, EmptyPayload{});
    
    EXPECT_EQ(queue.size(), 2);
    
    auto events = queue.popAll();
    EXPECT_EQ(events[0].source, entity1);
    EXPECT_EQ(events[1].source, entity2);
}

// Test event queue with complex processing scenario
TEST_F(EventTest, ComplexProcessingScenario) {
    EventQueue<MovePayload> moveQueue;
    EventQueue<DamagePayload> damageQueue;
    
    // Simulate game events
    moveQueue.push(entity1, MovePayload{5.0f, 0.0f});   // Move right
    moveQueue.push(entity2, MovePayload{-3.0f, 2.0f});  // Move left-up
    damageQueue.push(entity1, DamagePayload{15, entity2}); // Take damage
    
    // Process movement
    std::vector<EntityID> movedEntities;
    moveQueue.processAndClear([&](const Event<MovePayload>& event) {
        movedEntities.push_back(event.source);
        // In real system, would update Position components here
    });
    
    // Process damage  
    std::vector<EntityID> damagedEntities;
    damageQueue.processAndClear([&](const Event<DamagePayload>& event) {
        damagedEntities.push_back(event.source);
        // In real system, would update Health components here
    });
    
    EXPECT_EQ(movedEntities.size(), 2);
    EXPECT_EQ(damagedEntities.size(), 1);
    EXPECT_TRUE(moveQueue.empty());
    EXPECT_TRUE(damageQueue.empty());
}