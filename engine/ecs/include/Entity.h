#pragma once

#include <cstdint>

namespace ECS {

using EntityID = uint32_t;

constexpr EntityID INVALID_ENTITY = 0;

struct Entity {
    EntityID id = INVALID_ENTITY;
    uint32_t generation = 0;
    uint64_t componentMask = 0;
    
    Entity() = default;
    
    explicit Entity(EntityID entityId) 
        : id(entityId), generation(0), componentMask(0) {}
    
    Entity(EntityID entityId, uint32_t gen) 
        : id(entityId), generation(gen), componentMask(0) {}
    
    bool isValid() const {
        return id != INVALID_ENTITY;
    }
    
    bool hasComponent(uint64_t componentBit) const {
        return (componentMask & componentBit) != 0;
    }
    
    bool hasComponents(uint64_t requiredMask) const {
        return (componentMask & requiredMask) == requiredMask;
    }
    
    void addComponent(uint64_t componentBit) {
        componentMask |= componentBit;
    }
    
    void removeComponent(uint64_t componentBit) {
        componentMask &= ~componentBit;
    }
    
    bool operator==(const Entity& other) const {
        return id == other.id && generation == other.generation;
    }
    
    bool operator!=(const Entity& other) const {
        return !(*this == other);
    }
};

struct EntityHandle {
    EntityID id = INVALID_ENTITY;
    uint32_t generation = 0;
    
    EntityHandle() = default;
    
    EntityHandle(EntityID entityId, uint32_t gen) 
        : id(entityId), generation(gen) {}
    
    explicit EntityHandle(const Entity& entity) 
        : id(entity.id), generation(entity.generation) {}
    
    bool isValid() const {
        return id != INVALID_ENTITY;
    }
    
    bool operator==(const EntityHandle& other) const {
        return id == other.id && generation == other.generation;
    }
    
    bool operator!=(const EntityHandle& other) const {
        return !(*this == other);
    }
};

} // namespace ECS