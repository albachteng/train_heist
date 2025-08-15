#pragma once

#include "Transform.hpp"
#include <cmath>

namespace ECS {

/**
 * TransformUtils - Utility functions for working with transform components
 * 
 * Provides common operations and coordinate system conversions needed
 * for 2.5D isometric games and general transform manipulation.
 */
namespace TransformUtils {

/**
 * Convert grid coordinates to world coordinates
 * Useful for positioning entities based on grid logic
 * @param gridPos Grid position (discrete tiles)
 * @param tileWidth Width of each tile in world units
 * @param tileHeight Height of each tile in world units
 * @return World position centered on the tile
 */
Position gridToWorld(const GridPosition& gridPos, float tileWidth = 1.0f, float tileHeight = 1.0f);

/**
 * Convert world coordinates to grid coordinates
 * Useful for determining which tile an entity occupies
 * @param worldPos World position
 * @param tileWidth Width of each tile in world units
 * @param tileHeight Height of each tile in world units
 * @return Grid position (rounded to nearest tile)
 */
GridPosition worldToGrid(const Position& worldPos, float tileWidth = 1.0f, float tileHeight = 1.0f);

/**
 * Convert grid coordinates to isometric screen coordinates
 * Implements the isometric projection for 2.5D rendering
 * @param gridPos Grid position
 * @param tileWidth Width of isometric tile on screen
 * @param tileHeight Height of isometric tile on screen
 * @return Screen position for isometric rendering
 */
Position gridToIsometric(const GridPosition& gridPos, float tileWidth = 64.0f, float tileHeight = 32.0f);

/**
 * Convert world coordinates to isometric screen coordinates
 * @param worldPos 3D world position
 * @param tileWidth Width of isometric tile on screen
 * @param tileHeight Height of isometric tile on screen
 * @return Screen position for isometric rendering
 */
Position worldToIsometric(const Position& worldPos, float tileWidth = 64.0f, float tileHeight = 32.0f);

/**
 * Calculate distance between two positions
 * @param pos1 First position
 * @param pos2 Second position
 * @return Euclidean distance between positions
 */
float distance(const Position& pos1, const Position& pos2);

/**
 * Calculate squared distance between two positions (faster, no sqrt)
 * Useful for distance comparisons without needing exact distance
 * @param pos1 First position
 * @param pos2 Second position
 * @return Squared Euclidean distance
 */
float distanceSquared(const Position& pos1, const Position& pos2);

/**
 * Calculate Manhattan distance between two grid positions
 * Useful for turn-based movement calculations
 * @param grid1 First grid position
 * @param grid2 Second grid position
 * @return Manhattan distance (sum of absolute differences)
 */
int manhattanDistance(const GridPosition& grid1, const GridPosition& grid2);

/**
 * Normalize a rotation angle to [0, 2π) range
 * @param angle Angle in radians
 * @return Normalized angle in [0, 2π) range
 */
float normalizeAngle(float angle);

/**
 * Convert degrees to radians
 * @param degrees Angle in degrees
 * @return Angle in radians
 */
constexpr float degreesToRadians(float degrees) {
    return degrees * (3.14159265359f / 180.0f);
}

/**
 * Convert radians to degrees
 * @param radians Angle in radians
 * @return Angle in degrees
 */
constexpr float radiansToDegrees(float radians) {
    return radians * (180.0f / 3.14159265359f);
}

/**
 * Apply uniform scaling to both X and Y axes
 * @param scale Scale component to modify
 * @param factor Uniform scale factor
 */
void applyUniformScale(Scale& scale, float factor);

/**
 * Linear interpolation between two positions
 * @param start Starting position
 * @param end Ending position
 * @param t Interpolation factor [0.0, 1.0]
 * @return Interpolated position
 */
Position lerp(const Position& start, const Position& end, float t);

/**
 * Check if two positions are approximately equal (within epsilon)
 * @param pos1 First position
 * @param pos2 Second position
 * @param epsilon Tolerance for comparison
 * @return true if positions are approximately equal
 */
bool approximately(const Position& pos1, const Position& pos2, float epsilon = 0.001f);

} // namespace TransformUtils
} // namespace ECS