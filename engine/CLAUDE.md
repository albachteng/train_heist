# CLAUDE.md

**CRITICAL: After the red phase, you must always pause and wait for my approval before moving forward with implementation.**

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a lightweight, ECS-driven 2D/2.5D isometric game engine written in C++. The project has completed its ECS foundation and SFML rendering implementation with comprehensive testing infrastructure.

## Architecture

The engine follows a modular ECS (Entity-Component-System) architecture with these core systems:

- **ECS Core** (`ecs/`): Entity/component management with bitmask tracking for efficient queries ✅ **IMPLEMENTED**
- **Systems Layer** (`ecs/systems/`): Priority-based system execution with dependency injection ✅ **IMPLEMENTED**
- **Logging System** (`logging/`): Multi-level logging with console/file output and global macros ✅ **IMPLEMENTED**
- **Rendering System** (`rendering/`): Complete SFML integration with interface abstractions ✅ **IMPLEMENTED**
- **Input System** (`input/`): User input mapping to game events *(next priority - interface ready)*
- **Physics System** (`physics/`): Movement, collisions, and grid alignment *(planned)*
- **Resources System** (`resources/`): Asset loading and management *(interface implemented)*
- **Utils** (`utils/`): Shared utilities and helper functions *(planned)*

## Key Design Principles

1. **Components are POD structs** - No behavior, zero-initialized (ZII)
2. **Systems are stateless functions** - Operate on component arrays
3. **Bitmask-based queries** - Efficient multi-component filtering
4. **Struct-of-Arrays (SoA) storage** - Cache-friendly component layout
5. **Memory arenas** - Fast allocation and swap-remove operations
6. **TDD approach** - Tests and headers written before implementation
7. **Typed events** - `Event<T>` system for decoupled communication
8. **Dependency injection** - Systems receive EntityManager as parameter for testability

## Development Workflow

The ECS core, systems layer, logging system, and rendering system are fully implemented with comprehensive test coverage (256 tests: 201 unit + 55 integration). For the input system and future engine systems:

1. **Start with headers** - Define component structs and system interfaces in `include/` directories
2. **Write tests first** - Create unit tests in `tests/` directories before implementation  
3. **Implement in isolation** - Each system should be independently testable
4. **Follow SoA patterns** - Store components in arrays, not as object members
5. **Use bitmasks for queries** - Entity bitmasks indicate component presence
6. **Follow existing ECS patterns** - Reference the implemented ECS core for architectural consistency

## Module Structure

Each module follows this pattern:
```
module/
├── include/    # Public headers and component definitions
├── src/        # Implementation files  
└── tests/      # Unit tests for the module
```

## Component Design

Components must be:
- POD (Plain Old Data) structs
- Zero-initialized by default
- Stored in Struct-of-Arrays layout
- Accessed via entity bitmask queries

Example component pattern:
```cpp
struct Position {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};
```

## System Design

Systems should be:
- Stateless functions operating on component arrays
- Testable in isolation
- Query components via bitmask filtering
- Communicate through typed events

## Event System

Use `Event<T>` with strongly typed payloads for system communication:
- Input events trigger movement
- Movement events trigger sound/animation
- Collision events trigger game logic

## Performance Considerations

- Use memory arenas for component allocation
- Implement swap-remove to keep arrays dense
- Leverage bitmasks for branch-free queries
- Maintain cache-friendly SoA data layout

## Testing Architecture

The project uses a two-tier testing system:

- **Unit Tests** (`make test`): 295+ fast tests with no external dependencies
  - Mock implementations for testing without graphics/window systems
  - ECS core, logging, components, input system, and system logic testing
- **Integration Tests** (`make integration`): Comprehensive SFML integration tests
  - Real SFML library integration validation
  - Color conversion, event handling, input processing, and rendering pipeline testing
  - Requires SFML libraries to be linked

## Current Implementation Status

### ✅ **Completed Systems**
- **ECS Core**: Full entity/component management with bitmask queries
- **Component Registry**: Automatic component type registration
- **Event System**: Typed event queues for decoupled communication
- **SystemManager**: Priority-based system execution with dependency injection
- **Logging**: Multi-level logging with file/console output
- **Transform Components**: Position, Rotation, Scale, GridPosition with utilities
- **Rendering Components**: Sprite and Renderable with ZII compliance
- **RenderSystem**: Entity filtering and rendering orchestration
- **Interface Abstractions**: IRenderer, IWindowManager, IResourceManager, IInputManager
- **SFML Integration**: Complete SFMLRenderer, SFMLWindowManager, SFMLResourceManager, SFMLInputManager
- **Mock Infrastructure**: Full mock implementations for testing (MockRenderer, MockWindowManager, MockResourceManager, MockInputManager)
- **Event Conversion**: SFML → Engine event abstraction layer
- **Input System**: SFMLInputManager with keyboard/mouse processing, InputSystem with ECS integration
- **Interactive Demo**: Working keyboard-controlled entity with complete input pipeline

### 🚧 **Next Priority: Grid-Based Physics System**
- **GridMovement Component**: Discrete grid movement with smooth visual transitions
- **Movement System**: Grid validation, interpolation, and turn-based movement support
- **Physics Components**: Velocity and acceleration for smooth animations between grid cells
- **Integration**: Update demo to use grid-based movement instead of free-form positioning

## Grid-Based Physics Design Approach

The physics system will support **dual-layer movement** for turn-based tactical gameplay:

### **Logical Layer** (Grid Authority)
- **GridPosition { int x, y; }**: Authoritative game position for all logic
- **GridMovement { int targetX, targetY; float progress; bool isMoving; }**: Movement state
- All collision detection, game rules, and turn management operate on grid coordinates

### **Visual Layer** (Smooth Animation)  
- **Position { float x, y, z; }**: Visual rendering position (interpolated)
- **Velocity { float dx, dy; }**: Movement speed for grid cell transitions
- Smooth interpolation between grid cells while maintaining discrete game logic

### **Movement Pipeline**
1. **Input → Grid Command**: Arrow keys generate grid movement requests
2. **Validation**: Check target cell validity (bounds, obstacles, game rules)
3. **Animation Start**: Set GridMovement target and initialize visual interpolation
4. **Frame Updates**: Update Position based on GridMovement progress
5. **Completion**: Snap to exact grid coordinates when movement finishes

This approach enables:
- **Turn-based gameplay**: Movement commands can be queued/executed in turns
- **Smooth visuals**: No jarring teleportation between cells
- **Game rule integration**: Collision detection, movement points, obstacles
- **Future scalability**: Multi-entity simultaneous movement, pathfinding