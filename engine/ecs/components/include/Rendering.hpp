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
 * - Zero-initialized by default (ZII compliant) - NOTE: alpha = 0.0f (transparent)
 * - Used for debugging, UI elements, and simple shapes
 * - Alternative to sprite-based rendering
 * 
 * Important: Zero-initialized renderables are transparent (alpha = 0.0f).
 * You must explicitly set alpha > 0.0f to make them visible:
 * 
 * Examples:
 *   Renderable solid{100.0f, 50.0f, 1.0f, 0.0f, 0.0f, 1.0f};    // Opaque red
 *   Renderable overlay{50.0f, 25.0f, 0.0f, 1.0f, 0.0f, 0.3f};   // Semi-transparent green
 *   Renderable invisible{10.0f, 10.0f, 1.0f, 1.0f, 1.0f, 0.0f}; // Invisible (ZII default)
 * 
 * Helper patterns for common use cases:
 *   // Debug visible rectangle: {width, height, r, g, b, 1.0f}
 *   // UI overlay: {width, height, r, g, b, 0.5f}
 *   // Invisible placeholder: {width, height} (defaults to alpha=0.0f)
 */
struct Renderable {
    float width = 0.0f;     // Shape width in world units
    float height = 0.0f;    // Shape height in world units
    float red = 0.0f;       // Red color component (0.0 - 1.0)
    float green = 0.0f;     // Green color component (0.0 - 1.0)
    float blue = 0.0f;      // Blue color component (0.0 - 1.0)
    float alpha = 0.0f;     // Alpha transparency (0.0 - 1.0, ZII = transparent)
    
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