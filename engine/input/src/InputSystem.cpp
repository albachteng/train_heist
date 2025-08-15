#include "../include/InputSystem.hpp"
#include "../../ecs/include/ComponentRegistry.hpp"

namespace ECS {

InputSystem::InputSystem(IInputManager* inputManager) 
    : inputManager_(inputManager) {
    if (!inputManager_) {
        throw std::invalid_argument("InputSystem requires valid IInputManager");
    }
}

uint64_t InputSystem::getRequiredComponents() const {
    return ComponentRegistry::getBitMask<InputHandler>() | 
           ComponentRegistry::getBitMask<InputState>() |
           ComponentRegistry::getBitMask<Controllable>();
}

void InputSystem::update(float deltaTime, EntityManager& entityManager) {
    // Update input manager state
    inputManager_->update();
    
    // Query all entities with input components
    auto entities = entityManager.getEntitiesWithComponents(getRequiredComponents());
    
    for (Entity entity : entities) {
        auto* handler = entityManager.getComponent<InputHandler>(entity);
        auto* state = entityManager.getComponent<InputState>(entity);
        auto* controllable = entityManager.getComponent<Controllable>(entity);
        
        if (handler && state && controllable && controllable->enabled) {
            updateEntityInput(entity, *handler, *state);
        }
    }
}

void InputSystem::updateEntityInput(Entity entity, InputHandler& handler, InputState& state) {
    if (!handler.acceptsInput) {
        // Clear all input state if input is disabled
        state.moveUp = false;
        state.moveDown = false;
        state.moveLeft = false;
        state.moveRight = false;
        state.action = false;
        state.cancel = false;
        state.actionPressed = false;
        state.cancelPressed = false;
        state.upPressed = false;
        state.downPressed = false;
        state.leftPressed = false;
        state.rightPressed = false;
        return;
    }
    
    // Update movement input (unless blocked)
    if (!handler.blockMovement) {
        state.moveUp = inputManager_->isKeyPressed(handler.upKey);
        state.moveDown = inputManager_->isKeyPressed(handler.downKey);
        state.moveLeft = inputManager_->isKeyPressed(handler.leftKey);
        state.moveRight = inputManager_->isKeyPressed(handler.rightKey);
        
        // Movement press events
        state.upPressed = inputManager_->wasKeyJustPressed(handler.upKey);
        state.downPressed = inputManager_->wasKeyJustPressed(handler.downKey);
        state.leftPressed = inputManager_->wasKeyJustPressed(handler.leftKey);
        state.rightPressed = inputManager_->wasKeyJustPressed(handler.rightKey);
    } else {
        state.moveUp = false;
        state.moveDown = false;
        state.moveLeft = false;
        state.moveRight = false;
        state.upPressed = false;
        state.downPressed = false;
        state.leftPressed = false;
        state.rightPressed = false;
    }
    
    // Update action input (unless blocked)
    if (!handler.blockActions) {
        state.action = inputManager_->isKeyPressed(handler.actionKey);
        state.cancel = inputManager_->isKeyPressed(handler.cancelKey);
        
        // Action press events
        state.actionPressed = inputManager_->wasKeyJustPressed(handler.actionKey);
        state.cancelPressed = inputManager_->wasKeyJustPressed(handler.cancelKey);
    } else {
        state.action = false;
        state.cancel = false;
        state.actionPressed = false;
        state.cancelPressed = false;
    }
}

} // namespace ECS