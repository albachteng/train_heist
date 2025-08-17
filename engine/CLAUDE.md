# CLAUDE.md

**CRITICAL: After the red phase, you must always pause and wait for my approval before moving forward with implementation.**

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a lightweight, ECS-driven 2D/2.5D isometric game engine written in C++. The project is currently in the design phase with detailed architecture documentation but no implementation code yet.

## Architecture

The engine follows a modular ECS (Entity-Component-System) architecture with these core systems:

- **ECS Core** (`ecs/`): Entity/component management with bitmask tracking for efficient queries ✅ **IMPLEMENTED**
- **Systems Layer** (`ecs/systems/`): Priority-based system execution with dependency injection ✅ **IMPLEMENTED**
- **Input System** (`input/`): User input mapping to game events *(planned - interface ready)*
- **Physics System** (`physics/`): Movement, collisions, and grid alignment *(planned)*
- **Rendering System** (`rendering/`): Sprite/tile drawing and camera transforms *(planned)*
- **Resources System** (`resources/`): Asset loading and management *(planned)*
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

The ECS core and systems layer are fully implemented with comprehensive test coverage (79 passing tests). For future engine systems:

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