#include "../include/Transform.hpp"

namespace ECS {

// Explicit template instantiations to generate symbols for the linker
// These use the generic getComponentBit<>() template from ComponentRegistry.hpp
template uint64_t getComponentBit<Position>();
template uint64_t getComponentBit<Rotation>();
template uint64_t getComponentBit<Scale>();
template uint64_t getComponentBit<GridPosition>();

} // namespace ECS