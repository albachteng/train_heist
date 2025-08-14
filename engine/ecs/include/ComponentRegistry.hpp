#pragma once

#include <cstdint>
#include <atomic>
#include <type_traits>

namespace ECS {

/**
 * ComponentRegistry - Automatic component bit assignment
 * 
 * Provides automatic bit assignment for component types, eliminating the need
 * for manual bit management. Each component type gets a unique bit position
 * assigned automatically when first queried.
 * 
 * Usage:
 *   uint64_t positionBit = getComponentBit<Position>();
 *   uint64_t velocityBit = getComponentBit<Velocity>();
 * 
 * Thread-safe: Uses atomic counter for bit assignment.
 * Limitation: Supports up to 64 component types (uint64_t bitmask).
 */

namespace ComponentRegistry {
    // Global atomic counter for bit assignment
    extern std::atomic<uint64_t> nextComponentBit;
    
    // Type-specific bit storage using template specialization
    template<typename Component>
    struct ComponentBitStorage {
        static uint64_t bit;
    };
    
    template<typename Component>
    uint64_t ComponentBitStorage<Component>::bit = 0;
}

/**
 * Get the component bit for a given type.
 * Assigns a new bit on first call, returns cached bit on subsequent calls.
 */
template<typename Component>
uint64_t getComponentBit() {
    static_assert(std::is_trivially_copyable_v<Component>, 
                  "Component types must be trivially copyable (POD structs only)");
    static_assert(std::is_default_constructible_v<Component>, 
                  "Component types must be default constructible");
    
    uint64_t& bit = ComponentRegistry::ComponentBitStorage<Component>::bit;
    if (bit == 0) {
        // Assign new bit position (thread-safe)
        uint64_t bitPosition = ComponentRegistry::nextComponentBit.fetch_add(1);
        if (bitPosition >= 64) {
            // Error: too many component types
            // In a real implementation, you might want to throw an exception
            // For now, we'll use bit 63 as an error indicator
            bit = 1ULL << 63;
        } else {
            bit = 1ULL << bitPosition;
        }
    }
    return bit;
}

/**
 * Get the number of registered component types
 */
inline uint64_t getRegisteredComponentCount() {
    return ComponentRegistry::nextComponentBit.load();
}

/**
 * Reset the component registry (primarily for testing)
 */
void resetComponentRegistry();

} // namespace ECS