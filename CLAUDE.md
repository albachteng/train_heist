# CLAUDE.md

**CRITICAL: After the red phase, you must always pause and wait for my approval before moving forward with implementation.**

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Train Heist is a turn-based tactical strategy game engine written in C++ using a lightweight ECS (Entity-Component-System) architecture. The ECS core is now implemented with Entity/ComponentArray/Event systems, component registry, and comprehensive test coverage. The game features isometric 2.5D grid-based gameplay where players control multiple units to stop and board trains across different time periods.

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
5. **Mark-dead-and-reuse entities** - Entities are marked as dead but remain in storage for efficient reuse with generation counters
6. **Typed events** - `Event<T>` system for decoupled, type-safe communication
7. **TDD approach** - Tests and headers written before implementation

## Development Workflow

### CRITICAL: Test-Driven Development (TDD) Process
**ALWAYS follow the Red-Green-Refactor cycle for ALL code changes:**

1. **RED STAGE** - Write failing tests first
   - Create unit tests that capture expected behavior
   - Stubs may be necessary for compilation
   - **PAUSE and ask for verification of the tests** before proceeding to implementation
   - Tests should be comprehensive and cover edge cases

2. **GREEN STAGE** - Implement minimal code to make tests pass
   - Write only enough code to pass the tests
   - **PAUSE and ask for verification** after implementing each component
   - Keep implementations focused and small

3. **REFACTOR STAGE** - Clean up and optimize
   - Improve code quality while keeping tests green
   - **PAUSE and verify** any significant refactoring

### Task Granularity Requirements
- **Complete ONE TODO item at a time** - No batching multiple tasks
- **Pause for verification** between each TODO completion
- **Small, reviewable chunks** - Each code change should be easily reviewable
- **Ask before proceeding** when any code change exceeds ~100 lines

### CRITICAL: Modular Development Requirements
**In general, work on ONE module at a time:**

1. **Single Module Focus** - Work only within one module directory at a time whenever possible
   - Example: When working on `engine/input/`, do NOT modify `game/` or other engine modules
   - Complete the module's implementation and tests before moving to integration

2. **No Cross-Module Changes Without Consultation** 
   - **NEVER modify files outside the current working module** without explicit approval
   - **ASK FIRST** if you need to change APIs or interfaces in other modules
   - **EXPLAIN** why cross-module changes are necessary before implementing

3. **Integration as Separate Phase**
   - Complete module development first (headers, implementation, tests all passing)
   - **PAUSE** and verify module is complete before integration
   - Integration (main.cpp, other modules) is a separate phase requiring approval

4. **API Assumptions Prohibited**
   - **DO NOT assume APIs exist** in other modules
   - **READ existing code** to understand current interfaces
   - **ASK** about missing APIs rather than implementing assumed interfaces
   - Refer to DESIGN.md, which exist in multiple directories across the codebase

### Current Implementation Status
The ECS core and systems layer are fully implemented with comprehensive test coverage (167 passing tests including rendering system).

**Major Components Completed:**
- ✅ **ECS Core**: Entity/ComponentArray/Event systems with bitmask queries
- ✅ **Systems Layer**: SystemManager with priority-based execution
- ✅ **Logging System**: Multi-level logging with console/file output
- ✅ **Transform Components**: Position, Rotation, Scale, GridPosition with 2.5D utilities
- ✅ **Rendering Components**: Sprite and Renderable POD components with ZII compliance
- ✅ **Rendering System**: Complete RenderSystem with dependency injection and MockRenderer
- ✅ **Component Registry**: Automatic bit assignment and type safety

**Development Progress**: See [ROADMAP.md](ROADMAP.md) for current development plan, phase tracking, and next steps.

Future development should follow these patterns:

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

### EntityManager Architecture
The EntityManager uses a **mark-dead-and-reuse** approach for entity lifecycle management:

**Key Features:**
- **Stable memory layout**: Dead entities remain in storage, entity pointers stay valid longer
- **Generation-based safety**: Each entity ID has a generation counter that increments on reuse
- **Efficient reuse**: O(1) entity ID reuse via free ID queue
- **System-friendly iteration**: `getAllEntitiesForIteration()` includes dead entities that systems should skip

**Core Methods:**
- `createEntity()` - Reuses dead entity slots with incremented generations, or creates new slots
- `destroyEntity()` - Marks entities as dead but keeps them in storage
- `isValid()` - Checks entity generation matches current generation for that ID
- `isAlive()` - Quick check if entity slot is alive (used during system iteration)
- `getEntityByID()` - Returns current generation entity or nullptr if dead

**Memory Behavior:**
- Storage grows with peak entity count but never shrinks
- Perfect for turn-based games with low entity churn
- Thread-safe with planned coarse-grained locking (single mutex)

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

### Test File Organization
- Follow 1:1 relationship: `FileName.cpp/.hpp` ↔ `tests/FileNameTests.cpp`
- This makes it clear which test file covers which source file
- Temporary test files are acceptable for debugging complex issues
- After debugging, consolidate temporary tests into appropriate existing test files

### TDD Process
- Follow strict red-green-refactor cycles: see tests fail first, then implement
- Discuss design changes before implementation to avoid unnecessary rework
- Keep changes localized to as few files as possible before testing
- Test consolidation should happen after implementation is complete and working
- **IMPORTANT**: When creating new test files or directories, always update the Makefile to include them in the build system
- **CRITICAL**: Never modify failing tests during the "Green" phase without explicit user approval - tests must remain unchanged once we move from Red to Green

### Unit Test Organization
- Each module has its own `tests/` directory
- ECS core logic must be fully testable without rendering or UI dependencies
- Integration tests go in `tests/integration/`

### Test Commands
- `make test` - Build and run all ECS unit tests
- Tests use GoogleTest framework (available in `third_party/googletest/`)

## Development Philosophy

### Implementation Approach
- Start simple, plan for optimization (e.g., std::vector → memory arenas)
- Use static enforcement over runtime checks (static_assert vs runtime validation)
- Measure performance improvements with concrete benchmarks (e.g., "14x faster")
- Discuss architectural changes before implementing to ensure alignment

### Code Quality Standards
- Template files: prefer `.hpp` over `.tpp` for better IDE support
- ZII compliance enforced through static_assert, not just documentation
- Performance-first design decisions with cache-friendly patterns
- API compatibility maintained during optimization migrations

### Build System Preferences
- Modular Makefile using TEST_MODULES list for scalability
- Automatic file discovery where reasonable (e.g., `$(wildcard *.cpp)`)
- Directory structure that supports future growth without major refactoring

## Documentation Standards

### Documentation Synchronization
- Keep documentation synchronized with actual implementation
- Provide concrete examples using real API calls from the codebase
- Include performance benchmarks and tradeoffs in design decisions
- Update design docs immediately after major implementation changes

### Migration Strategy
- Design APIs to support future optimizations without breaking changes
- Document migration paths clearly (e.g., std::vector → memory arenas)
- Maintain backward compatibility during optimization phases

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
