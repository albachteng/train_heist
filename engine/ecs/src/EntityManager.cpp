#include "../include/EntityManager.hpp"
#include <algorithm>

namespace ECS {

Entity EntityManager::createEntity() {
    Entity entity;
    entity.componentMask = 0;
    
    // Ensure we have at least one slot (index 0 reserved for INVALID_ENTITY)
    if (entities.empty()) {
        entities.push_back(Entity{}); // Index 0 - reserved, never used
        generations.push_back(0);
        alive.push_back(false);
    }
    
    if (!freeIds.empty()) {
        // Reuse a dead entity slot
        entity.id = freeIds.front();
        freeIds.pop();
        
        // Increment generation for reused ID
        generations[entity.id]++;
        entity.generation = generations[entity.id];
        
        // Update the entity in storage and mark as alive
        entities[entity.id] = entity;
        alive[entity.id] = true;
    } else {
        // Create new entity with fresh ID
        entity.id = nextId++;
        entity.generation = 0;
        
        // Expand storage to accommodate new ID
        while (entities.size() <= entity.id) {
            entities.push_back(Entity{});
            generations.push_back(0);
            alive.push_back(false);
        }
        
        // Set generation and store entity
        generations[entity.id] = entity.generation;
        entities[entity.id] = entity;
        alive[entity.id] = true;
    }
    
    return entity;
}

void EntityManager::destroyEntity(const Entity& entity) {
    // Check if entity is valid and within bounds
    if (entity.id == INVALID_ENTITY || entity.id >= entities.size()) {
        return;
    }
    
    // Check if this is the current generation
    if (generations[entity.id] != entity.generation || !alive[entity.id]) {
        return;
    }
    
    // Mark entity as dead but keep it in storage
    alive[entity.id] = false;
    freeIds.push(entity.id);
    
    // Clear component mask for the dead entity
    entities[entity.id].componentMask = 0;
}

bool EntityManager::isValid(const Entity& entity) const {
    if (entity.id == INVALID_ENTITY || entity.id >= entities.size()) {
        return false;
    }
    
    // Check if entity is alive and has matching generation
    return alive[entity.id] && generations[entity.id] == entity.generation;
}

bool EntityManager::isValid(const EntityHandle& handle) const {
    if (handle.id == INVALID_ENTITY || handle.id >= entities.size()) {
        return false;
    }
    
    // Check if entity is alive and has matching generation
    return alive[handle.id] && generations[handle.id] == handle.generation;
}

EntityHandle EntityManager::createHandle(const Entity& entity) const {
    return EntityHandle(entity.id, entity.generation);
}

Entity* EntityManager::getEntity(const EntityHandle& handle) {
    if (!isValid(handle)) {
        return nullptr;
    }
    
    return &entities[handle.id];
}

const Entity* EntityManager::getEntity(const EntityHandle& handle) const {
    if (!isValid(handle)) {
        return nullptr;
    }
    
    return &entities[handle.id];
}

size_t EntityManager::getActiveEntityCount() const {
    size_t count = 0;
    for (size_t i = 0; i < alive.size(); ++i) {
        if (alive[i]) {
            count++;
        }
    }
    return count;
}

size_t EntityManager::getTotalEntityCount() const {
    // Return total entities ever created (excluding index 0 which is reserved)
    return entities.size() > 0 ? entities.size() - 1 : 0;
}

size_t EntityManager::getDeadEntityCount() const {
    return freeIds.size();
}

bool EntityManager::isAlive(EntityID entityId) const {
    if (entityId == INVALID_ENTITY || entityId >= alive.size()) {
        return false;
    }
    
    return alive[entityId];
}

Entity* EntityManager::getEntityByID(EntityID entityId) {
    if (!isAlive(entityId)) {
        return nullptr;
    }
    
    return &entities[entityId];
}

const Entity* EntityManager::getEntityByID(EntityID entityId) const {
    if (!isAlive(entityId)) {
        return nullptr;
    }
    
    return &entities[entityId];
}

std::vector<const Entity*> EntityManager::getAllEntitiesForIteration() const {
    // Return pointers to entities excluding index 0 (reserved for INVALID_ENTITY)
    std::vector<const Entity*> result;
    if (entities.size() <= 1) {
        return result;
    }

    result.reserve(entities.size() - 1);
    for (size_t i = 1; i < entities.size(); ++i) {
        result.push_back(&entities[i]);
    }

    return result;
}

void EntityManager::clear() {
    entities.clear();
    generations.clear();
    alive.clear();
    while (!freeIds.empty()) {
        freeIds.pop();
    }
    nextId = 1;
}

} // namespace ECS