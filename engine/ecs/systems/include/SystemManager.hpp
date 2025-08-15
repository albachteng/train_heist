#pragma once

#include "ISystem.hpp"
#include "../../include/EntityManager.hpp"
#include <memory>
#include <vector>

namespace ECS {

/**
 * SystemManager - Manages registration and execution of ECS systems
 * 
 * Handles system lifecycle, priority-based execution ordering, and provides
 * dependency injection capabilities for systems that need external interfaces.
 * 
 * Key Features:
 * - Priority-based system execution (lower priority values execute first)
 * - Conditional system updates (systems can skip frames via shouldUpdate)
 * - Integration with EntityManager for entity filtering
 * - Dependency injection support for testability
 */
class SystemManager {
private:
    std::vector<std::unique_ptr<ISystem>> systems;
    bool systemsNeedSorting = false;

public:
    /**
     * Construct SystemManager
     */
    SystemManager() = default;
    
    ~SystemManager() = default;
    
    // Non-copyable but movable
    SystemManager(const SystemManager&) = delete;
    SystemManager& operator=(const SystemManager&) = delete;
    SystemManager(SystemManager&&) = default;
    SystemManager& operator=(SystemManager&&) = default;
    
    /**
     * Register a system for execution
     * Systems are automatically sorted by priority after registration
     * @param system Unique pointer to system implementation
     */
    void registerSystem(std::unique_ptr<ISystem> system);
    
    /**
     * Update all registered systems in priority order
     * Only systems where shouldUpdate() returns true will be executed
     * @param deltaTime Time elapsed since last update (in seconds)
     * @param entityManager Reference to entity manager for systems to use
     */
    void updateAll(float deltaTime, EntityManager& entityManager);
    
    /**
     * Remove all registered systems
     */
    void clearSystems();
    
    /**
     * Get the number of registered systems
     * @return Count of registered systems
     */
    size_t getSystemCount() const;

private:
    /**
     * Sort systems by priority if needed
     */
    void sortSystemsIfNeeded();
};

} // namespace ECS