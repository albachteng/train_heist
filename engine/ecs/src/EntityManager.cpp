#include "../include/EntityManager.hpp"
#include <algorithm>

namespace ECS {

Entity EntityManager::createEntity() {
    Entity entity;
    entity.componentMask = 0;
    
    if (!freeIds.empty()) {
        // Reuse a freed entity ID
        entity.id = freeIds.front();
        freeIds.pop();
        
        // Make sure we have space for this ID in generations array
        while (entity.id >= generations.size()) {
            generations.push_back(0);
        }
        
        // Increment generation for reused ID
        generations[entity.id]++;
        entity.generation = generations[entity.id];
    } else {
        // Create new entity with fresh ID
        entity.id = nextId++;
        
        // Make sure we have space for this ID in generations array
        while (entity.id >= generations.size()) {
            generations.push_back(0);
        }
        
        entity.generation = generations[entity.id]; // Should be 0 for new IDs
    }
    
    entities.push_back(entity);
    return entity;
}

void EntityManager::destroyEntity(const Entity& entity) {
    // Find the entity in our storage
    auto it = std::find_if(entities.begin(), entities.end(),
                          [entity](const Entity& e) { 
                              return e.id == entity.id && e.generation == entity.generation; 
                          });
    
    if (it != entities.end()) {
        // Mark ID for reuse
        freeIds.push(entity.id);
        
        // Remove the entity from active entities
        entities.erase(it);
    }
}

bool EntityManager::isValid(const Entity& entity) const {
    if (entity.id == INVALID_ENTITY) {
        return false;
    }
    
    // Find entity with matching ID and generation
    auto it = std::find_if(entities.begin(), entities.end(),
                          [entity](const Entity& e) { 
                              return e.id == entity.id && e.generation == entity.generation; 
                          });
    
    return it != entities.end();
}

bool EntityManager::isValid(const EntityHandle& handle) const {
    if (handle.id == INVALID_ENTITY) {
        return false;
    }
    
    // Check if there's a valid entity with matching ID and generation
    auto it = std::find_if(entities.begin(), entities.end(),
                          [handle](const Entity& e) { 
                              return e.id == handle.id && e.generation == handle.generation; 
                          });
    
    return it != entities.end();
}

EntityHandle EntityManager::createHandle(const Entity& entity) const {
    return EntityHandle(entity.id, entity.generation);
}

Entity* EntityManager::getEntity(const EntityHandle& handle) {
    if (!isValid(handle)) {
        return nullptr;
    }
    
    auto it = std::find_if(entities.begin(), entities.end(),
                          [handle](const Entity& e) { 
                              return e.id == handle.id && e.generation == handle.generation; 
                          });
    
    return it != entities.end() ? &(*it) : nullptr;
}

const Entity* EntityManager::getEntity(const EntityHandle& handle) const {
    if (!isValid(handle)) {
        return nullptr;
    }
    
    auto it = std::find_if(entities.begin(), entities.end(),
                          [handle](const Entity& e) { 
                              return e.id == handle.id && e.generation == handle.generation; 
                          });
    
    return it != entities.end() ? &(*it) : nullptr;
}

size_t EntityManager::getActiveEntityCount() const {
    return entities.size();
}

void EntityManager::clear() {
    entities.clear();
    generations.clear();
    while (!freeIds.empty()) {
        freeIds.pop();
    }
    nextId = 1;
}

} // namespace ECS