#pragma once

#include "../../ecs/include/ComponentRegistry.hpp"

namespace ECS {

/**
 * InputHandler - Component for entities that can receive input
 * 
 * Stores key mappings and input state for controllable entities.
 */
struct InputHandler {
    // Movement keys
    int upKey = 26;      // Keys::Up
    int downKey = 27;    // Keys::Down  
    int leftKey = 28;    // Keys::Left
    int rightKey = 29;   // Keys::Right
    
    // Action keys
    int actionKey = 30;  // Keys::Space
    int cancelKey = 32;  // Keys::Escape
    
    // Input state flags
    bool acceptsInput = true;
    bool blockMovement = false;
    bool blockActions = false;
};

/**
 * Controllable - Marker component for player-controllable entities
 * 
 * Simple flag component to identify entities that should respond to player input.
 */
struct Controllable {
    bool enabled = true;
    int playerId = 0;  // For multiplayer support
};

/**
 * InputState - Component storing current input state for an entity
 * 
 * Updated by InputSystem each frame based on InputHandler key mappings.
 */
struct InputState {
    // Movement input
    bool moveUp = false;
    bool moveDown = false;
    bool moveLeft = false;
    bool moveRight = false;
    
    // Action input
    bool action = false;
    bool cancel = false;
    
    // Input events (triggered once per press)
    bool actionPressed = false;
    bool cancelPressed = false;
    bool upPressed = false;
    bool downPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;
};

} // namespace ECS

// Register components with the ECS system
REGISTER_COMPONENT(ECS::InputHandler)
REGISTER_COMPONENT(ECS::Controllable)
REGISTER_COMPONENT(ECS::InputState)