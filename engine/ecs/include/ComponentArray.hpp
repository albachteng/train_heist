#pragma once

#include "Entity.h"
#include "EntityManager.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include <type_traits>

namespace ECS {

/**
 * ComponentArray - Struct-of-Arrays storage for components
 * 
 * Stores components in cache-friendly SoA layout with dense arrays.
 * Uses swap-remove to maintain array density without fragmentation.
 * 
 * FUTURE OPTIMIZATION: Currently uses std::vector for simplicity and correctness.
 * Will be migrated to memory arenas for better performance:
 * - Single contiguous allocation instead of fragmented std::vector growth
 * - Better cache locality for component iteration  
 * - Zero malloc calls during gameplay
 * - 2-3x performance improvement for large component counts
 * 
 * The public interface is designed to support this migration without API changes.
 */
template <typename Component>
class ComponentArray {
    // ZII (Zero-is-Initialization) Compliance Enforcement
    static_assert(std::is_trivially_copyable_v<Component>, 
                  "Component types must be trivially copyable (POD structs only)");
    static_assert(std::is_default_constructible_v<Component>, 
                  "Component types must be default constructible");
    // Note: We can't directly check for ZII compliance at compile time, but we enforce
    // the prerequisites: trivial copyability and default construction. Developers must
    // ensure all members have default initialization (= 0.0f, etc.) in their component structs.

private:
    // NOTE: Currently using std::vector for simplicity. Future migration to memory arenas
    // will replace these with arena-allocated arrays for better performance.
    std::vector<Component> components;        // SoA: Component data
    std::vector<EntityID> entityIDs;          // SoA: Corresponding entity IDs  
    std::unordered_map<EntityID, size_t> entityIndex; // Fast entity->index lookup

public:
    ComponentArray() = default;
    ~ComponentArray() = default;
    
    // Non-copyable but movable
    ComponentArray(const ComponentArray&) = delete;
    ComponentArray& operator=(const ComponentArray&) = delete;
    ComponentArray(ComponentArray&&) = default;
    ComponentArray& operator=(ComponentArray&&) = default;

    // Add component to entity
    void add(EntityID entityId, const Component& component, uint64_t componentBit, EntityManager& entityManager) {
        // Check if entity already has this component
        auto it = entityIndex.find(entityId);
        if (it != entityIndex.end()) {
            // Update existing component
            components[it->second] = component;
            return;
        }

        // Add new component
        components.push_back(component);
        entityIDs.push_back(entityId);
        entityIndex[entityId] = components.size() - 1;

        // Update entity bitmask IN EntityManager's stored entity (not a local copy)
        Entity* storedEntity = entityManager.getEntityByID(entityId);
        if (storedEntity) {
            storedEntity->componentMask |= componentBit;
        }
    }
    
    // Check if entity has this component
    bool has(EntityID entityId) const {
        return entityIndex.find(entityId) != entityIndex.end();
    }
    
    // Get component for entity (returns nullptr if not found)
    Component* get(EntityID entityId) {
        auto it = entityIndex.find(entityId);
        return it != entityIndex.end() ? &components[it->second] : nullptr;
    }
    
    const Component* get(EntityID entityId) const {
        auto it = entityIndex.find(entityId);
        return it != entityIndex.end() ? &components[it->second] : nullptr;
    }
    
    // Remove component from entity
    void remove(EntityID entityId, uint64_t componentBit, EntityManager& entityManager) {
        auto it = entityIndex.find(entityId);
        if (it == entityIndex.end()) {
            return; // Entity doesn't have this component
        }

        size_t indexToRemove = it->second;
        EntityID lastEntity = entityIDs.back();

        // Swap-remove: move last element to the position being removed
        components[indexToRemove] = std::move(components.back());
        entityIDs[indexToRemove] = lastEntity;

        // Update index mapping for the moved element
        if (lastEntity != entityId) {
            entityIndex[lastEntity] = indexToRemove;
        }

        // Remove the last elements
        components.pop_back();
        entityIDs.pop_back();
        entityIndex.erase(it);

        // Clear component bit IN EntityManager's stored entity (not a local copy)
        Entity* storedEntity = entityManager.getEntityByID(entityId);
        if (storedEntity) {
            storedEntity->componentMask &= ~componentBit;
        }
    }
    
    // Get total number of components
    size_t size() const {
        return components.size();
    }
    
    // Check if array is empty
    bool empty() const {
        return components.empty();
    }
    
    // Clear all components
    void clear() {
        components.clear();
        entityIDs.clear();
        entityIndex.clear();
    }
    
    // Reserve space for components (optimization)
    void reserve(size_t capacity) {
        components.reserve(capacity);
        entityIDs.reserve(capacity);
        entityIndex.reserve(capacity);
    }
    
    // Iteration support - get component at index
    Component& getByIndex(size_t index) {
        assert(index < components.size());
        return components[index];
    }
    
    const Component& getByIndex(size_t index) const {
        assert(index < components.size());
        return components[index];
    }
    
    // Get entity ID at index
    EntityID getEntityByIndex(size_t index) const {
        assert(index < entityIDs.size());
        return entityIDs[index];
    }
    
    // Get all components (for system iteration)
    const std::vector<Component>& getComponents() const {
        return components;
    }
    
    const std::vector<EntityID>& getEntityIDs() const {
        return entityIDs;
    }
};

} // namespace ECS