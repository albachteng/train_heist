#include "../include/TransformUtils.hpp"
#include <cmath>
#include <algorithm>

namespace ECS {
namespace TransformUtils {

Position gridToWorld(const GridPosition& gridPos, float tileWidth, float tileHeight) {
    // Convert grid coordinates to world coordinates, centering on tile
    Position world;
    world.x = gridPos.x * tileWidth + (tileWidth / 2.0f);
    world.y = gridPos.y * tileHeight + (tileHeight / 2.0f);
    world.z = 0.0f; // Default Z for 2D grid
    return world;
}

GridPosition worldToGrid(const Position& worldPos, float tileWidth, float tileHeight) {
    // Convert world coordinates to grid coordinates (rounded to nearest tile)
    GridPosition grid;
    grid.x = static_cast<int>(std::round(worldPos.x / tileWidth));
    grid.y = static_cast<int>(std::round(worldPos.y / tileHeight));
    return grid;
}

Position gridToIsometric(const GridPosition& gridPos, float tileWidth, float tileHeight) {
    // Isometric projection: screen_x = (grid_x - grid_y) * tile_width/2
    // screen_y = (grid_x + grid_y) * tile_height/2
    Position iso;
    iso.x = (gridPos.x - gridPos.y) * (tileWidth / 2.0f);
    iso.y = (gridPos.x + gridPos.y) * (tileHeight / 2.0f);
    iso.z = 0.0f; // Z can be used for depth sorting later
    return iso;
}

Position worldToIsometric(const Position& worldPos, float tileWidth, float tileHeight) {
    // Convert world coordinates to isometric projection
    Position iso;
    iso.x = (worldPos.x - worldPos.y) * (tileWidth / 2.0f);
    iso.y = (worldPos.x + worldPos.y) * (tileHeight / 2.0f);
    iso.z = worldPos.z; // Preserve Z for depth sorting
    return iso;
}

float distance(const Position& pos1, const Position& pos2) {
    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    float dz = pos2.z - pos1.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

float distanceSquared(const Position& pos1, const Position& pos2) {
    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    float dz = pos2.z - pos1.z;
    return dx * dx + dy * dy + dz * dz;
}

int manhattanDistance(const GridPosition& grid1, const GridPosition& grid2) {
    return std::abs(grid2.x - grid1.x) + std::abs(grid2.y - grid1.y);
}

float normalizeAngle(float angle) {
    constexpr float TWO_PI = 2.0f * 3.14159265359f;
    
    // Bring angle into [0, 2π) range
    angle = std::fmod(angle, TWO_PI);
    
    // Handle negative angles
    if (angle < 0.0f) {
        angle += TWO_PI;
    }
    
    return angle;
}

void applyUniformScale(Scale& scale, float factor) {
    scale.x *= factor;
    scale.y *= factor;
}

Position lerp(const Position& start, const Position& end, float t) {
    // Clamp t to [0, 1] range
    t = std::clamp(t, 0.0f, 1.0f);
    
    Position result;
    result.x = start.x + t * (end.x - start.x);
    result.y = start.y + t * (end.y - start.y);
    result.z = start.z + t * (end.z - start.z);
    return result;
}

bool approximately(const Position& pos1, const Position& pos2, float epsilon) {
    float dx = std::abs(pos2.x - pos1.x);
    float dy = std::abs(pos2.y - pos1.y);
    float dz = std::abs(pos2.z - pos1.z);
    
    return dx <= epsilon && dy <= epsilon && dz <= epsilon;
}

} // namespace TransformUtils
} // namespace ECS