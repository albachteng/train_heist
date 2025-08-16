#pragma once

#include "../../include/ComponentRegistry.hpp"

namespace ECS {

/**
 * Sprite - Component for texture-based rendering
 * 
 * Features:
 * - Texture ID for renderer lookup
 * - Dimensions for sprite sizing
 * - Zero-initialized by default (ZII compliant)
 * - Cache-friendly POD struct for SoA storage
 * - Used with Position component for world placement
 */
struct Sprite {
    int textureId = 0;      // Texture identifier for renderer
    float width = 0.0f;     // Sprite width in world units
    float height = 0.0f;    // Sprite height in world units
    
    // Equality operators for testing
    bool operator==(const Sprite& other) const {
        return textureId == other.textureId && 
               width == other.width && 
               height == other.height;
    }
    
    bool operator!=(const Sprite& other) const {
        return !(*this == other);
    }
};

/**
 * Renderable - Component for basic shape rendering
 * 
 * Features:
 * - RGBA color values for shape rendering
 * - Dimensions for rectangle shapes
 * - Zero-initialized by default (ZII compliant)
 * - Used for debugging, UI elements, and simple shapes
 * - Alternative to sprite-based rendering
 */
struct Renderable {
    float width = 0.0f;     // Shape width in world units
    float height = 0.0f;    // Shape height in world units
    float red = 0.0f;       // Red color component (0.0 - 1.0)
    float green = 0.0f;     // Green color component (0.0 - 1.0)
    float blue = 0.0f;      // Blue color component (0.0 - 1.0)
    float alpha = 1.0f;     // Alpha transparency (0.0 - 1.0, default opaque)
    
    // Equality operators for testing
    bool operator==(const Renderable& other) const {
        return width == other.width && 
               height == other.height &&
               red == other.red &&
               green == other.green &&
               blue == other.blue &&
               alpha == other.alpha;
    }
    
    bool operator!=(const Renderable& other) const {
        return !(*this == other);
    }
};

} // namespace ECS

// No explicit template specializations needed - the generic getComponentBit<>() 
// template in ComponentRegistry.hpp handles all component types automatically