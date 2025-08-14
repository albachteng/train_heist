#pragma once

#include "Entity.h"
#include <vector>
#include <functional>
#include <algorithm>

namespace ECS {

/**
 * EmptyPayload - For events that don't need additional data
 * 
 * Used for simple notification events like "EntityDestroyed" or "LevelComplete"
 * where the EntityID source is sufficient information.
 */
struct EmptyPayload {
    // Intentionally empty - just a type marker
    bool operator==(const EmptyPayload&) const { return true; }
    bool operator!=(const EmptyPayload&) const { return false; }
};

/**
 * Event<T> - Strongly typed event with source entity and payload
 * 
 * Events provide decoupled communication between systems:
 * - Input system generates MoveEvent<Direction>
 * - Movement system processes events and generates SoundEvent<EmptyPayload>  
 * - Sound system processes sound events
 * 
 * The EntityID source identifies which entity triggered the event.
 * The payload T contains event-specific data with full type safety.
 */
template <typename T>
struct Event {
    EntityID source = INVALID_ENTITY;
    T payload;
    
    Event() = default;
    
    Event(EntityID sourceEntity, const T& eventPayload)
        : source(sourceEntity), payload(eventPayload) {}
    
    explicit Event(const T& eventPayload)
        : source(INVALID_ENTITY), payload(eventPayload) {}
    
    bool operator==(const Event<T>& other) const {
        return source == other.source && payload == other.payload;
    }
    
    bool operator!=(const Event<T>& other) const {
        return !(*this == other);
    }
};

/**
 * EventQueue<T> - FIFO queue for strongly typed events
 * 
 * Each event type gets its own queue for type safety and performance.
 * Systems can push events during processing and pop them in batch
 * for efficient processing.
 * 
 * Design decisions:
 * - popAll() returns by value for safety (no dangling references)
 * - clear() separate from popAll() for flexibility
 * - Non-copyable but movable for performance
 */
template <typename T>
class EventQueue {
private:
    std::vector<Event<T>> events;

public:
    EventQueue() = default;
    ~EventQueue() = default;
    
    // Non-copyable but movable
    EventQueue(const EventQueue&) = delete;
    EventQueue& operator=(const EventQueue&) = delete;
    EventQueue(EventQueue&&) = default;
    EventQueue& operator=(EventQueue&&) = default;
    
    // Add event to queue
    void push(const Event<T>& event);
    void push(EntityID source, const T& payload);
    void push(const T& payload); // For events without specific source entity
    
    // Emplace event directly (more efficient)
    template<typename... Args>
    void emplace(EntityID source, Args&&... args);
    
    template<typename... Args>  
    void emplace(Args&&... args); // For events without specific source entity
    
    // Get all events and clear queue (typical system processing pattern)
    std::vector<Event<T>> popAll();
    
    // Peek at events without removing them
    const std::vector<Event<T>>& peek() const;
    
    // Get number of pending events
    size_t size() const;
    
    // Check if queue is empty
    bool empty() const;
    
    // Clear all events without returning them
    void clear();
    
    // Reserve space for performance
    void reserve(size_t capacity);
    
    // Process events with callback function
    template<typename Func>
    void process(Func&& callback);
    
    // Process and clear events with callback
    template<typename Func>
    void processAndClear(Func&& callback);
};

// Common event type aliases for convenience
using SimpleEvent = Event<EmptyPayload>;
using SimpleEventQueue = EventQueue<EmptyPayload>;

// Template implementations (must be inline for templates)

template <typename T>
void EventQueue<T>::push(const Event<T>& event) {
    events.push_back(event);
}

template <typename T>
void EventQueue<T>::push(EntityID source, const T& payload) {
    events.emplace_back(source, payload);
}

template <typename T>
void EventQueue<T>::push(const T& payload) {
    events.emplace_back(payload);
}

template <typename T>
template<typename... Args>
void EventQueue<T>::emplace(EntityID source, Args&&... args) {
    events.emplace_back(source, T{std::forward<Args>(args)...});
}

template <typename T>
template<typename... Args>
void EventQueue<T>::emplace(Args&&... args) {
    events.emplace_back(INVALID_ENTITY, T{std::forward<Args>(args)...});
}

template <typename T>
std::vector<Event<T>> EventQueue<T>::popAll() {
    std::vector<Event<T>> result = std::move(events);
    events.clear();
    return result;
}

template <typename T>
const std::vector<Event<T>>& EventQueue<T>::peek() const {
    return events;
}

template <typename T>
size_t EventQueue<T>::size() const {
    return events.size();
}

template <typename T>
bool EventQueue<T>::empty() const {
    return events.empty();
}

template <typename T>
void EventQueue<T>::clear() {
    events.clear();
}

template <typename T>
void EventQueue<T>::reserve(size_t capacity) {
    events.reserve(capacity);
}

template <typename T>
template<typename Func>
void EventQueue<T>::process(Func&& callback) {
    for (const auto& event : events) {
        callback(event);
    }
}

template <typename T>
template<typename Func>
void EventQueue<T>::processAndClear(Func&& callback) {
    for (const auto& event : events) {
        callback(event);
    }
    events.clear();
}

} // namespace ECS