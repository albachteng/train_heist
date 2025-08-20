#pragma once

#include "IInputManager.hpp"
#include "../../include/EntityManager.hpp"
#include "../../components/include/Transform.hpp"

namespace ECS {

/**
 * InputSystem - Handles input processing for controllable entities
 * 
 * Processes input events and updates entity positions/states based on user input.
 * Designed to work with any IInputManager implementation (SFML, mock, etc.).
 */
class InputSystem {
public:
    /**
     * Constructor with dependency injection
     * @param inputManager Input manager to use for input queries
     */
    explicit InputSystem(IInputManager* inputManager);
    ~InputSystem() = default;
    
    /**
     * Update system - processes input and updates entity states
     * @param entityManager Entity manager containing entities to process
     * @param deltaTime Time elapsed since last update (for movement speed)
     */
    void update(EntityManager& entityManager, float deltaTime = 1.0f);
    
    /**
     * Set which entity should be controlled by keyboard input
     * @param entityId ID of entity to control (or invalid entity to disable)
     */
    void setControlledEntity(EntityID entityId);
    
    /**
     * Get the currently controlled entity
     * @return Entity ID being controlled, or INVALID_ENTITY if none
     */
    EntityID getControlledEntity() const;
    
    /**
     * Set movement speed for keyboard control
     * @param speed Movement speed in pixels per frame
     */
    void setMovementSpeed(float speed);
    
    /**
     * Get current movement speed
     * @return Movement speed in pixels per frame
     */
    float getMovementSpeed() const;
    
    /**
     * Check if input system has a valid input manager
     * @return true if input manager is available
     */
    bool hasInputManager() const;
    
    /**
     * Get the input manager being used
     * @return Pointer to input manager
     */
    IInputManager* getInputManager() const;

private:
    /**
     * Process keyboard input for controlled entity
     * @param entityManager Entity manager for position updates
     * @param deltaTime Time multiplier for movement
     */
    void processKeyboardInput(EntityManager& entityManager, float deltaTime);
    
    /**
     * Process mouse input and log clicks
     */
    void processMouseInput();
    
    // Dependencies
    IInputManager* inputManager;
    
    // State
    EntityID controlledEntity;
    float movementSpeed;
    
    // Input state tracking
    bool wasMouseLeftPressed;
    bool wasMouseRightPressed;
};

} // namespace ECS