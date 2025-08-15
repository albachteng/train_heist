#pragma once

#include "../../include/ComponentRegistry.hpp"

namespace ECS {

/**
 * Position - 3D world coordinates for entities
 * 
 * Features:
 * - 3D coordinates ready for 2.5D isometric projection
 * - Zero-initialized by default (ZII compliant)
 * - Cache-friendly POD struct for SoA storage
 * - Z coordinate used for depth sorting in isometric view
 */
struct Position {
    float x = 0.0f;  // X coordinate in world space
    float y = 0.0f;  // Y coordinate in world space
    float z = 0.0f;  // Z coordinate for depth/height in 2.5D
    
    // Equality operators for testing
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

/**
 * Rotation - Angular rotation for entities
 * 
 * Features:
 * - Single angle in radians for 2D rotation
 * - Zero-initialized by default (ZII compliant)
 * - Suitable for sprite rotation and isometric object orientation
 */
struct Rotation {
    float angle = 0.0f;  // Rotation angle in radians
    
    // Equality operators for testing
    bool operator==(const Rotation& other) const {
        return angle == other.angle;
    }
    
    bool operator!=(const Rotation& other) const {
        return !(*this == other);
    }
};

/**
 * Scale - Scaling factors for entities
 * 
 * Features:
 * - Separate X and Y scaling for non-uniform scaling
 * - Default scale of 1.0 (no scaling)
 * - Zero-initialized would make entities invisible, so default to 1.0
 * - Useful for sprite scaling and visual effects
 */
struct Scale {
    float x = 1.0f;  // X-axis scale factor (1.0 = normal size)
    float y = 1.0f;  // Y-axis scale factor (1.0 = normal size)
    
    // Equality operators for testing
    bool operator==(const Scale& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Scale& other) const {
        return !(*this == other);
    }
};

/**
 * GridPosition - Discrete grid coordinates for tile-based games
 * 
 * Features:
 * - Integer coordinates for grid-based movement
 * - Zero-initialized by default (ZII compliant)
 * - Used for game logic while Position handles rendering coordinates
 * - Essential for turn-based tactical games like Train Heist
 */
struct GridPosition {
    int x = 0;  // Grid X coordinate (tile column)
    int y = 0;  // Grid Y coordinate (tile row)
    
    // Equality operators for testing
    bool operator==(const GridPosition& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const GridPosition& other) const {
        return !(*this == other);
    }
};

} // namespace ECS

/**
 * Component registry declarations
 * These will be defined in the implementation to get unique bit assignments
 */
namespace ECS {
    // Component bit getter declarations
    template<> uint64_t getComponentBit<Position>();
    template<> uint64_t getComponentBit<Rotation>();
    template<> uint64_t getComponentBit<Scale>();
    template<> uint64_t getComponentBit<GridPosition>();
}