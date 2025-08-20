#include "../include/InputSystem.hpp"
#include "../include/IInputManager.hpp"
#include "../../include/EntityManager.hpp"
#include "../../components/include/Transform.hpp"
#include "../../logging/include/Logger.hpp"

namespace ECS {

InputSystem::InputSystem(IInputManager* inputManager)
    : inputManager(inputManager)
    , controlledEntity(INVALID_ENTITY)
    , movementSpeed(50.0f)  // Default 50 pixels per frame
    , wasMouseLeftPressed(false)
    , wasMouseRightPressed(false)
{
}

void InputSystem::update(EntityManager& entityManager, float deltaTime) {
    if (!inputManager) {
        return;
    }
    
    // Update input manager state first
    inputManager->update();
    
    // Process different types of input
    processKeyboardInput(entityManager, deltaTime);
    processMouseInput();
}

void InputSystem::setControlledEntity(EntityID entityId) {
    controlledEntity = entityId;
    if (entityId != INVALID_ENTITY) {
        LOG_INFO("InputSystem", "Set controlled entity to: " + std::to_string(entityId));
    } else {
        LOG_INFO("InputSystem", "Disabled entity control");
    }
}

EntityID InputSystem::getControlledEntity() const {
    return controlledEntity;
}

void InputSystem::setMovementSpeed(float speed) {
    movementSpeed = speed;
}

float InputSystem::getMovementSpeed() const {
    return movementSpeed;
}

bool InputSystem::hasInputManager() const {
    return inputManager != nullptr;
}

IInputManager* InputSystem::getInputManager() const {
    return inputManager;
}

void InputSystem::processKeyboardInput(EntityManager& entityManager, float deltaTime) {
    // Skip if no entity is controlled
    if (controlledEntity == INVALID_ENTITY) {
        return;
    }
    
    // For this demo, we'll just log key presses
    // The actual movement will be handled in main.cpp
    
    // Log key presses for debugging
    if (inputManager->wasKeyPressed(KeyCode::Left)) {
        LOG_INFO("InputSystem", "Left arrow pressed - moving entity " + std::to_string(controlledEntity));
    }
    if (inputManager->wasKeyPressed(KeyCode::Right)) {
        LOG_INFO("InputSystem", "Right arrow pressed - moving entity " + std::to_string(controlledEntity));
    }
    if (inputManager->wasKeyPressed(KeyCode::Up)) {
        LOG_INFO("InputSystem", "Up arrow pressed - moving entity " + std::to_string(controlledEntity));
    }
    if (inputManager->wasKeyPressed(KeyCode::Down)) {
        LOG_INFO("InputSystem", "Down arrow pressed - moving entity " + std::to_string(controlledEntity));
    }
}

void InputSystem::processMouseInput() {
    // Check for mouse button presses
    bool leftPressed = inputManager->wasMouseButtonPressed(0);
    bool rightPressed = inputManager->wasMouseButtonPressed(1);
    
    if (leftPressed) {
        int x, y;
        inputManager->getMousePosition(x, y);
        LOG_INFO("InputSystem", "Left mouse click at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }
    
    if (rightPressed) {
        int x, y;
        inputManager->getMousePosition(x, y);
        LOG_INFO("InputSystem", "Right mouse click at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }
    
    // Track state for next frame
    wasMouseLeftPressed = leftPressed;
    wasMouseRightPressed = rightPressed;
}

} // namespace ECS