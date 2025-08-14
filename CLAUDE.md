# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Train Heist is a turn-based tactical strategy game engine written in C++ using a lightweight ECS (Entity-Component-System) architecture. The project is currently in the design phase with comprehensive documentation but no implementation code yet. The game features isometric 2.5D grid-based gameplay where players control multiple units to stop and board trains across different time periods.

## Build System

### Primary Build Commands
- `make` or `make all` - Build the main game executable
- `make test` - Build and run the ECS unit tests
- `make run` - Build and run the main game executable
- `make clean` - Clean all build artifacts

### Platform Support
The Makefile automatically detects the platform (Linux, macOS, Windows) and configures the appropriate library paths:
- **Linux**: Uses `build/linux/` directory, links against `-lGL`
- **macOS**: Uses `build/macos/` directory, uses `-framework OpenGL`
- **Windows**: Uses `build/windows/` directory, links against `-lopengl32`

### Dependencies
All third-party libraries are built from source as git submodules:
- **SFML**: Window management, input handling, and basic rendering
- **OpenGL/GLAD**: Shaders and advanced rendering effects
- **GoogleTest**: Unit testing framework

Library installation paths are platform-specific in `third_party/[library]/[platform]/`

## Architecture Overview

### Core Engine Systems
The engine follows a strict modular ECS architecture with these systems:

1. **ECS Core** (`engine/ecs/`): Entity/component management with bitmask-based queries
2. **Rendering** (`engine/rendering/`): Sprite/tile rendering with OpenGL and SFML
3. **Physics** (`engine/physics/`): Movement, collisions, and grid alignment
4. **Input** (`engine/input/`): User input mapping to game events  
5. **Resources** (`engine/resources/`): Asset loading and management
6. **Utils** (`engine/utils/`): Shared utilities and helper functions

### Key Design Principles
1. **Components are POD structs** - Plain Old Data with no behavior, zero-initialized
2. **Systems are stateless functions** - Operate on component arrays via bitmask queries
3. **Struct-of-Arrays (SoA) storage** - Cache-friendly component layout in memory arenas
4. **Bitmask-based queries** - Efficient multi-component filtering with branch-free operations
5. **Typed events** - `Event<T>` system for decoupled, type-safe communication
6. **TDD approach** - Tests and headers written before implementation

## Development Workflow

### Since No Code Exists Yet
This is a greenfield project with comprehensive design documentation but no implementation:

1. **Start with headers first** - Define component structs and system interfaces in `include/` directories
2. **Write tests before implementation** - Create unit tests in module `tests/` directories
3. **Follow SoA patterns** - Store components in arrays, not as object members
4. **Use bitmask queries** - Entity bitmasks indicate component presence for efficient filtering
5. **Implement systems as pure functions** - Each system should be independently testable

### Module Structure Pattern
Each engine module follows this consistent structure:
```
module_name/
├── include/    # Public headers and component definitions
├── src/        # Implementation files
└── tests/      # Unit tests for the module
```

### Component Design Requirements
Components must be:
- POD (Plain Old Data) structs only
- Zero-initialized by default (ZII)
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

### System Design Requirements  
Systems should be:
- Stateless functions operating on component arrays
- Testable in isolation without dependencies
- Query components via bitmask filtering: `(entity.componentMask & requiredMask) == requiredMask`
- Communicate through typed `Event<T>` payloads

### Memory Management Strategy
- Use memory arenas for component allocation
- Implement swap-remove to keep arrays dense 
- Leverage bitmasks for branch-free queries
- Maintain cache-friendly SoA data layout

## Game-Specific Context

### Game Design
- **Genre**: Turn-based tactical strategy with grid-based movement
- **Setting**: Time-travel theme across multiple eras (Wild West → Norse → Space Pirates)
- **Core Loop**: Control units, navigate grid, stop trains, steal cargo
- **Visual Style**: Isometric 2.5D with era-specific art that dynamically changes

### Key Game Systems
- **Grid-Based Movement**: Turn-based unit movement on tiles
- **Train Mechanics**: Predetermined train paths that must be stopped and boarded
- **Time Travel**: Dynamic asset swapping for visual themes across eras
- **Level System**: Data-driven level definitions (JSON/XML) for scenarios

### Engine-Game Separation
The engine is designed to be game-agnostic:
- Core ECS systems have no game-specific logic
- Game content lives in `game/` directory
- Level data and entity archetypes loaded from external files
- Asset management supports dynamic theme switching

## Testing Strategy

### Unit Test Organization
- Each module has its own `tests/` directory
- One test file per system/component: `ComponentArrayTests.cpp`, `MovementSystemTests.cpp`
- ECS core logic must be fully testable without rendering or UI dependencies
- Integration tests go in `tests/integration/`

### Test Commands
- `make test` - Build and run all ECS unit tests
- Tests use GoogleTest framework (available in `third_party/googletest/`)

## Performance Considerations

The engine prioritizes performance through:
- **Cache-friendly iteration**: SoA component storage in memory arenas
- **Branch-free queries**: Bitmask-based component checks
- **Dense arrays**: Swap-remove maintains array density without fragmentation
- **Zero initialization**: Predictable component states and safe defaults

## Event System Architecture

Use strongly typed `Event<T>` payloads for system communication:
- Input events → Movement system updates
- Movement events → Sound/animation systems
- Collision events → Game logic systems
- Decoupled, type-safe inter-system communication

## Cross-Platform Considerations

The build system handles platform differences automatically:
- Library paths configured per platform in Makefile
- Third-party dependencies built from source for consistency
- Platform-specific OpenGL linking handled transparently