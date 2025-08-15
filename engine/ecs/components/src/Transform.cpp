#include "../include/Transform.hpp"

namespace ECS {

// Template specializations for transform components
// These provide the implementations declared in the header

template<>
uint64_t getComponentBit<Position>() {
    static_assert(std::is_trivially_copyable_v<Position>, 
                  "Component types must be trivially copyable (POD structs only)");
    static_assert(std::is_default_constructible_v<Position>, 
                  "Component types must be default constructible");
    
    uint64_t& bit = ComponentRegistry::ComponentBitStorage<Position>::bit;
    if (bit == 0) {
        uint64_t bitPosition = ComponentRegistry::nextComponentBit.fetch_add(1);
        if (bitPosition >= 64) {
            bit = 1ULL << 63;
        } else {
            bit = 1ULL << bitPosition;
        }
    }
    return bit;
}

template<>
uint64_t getComponentBit<Rotation>() {
    static_assert(std::is_trivially_copyable_v<Rotation>, 
                  "Component types must be trivially copyable (POD structs only)");
    static_assert(std::is_default_constructible_v<Rotation>, 
                  "Component types must be default constructible");
    
    uint64_t& bit = ComponentRegistry::ComponentBitStorage<Rotation>::bit;
    if (bit == 0) {
        uint64_t bitPosition = ComponentRegistry::nextComponentBit.fetch_add(1);
        if (bitPosition >= 64) {
            bit = 1ULL << 63;
        } else {
            bit = 1ULL << bitPosition;
        }
    }
    return bit;
}

template<>
uint64_t getComponentBit<Scale>() {
    static_assert(std::is_trivially_copyable_v<Scale>, 
                  "Component types must be trivially copyable (POD structs only)");
    static_assert(std::is_default_constructible_v<Scale>, 
                  "Component types must be default constructible");
    
    uint64_t& bit = ComponentRegistry::ComponentBitStorage<Scale>::bit;
    if (bit == 0) {
        uint64_t bitPosition = ComponentRegistry::nextComponentBit.fetch_add(1);
        if (bitPosition >= 64) {
            bit = 1ULL << 63;
        } else {
            bit = 1ULL << bitPosition;
        }
    }
    return bit;
}

template<>
uint64_t getComponentBit<GridPosition>() {
    static_assert(std::is_trivially_copyable_v<GridPosition>, 
                  "Component types must be trivially copyable (POD structs only)");
    static_assert(std::is_default_constructible_v<GridPosition>, 
                  "Component types must be default constructible");
    
    uint64_t& bit = ComponentRegistry::ComponentBitStorage<GridPosition>::bit;
    if (bit == 0) {
        uint64_t bitPosition = ComponentRegistry::nextComponentBit.fetch_add(1);
        if (bitPosition >= 64) {
            bit = 1ULL << 63;
        } else {
            bit = 1ULL << bitPosition;
        }
    }
    return bit;
}

} // namespace ECS