#pragma once

#include "../../ecs/components/include/Transform.hpp"

namespace ECS {

/**
 * GridMovement - Component for discrete grid-based movement with smooth visual transitions
 * 
 * This component bridges the logical grid layer and visual animation layer:
 * - Game logic operates on discrete grid coordinates (targetX, targetY)
 * - Visual rendering uses smooth interpolation based on progress
 * - Supports turn-based gameplay while maintaining smooth animations
 * 
 * Design principle: GridPosition is authoritative, Position is interpolated
 */
struct GridMovement {
    // Target grid coordinates (logical destination)
    int targetX = 0;
    int targetY = 0;
    
    // Animation state
    float progress = 0.0f;      // Movement progress (0.0 = start, 1.0 = complete)
    float speed = 1.0f;         // Movement speed multiplier (higher = faster)
    bool isMoving = false;      // Currently animating between cells?
    
    // Movement validation and queueing
    bool hasPendingMove = false;   // Movement request waiting for current move to finish
    int pendingX = 0;              // Queued target X coordinate
    int pendingY = 0;              // Queued target Y coordinate
    
    // Zero initialization compliance (ZII)
    GridMovement() = default;
    
    /**
     * Constructor for immediate movement setup
     */
    GridMovement(int targetX, int targetY, float speed = 1.0f)
        : targetX(targetX), targetY(targetY), speed(speed), isMoving(true) {}
        
    /**
     * Check if movement is complete
     */
    bool isComplete() const {
        return progress >= 1.0f;
    }
    
    /**
     * Reset movement state (called when movement completes)
     * Note: progress and hasPendingMove are intentionally NOT reset
     * - progress: preserved so completion can be verified
     * - hasPendingMove: preserved so queued movements can be executed manually
     */
    void reset() {
        isMoving = false;
    }
    
    /**
     * Queue a movement request (for turn-based systems)
     */
    void queueMove(int x, int y) {
        hasPendingMove = true;
        pendingX = x;
        pendingY = y;
    }
    
    /**
     * Start the queued movement (if any)
     */
    bool startQueuedMove() {
        if (hasPendingMove) {
            targetX = pendingX;
            targetY = pendingY;
            progress = 0.0f;
            isMoving = true;
            hasPendingMove = false;
            return true;
        }
        return false;
    }
};

/**
 * GridBounds - Component for defining valid movement boundaries
 * 
 * Used for collision detection and movement validation
 */
struct GridBounds {
    int minX = 0;
    int minY = 0;
    int maxX = 10;      // Default 10x10 grid
    int maxY = 10;
    
    // Zero initialization compliance (ZII)
    GridBounds() = default;
    
    GridBounds(int minX, int minY, int maxX, int maxY)
        : minX(minX), minY(minY), maxX(maxX), maxY(maxY) {}
        
    /**
     * Check if coordinates are within bounds
     */
    bool isValid(int x, int y) const {
        return x >= minX && x <= maxX && y >= minY && y <= maxY;
    }
    
    /**
     * Clamp coordinates to bounds
     */
    void clamp(int& x, int& y) const {
        if (x < minX) x = minX;
        if (x > maxX) x = maxX;
        if (y < minY) y = minY;
        if (y > maxY) y = maxY;
    }
};

} // namespace ECS