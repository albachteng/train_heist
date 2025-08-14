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
    explicit Entity(EntityID entityId);
    Entity(EntityID entityId, uint32_t gen);
    
    bool isValid() const;
    bool hasComponent(uint64_t componentBit) const;
    bool hasComponents(uint64_t requiredMask) const;
    void addComponent(uint64_t componentBit);
    void removeComponent(uint64_t componentBit);
    
    bool operator==(const Entity& other) const;
    bool operator!=(const Entity& other) const;
};

struct EntityHandle {
    EntityID id = INVALID_ENTITY;
    uint32_t generation = 0;
    
    EntityHandle() = default;
    EntityHandle(EntityID entityId, uint32_t gen);
    explicit EntityHandle(const Entity& entity);
    
    bool isValid() const;
    bool operator==(const EntityHandle& other) const;
    bool operator!=(const EntityHandle& other) const;
};

} // namespace ECS