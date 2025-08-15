#include "../include/SystemManager.hpp"
#include <algorithm>

namespace ECS {

// Constructor now has default implementation in header

void SystemManager::registerSystem(std::unique_ptr<ISystem> system) {
    systems.push_back(std::move(system));
    systemsNeedSorting = true;
}

void SystemManager::updateAll(float deltaTime, EntityManager& entityManager) {
    // Sort systems by priority if needed
    sortSystemsIfNeeded();
    
    // Update all systems that should run this frame
    for (auto& system : systems) {
        if (system->shouldUpdate(deltaTime)) {
            system->update(deltaTime, entityManager);
        }
    }
}

void SystemManager::clearSystems() {
    systems.clear();
    systemsNeedSorting = false;
}

size_t SystemManager::getSystemCount() const {
    return systems.size();
}

// getEntityManager() method removed - EntityManager now passed as parameter

void SystemManager::sortSystemsIfNeeded() {
    if (systemsNeedSorting) {
        std::sort(systems.begin(), systems.end(),
                  [](const std::unique_ptr<ISystem>& a, const std::unique_ptr<ISystem>& b) {
                      return a->getPriority() < b->getPriority();
                  });
        systemsNeedSorting = false;
    }
}

} // namespace ECS