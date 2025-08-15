#pragma once

#include "../../include/EntityManager.hpp"
#include "../../include/ComponentRegistry.hpp"
#include <vector>
#include <functional>

namespace ECS {

/**
 * SystemUtils - Utility functions for system implementations
 * 
 * Provides common patterns and helpers that systems can use to
 * query entities, iterate over components, and perform other
 * common ECS operations.
 */
namespace SystemUtils {

/**
 * Execute a function for each entity that has the required components
 * @param entityManager The entity manager to query
 * @param requiredComponents Bitmask of required components
 * @param processor Function to call for each matching entity
 */
template<typename Processor>
void forEachEntity(EntityManager& entityManager, 
                   uint64_t requiredComponents, 
                   Processor&& processor) {
    std::vector<Entity> entities = entityManager.getAllEntitiesForIteration();
    
    for (const Entity& entity : entities) {
        if (entityManager.isValid(entity) && 
            (entity.componentMask & requiredComponents) == requiredComponents) {
            processor(entity);
        }
    }
}

/**
 * Execute a function for each entity that has the required components (with entity reference)
 * Useful when the processor needs to modify the entity
 * @param entityManager The entity manager to query
 * @param requiredComponents Bitmask of required components
 * @param processor Function to call for each matching entity
 */
template<typename Processor>
void forEachEntityRef(EntityManager& entityManager, 
                      uint64_t requiredComponents, 
                      Processor&& processor) {
    std::vector<Entity> entities = entityManager.getAllEntitiesForIteration();
    
    for (const Entity& entity : entities) {
        if (entityManager.isValid(entity) && 
            (entity.componentMask & requiredComponents) == requiredComponents) {
            // Get a reference to the actual stored entity, not the copy
            Entity* storedEntity = entityManager.getEntityByID(entity.id);
            if (storedEntity) {
                processor(*storedEntity);
            }
        }
    }
}

/**
 * Count entities that have the required components
 * @param entityManager The entity manager to query
 * @param requiredComponents Bitmask of required components
 * @return Number of matching entities
 */
inline size_t countEntitiesWithComponents(EntityManager& entityManager, 
                                          uint64_t requiredComponents) {
    size_t count = 0;
    forEachEntity(entityManager, requiredComponents, [&count](const Entity&) {
        count++;
    });
    return count;
}

/**
 * Check if any entities exist with the required components
 * @param entityManager The entity manager to query
 * @param requiredComponents Bitmask of required components
 * @return true if at least one matching entity exists
 */
inline bool hasEntitiesWithComponents(EntityManager& entityManager, 
                                      uint64_t requiredComponents) {
    std::vector<Entity> entities = entityManager.getAllEntitiesForIteration();
    
    for (const Entity& entity : entities) {
        if (entityManager.isValid(entity) && 
            (entity.componentMask & requiredComponents) == requiredComponents) {
            return true;
        }
    }
    return false;
}

/**
 * Find the first entity that has the required components
 * @param entityManager The entity manager to query
 * @param requiredComponents Bitmask of required components
 * @return Pointer to first matching entity, or nullptr if none found
 */
inline Entity* findFirstEntityWithComponents(EntityManager& entityManager, 
                                             uint64_t requiredComponents) {
    std::vector<Entity> entities = entityManager.getAllEntitiesForIteration();
    
    for (Entity& entity : entities) {
        if (entityManager.isValid(entity) && 
            (entity.componentMask & requiredComponents) == requiredComponents) {
            return &entity;
        }
    }
    return nullptr;
}

/**
 * Create a component bitmask for a single component type
 * @return Component bitmask for the specified type
 */
template<typename Component>
inline uint64_t getRequiredMask() {
    return getComponentBit<Component>();
}

/**
 * Create a component bitmask for multiple component types
 * @return Combined component bitmask for all specified types
 */
template<typename Component1, typename Component2, typename... Components>
inline uint64_t getRequiredMask() {
    return getComponentBit<Component1>() | getRequiredMask<Component2, Components...>();
}

} // namespace SystemUtils
} // namespace ECS