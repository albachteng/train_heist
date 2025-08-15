#pragma once

#include "Entity.h"
#include <vector>
#include <queue>

namespace ECS {

/**
 * EntityManager - Core ECS entity lifecycle management
 * 
 * Provides centralized entity creation, destruction, and validation.
 * Manages entity ID allocation, generation counters, and entity reuse.
 * 
 * Key Features:
 * - Efficient entity ID reuse with generation counters
 * - Safe entity handle validation
 * - Automatic cleanup of destroyed entities
 * - Thread-safe operations (future: atomic operations)
 */
class EntityManager {
private:
    std::vector<Entity> entities;           // All entities (including dead ones) - indexed by ID
    std::vector<uint32_t> generations;      // Generation counter for each entity ID (indexed by ID) 
    std::vector<bool> alive;               // Tracks which entity slots are alive (indexed by ID)
    std::queue<EntityID> freeIds;          // Pool of reusable entity IDs
    EntityID nextId = 1;                   // Next available entity ID (0 reserved for INVALID_ENTITY)
    
public:
    EntityManager() = default;
    ~EntityManager() = default;
    
    // Non-copyable but movable
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
    EntityManager(EntityManager&&) = default;
    EntityManager& operator=(EntityManager&&) = default;
    
    /**
     * Create a new entity with unique ID and generation counter
     */
    Entity createEntity();
    
    /**
     * Destroy an entity and mark it for ID reuse
     */
    void destroyEntity(const Entity& entity);
    
    /**
     * Check if an entity is valid (exists and has correct generation)
     */
    bool isValid(const Entity& entity) const;
    
    /**
     * Check if an entity handle is valid
     */
    bool isValid(const EntityHandle& handle) const;
    
    /**
     * Create a handle from an entity for external references
     */
    EntityHandle createHandle(const Entity& entity) const;
    
    /**
     * Get entity from handle (returns nullptr if invalid)
     */
    Entity* getEntity(const EntityHandle& handle);
    const Entity* getEntity(const EntityHandle& handle) const;
    
    /**
     * Get total number of active (living) entities
     */
    size_t getActiveEntityCount() const;
    
    /**
     * Get total number of entities including dead ones (total storage used)
     */
    size_t getTotalEntityCount() const;
    
    /**
     * Get number of dead entities available for reuse
     */
    size_t getDeadEntityCount() const;
    
    /**
     * Check if an entity slot is alive (not necessarily valid - use isValid for full validation)
     */
    bool isAlive(EntityID entityId) const;
    
    /**
     * Get entity by ID (returns current generation entity or nullptr if dead)
     */
    Entity* getEntityByID(EntityID entityId);
    const Entity* getEntityByID(EntityID entityId) const;
    
    /**
     * Get all entities for system iteration (includes dead entities that systems should skip)
     */
    std::vector<Entity> getAllEntitiesForIteration() const;
    
    /**
     * Clear all entities (for testing/reset)
     */
    void clear();
};

} // namespace ECS