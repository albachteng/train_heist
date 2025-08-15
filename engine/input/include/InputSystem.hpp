#pragma once

#include "../../ecs/systems/include/ISystem.hpp"
#include "IInputManager.hpp"
#include "InputComponents.hpp"
#include "../../ecs/include/EntityManager.hpp"

namespace ECS {

/**
 * InputSystem - System that processes input and updates InputState components
 * 
 * Queries all entities with InputHandler and InputState components,
 * reads input from IInputManager, and updates component state accordingly.
 */
class InputSystem : public ISystem {
public:
    explicit InputSystem(IInputManager* inputManager);
    virtual ~InputSystem() = default;
    
    // ISystem interface
    void update(float deltaTime, EntityManager& entityManager) override;
    int getPriority() const override { return 10; } // High priority - input processed first
    uint64_t getRequiredComponents() const override;
    bool shouldUpdate(float deltaTime) const override { return true; } // Always update
    
private:
    IInputManager* inputManager_;
    
    void updateEntityInput(Entity entity, InputHandler& handler, InputState& state);
};

} // namespace ECS