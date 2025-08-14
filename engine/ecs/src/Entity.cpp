#include "../include/Entity.h"

namespace ECS {

// Entity constructors
Entity::Entity(EntityID entityId) 
    : id(entityId), generation(0), componentMask(0) {
}

Entity::Entity(EntityID entityId, uint32_t gen) 
    : id(entityId), generation(gen), componentMask(0) {
}

// Entity methods
bool Entity::isValid() const {
    return id != INVALID_ENTITY;
}

bool Entity::hasComponent(uint64_t componentBit) const {
    return (componentMask & componentBit) != 0;
}

bool Entity::hasComponents(uint64_t requiredMask) const {
    return (componentMask & requiredMask) == requiredMask;
}

void Entity::addComponent(uint64_t componentBit) {
    componentMask |= componentBit;
}

void Entity::removeComponent(uint64_t componentBit) {
    componentMask &= ~componentBit;
}

bool Entity::operator==(const Entity& other) const {
    return id == other.id && generation == other.generation;
}

bool Entity::operator!=(const Entity& other) const {
    return !(*this == other);
}

// EntityHandle constructors
EntityHandle::EntityHandle(EntityID entityId, uint32_t gen) 
    : id(entityId), generation(gen) {
}

EntityHandle::EntityHandle(const Entity& entity) 
    : id(entity.id), generation(entity.generation) {
}

// EntityHandle methods
bool EntityHandle::isValid() const {
    return id != INVALID_ENTITY;
}

bool EntityHandle::operator==(const EntityHandle& other) const {
    return id == other.id && generation == other.generation;
}

bool EntityHandle::operator!=(const EntityHandle& other) const {
    return !(*this == other);
}

} // namespace ECS