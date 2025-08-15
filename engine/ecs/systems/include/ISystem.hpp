#pragma once

#include <cstdint>
#include "../../include/EntityManager.hpp"

namespace ECS {

/**
 * ISystem - Base interface for all ECS systems
 * 
 * Systems operate on entities with specific component combinations.
 * They are stateless functions that receive injected dependencies
 * and process component data each frame.
 */
class ISystem {
public:
    virtual ~ISystem() = default;
    
    /**
     * Update system logic for one frame
     * @param deltaTime Time elapsed since last update (in seconds)
     * @param entityManager Reference to entity manager for component queries
     */
    virtual void update(float deltaTime, EntityManager& entityManager) = 0;
    
    /**
     * Get bitmask representing required components for this system
     * Systems only process entities that have ALL required components
     * @return Component bitmask (use getComponentBit<Component>() to build)
     */
    virtual uint64_t getRequiredComponents() const = 0;
    
    /**
     * Get system execution priority (lower values execute first)
     * Default: 1000 (allows insertion before/after with 0-999, 1001+)
     * @return Priority value for system scheduling
     */
    virtual int getPriority() const { return 1000; }
    
    /**
     * Check if system should be updated this frame
     * Useful for systems that run every N frames or have other conditions
     * @param deltaTime Time elapsed since last update
     * @return true if system should update, false to skip
     */
    virtual bool shouldUpdate(float deltaTime) const { 
        (void)deltaTime; // Suppress unused parameter warning
        return true; 
    }
};

} // namespace ECS