#include "../include/ComponentRegistry.hpp"

namespace ECS {

namespace ComponentRegistry {
    // Initialize the global atomic counter
    std::atomic<uint64_t> nextComponentBit{0};
}

void resetComponentRegistry() {
    ComponentRegistry::nextComponentBit.store(0);
    // Note: This doesn't reset the static bit storage in ComponentBitStorage<T>::bit
    // In a real implementation, you might want to maintain a registry of all 
    // component types for complete reset functionality. For testing, we can
    // work around this by using fresh component types.
}

} // namespace ECS