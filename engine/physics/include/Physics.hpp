#pragma once

#include <cmath>
#include <cstdlib>

namespace ECS {

/**
 * Velocity - Component for continuous movement and animation speed
 * 
 * Used for:
 * - Smooth interpolation between grid cells during GridMovement
 * - Continuous movement effects (particle systems, floating objects)
 * - Animation speed control and easing effects
 */
struct Velocity {
    float dx = 0.0f;    // Horizontal velocity (pixels/units per frame)
    float dy = 0.0f;    // Vertical velocity (pixels/units per frame)
    
    // Zero initialization compliance (ZII)
    Velocity() = default;
    
    Velocity(float dx, float dy) : dx(dx), dy(dy) {}
    
    /**
     * Get magnitude (speed) of velocity vector
     */
    float magnitude() const {
        return std::sqrt(dx * dx + dy * dy);
    }
    
    /**
     * Normalize velocity to unit vector (preserving direction)
     */
    void normalize() {
        float mag = magnitude();
        if (mag > 0.0f) {
            dx /= mag;
            dy /= mag;
        }
    }
    
    /**
     * Scale velocity by factor
     */
    void scale(float factor) {
        dx *= factor;
        dy *= factor;
    }
    
    /**
     * Check if velocity is effectively zero
     */
    bool isZero(float epsilon = 0.001f) const {
        return std::abs(dx) < epsilon && std::abs(dy) < epsilon;
    }
};

/**
 * Acceleration - Component for physics-based motion and force application
 * 
 * Used for:
 * - Smooth acceleration/deceleration during grid movement transitions
 * - Physics effects like gravity, wind, or magnetic forces
 * - Easing functions for polished animation curves
 */
struct Acceleration {
    float dx = 0.0f;    // Horizontal acceleration (change in velocity per frame)
    float dy = 0.0f;    // Vertical acceleration (change in velocity per frame)
    
    // Damping factor for velocity reduction over time
    float damping = 1.0f;   // 1.0 = no damping, 0.0 = immediate stop
    
    // Zero initialization compliance (ZII)
    Acceleration() = default;
    
    Acceleration(float dx, float dy, float damping = 1.0f) 
        : dx(dx), dy(dy), damping(damping) {}
    
    /**
     * Apply acceleration to velocity component
     */
    void applyTo(Velocity& velocity) const {
        velocity.dx += dx;
        velocity.dy += dy;
        
        // Apply damping
        if (damping < 1.0f) {
            velocity.dx *= damping;
            velocity.dy *= damping;
        }
    }
    
    /**
     * Check if acceleration is effectively zero
     */
    bool isZero(float epsilon = 0.001f) const {
        return std::abs(dx) < epsilon && std::abs(dy) < epsilon;
    }
};

/**
 * MovementConstraints - Component for limiting and controlling movement behavior
 * 
 * Used for:
 * - Maximum speed limits
 * - Movement restrictions (horizontal-only, vertical-only)
 * - Grid snapping behavior
 */
struct MovementConstraints {
    float maxSpeed = 1000.0f;       // Maximum velocity magnitude
    bool constrainToGrid = false;   // Snap movement to grid boundaries
    bool allowDiagonal = true;      // Allow diagonal movement
    bool allowContinuous = true;    // Allow free-form movement (vs grid-only)
    
    // Grid-specific constraints
    float gridSize = 32.0f;         // Size of each grid cell (for snapping)
    
    // Zero initialization compliance (ZII)
    MovementConstraints() = default;
    
    /**
     * Apply constraints to velocity
     */
    void applyTo(Velocity& velocity) const {
        // Apply speed limit
        if (velocity.magnitude() > maxSpeed) {
            velocity.normalize();
            velocity.scale(maxSpeed);
        }
        
        // Apply directional constraints
        if (!allowDiagonal) {
            // Force movement to primary axes (whichever is stronger)
            if (std::abs(velocity.dx) > std::abs(velocity.dy)) {
                velocity.dy = 0.0f;
            } else {
                velocity.dx = 0.0f;
            }
        }
        
        // Grid snapping
        if (constrainToGrid) {
            // Quantize velocity to grid-aligned movement
            velocity.dx = std::floor(velocity.dx / gridSize + 0.5f) * gridSize;
            velocity.dy = std::floor(velocity.dy / gridSize + 0.5f) * gridSize;
        }
    }
};

} // namespace ECS