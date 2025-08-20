# Train Heist - Tactical Strategy Game Engine

A lightweight, turn-based tactical strategy game & game engine written in C++ using a modern Entity-Component-System (ECS) architecture. Players control multiple units to stop and board trains across different time periods in an isometric 2.5D environment.

## Quick Start

```bash
# Clone the repository with submodules
git clone --recursive https://github.com/your-username/train_heist.git
cd train_heist

# Build third-party dependencies (see Platform Setup below)
# Then build and test
make test         # Build and run unit tests (201 tests, fast)
make integration  # Build and run integration tests (55 SFML tests)
make run          # Build and run the main game executable
```

## Platform Setup

### Prerequisites

All platforms require:
- **C++17 compatible compiler** (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake 3.16+** (for building third-party libraries)
- **Git** with submodule support

### Linux (Ubuntu/Debian)

```bash
# Install build dependencies
sudo apt update
sudo apt install build-essential cmake git

# Install system dependencies for SFML
sudo apt install libx11-dev libxrandr-dev libxcursor-dev libxi-dev \
                 libudev-dev libgl1-mesa-dev

# Build third-party libraries
cd third_party/SFML
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=install/linux -DSFML_BUILD_EXAMPLES=OFF
cmake --build build --target install

cd ../googletest
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=linux -DCMAKE_BUILD_TYPE=Release
cmake --build build --target install
```

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake

# Build third-party libraries
cd third_party/SFML
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=install/macos -DSFML_BUILD_EXAMPLES=OFF
cmake --build build --target install

cd ../googletest
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=macos -DCMAKE_BUILD_TYPE=Release
cmake --build build --target install
```

### Windows (MSYS2/MinGW)

```bash
# Install MSYS2 from https://www.msys2.org/
# Open MSYS2 terminal and install dependencies
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make git

# Build third-party libraries
cd third_party/SFML
cmake -B build -S . -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=install/windows -DSFML_BUILD_EXAMPLES=OFF
cmake --build build --target install

cd ../googletest
cmake -B build -S . -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=windows -DCMAKE_BUILD_TYPE=Release
cmake --build build --target install
```

### Windows (Visual Studio)

```bash
# Open Visual Studio Developer Command Prompt
# Navigate to project directory

# Build third-party libraries
cd third_party\\SFML
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=install\\windows -DSFML_BUILD_EXAMPLES=OFF
cmake --build build --config Release --target install

cd ..\\googletest
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=windows -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target install
```

## Build System

The project uses a cross-platform Makefile that automatically detects your operating system and configures the appropriate library paths.

### Primary Build Commands

```bash
make              # Build the main game executable
make test         # Build and run unit tests (201 tests, fast)
make integration  # Build and run integration tests (55 SFML tests)
make run          # Build and run the main game executable
make clean        # Clean all build artifacts
```

### Testing Architecture

The project uses a **two-tier testing system**:

- **Unit Tests** (`make test`): 201 fast tests with no external dependencies
  - Mock implementations for graphics/window systems
  - ECS core, logging, components, and system logic
  - Runs in ~3ms, ideal for development iteration
- **Integration Tests** (`make integration`): 55 SFML integration tests  
  - Real SFML library validation and integration testing
  - Color conversion, event handling, rendering pipeline
  - Requires SFML libraries, runs in ~300ms

### Build Directories

- **Linux**: `build/linux/`
- **macOS**: `build/macos/`
- **Windows**: `build/windows/`

## Architecture Overview

### Core Engine Systems

The engine follows a strict modular ECS architecture:

1. **ECS Core** (`engine/ecs/`): Entity/component management with bitmask-based queries ✅ **IMPLEMENTED**
2. **Systems Layer** (`engine/ecs/systems/`): Priority-based system execution with dependency injection ✅ **IMPLEMENTED**
3. **Logging System** (`engine/logging/`): Multi-level logging with console/file output ✅ **IMPLEMENTED**
4. **Rendering** (`engine/rendering/`): Complete SFML integration with interface abstractions ✅ **IMPLEMENTED**
5. **Physics** (`engine/physics/`): Movement, collisions, and grid alignment *(planned)*
6. **Input** (`engine/input/`): User input mapping to game events *(next priority)*
7. **Resources** (`engine/resources/`): Asset loading and management *(interface ready)*
8. **Utils** (`engine/utils/`): Shared utilities and helper functions *(planned)*

### Key Design Principles

- **Components are POD structs** - Plain Old Data with no behavior, zero-initialized
- **Systems are stateless functions** - Operate on component arrays via bitmask queries
- **Struct-of-Arrays (SoA) storage** - Cache-friendly component layout
- **Mark-dead-and-reuse entities** - Entities are marked as dead but remain in storage for efficient reuse
- **Bitmask-based queries** - Efficient multi-component filtering with branch-free operations
- **Typed events** - `Event<T>` system for decoupled, type-safe communication
- **TDD approach** - Tests and headers written before implementation

## Third-Party Libraries

### Git Submodules

The following libraries are included as git submodules and built from source:

- **SFML 3.0.0** (`third_party/SFML/`) - Window management, input handling, basic rendering
  - **Fully integrated** - SFMLRenderer, SFMLWindowManager, SFMLResourceManager implemented
  - Complete event conversion layer (SFML → Engine events)
  - Comprehensive integration testing with 55 test cases
- **GoogleTest** (`third_party/googletest/`) - Unit testing framework
  - **Actively used** - 256 comprehensive tests (201 unit + 55 integration)

### Embedded Libraries

- **OpenGL/GLAD** (`third_party/OpenGL/`) - OpenGL function loader
  - Embedded as source files (glad.h, glad.c)
  - Will be used for advanced rendering effects

## Development Workflow

### Current Implementation Status

The ECS core, systems layer, logging system, and rendering system are **fully implemented** with comprehensive test coverage (256 tests: 201 unit + 55 integration). Key accomplishments:

- **Complete SFML Integration**: SFMLRenderer, SFMLWindowManager, SFMLResourceManager with interface abstractions
- **Two-Tier Testing**: Fast unit tests + thorough integration tests
- **Event Conversion Layer**: Complete SFML → Engine event abstraction
- **Mock Infrastructure**: Full mock implementations for testing without external dependencies

**Next Priority**: Input System implementation (IInputManager interface already defined)

**Development Progress**: See [ROADMAP.md](ROADMAP.md) for current development plan, progress tracking, and next milestones.

### Getting Started

1. **Examine the ECS tests** - Start with `engine/ecs/tests/` to understand the architecture
2. **Read design documentation** - Check `engine/ecs/DESIGN.md` for detailed technical info
3. **Follow TDD approach** - Write tests first, then implement functionality
4. **Use existing patterns** - Follow the established module structure and conventions

### Module Structure

Each engine module follows this pattern:

```
module_name/
├── include/    # Public headers and component definitions
├── src/        # Implementation files
└── tests/      # Unit tests for the module
```

### Component Design Requirements

Components must be:
- POD (Plain Old Data) structs only
- Zero-initialized by default (ZII compliant)
- Stored in Struct-of-Arrays layout
- Accessed via entity bitmask queries

Example component:

```cpp
struct Position {
    float x = 0.0f;  // ✅ Zero-initialized
    float y = 0.0f;  // ✅ Zero-initialized  
    float z = 0.0f;  // ✅ Zero-initialized
};
```

### System Design Requirements

Systems should be:
- Stateless functions operating on component arrays
- Testable in isolation without dependencies
- Query components via bitmask filtering: `(entity.componentMask & requiredMask) == requiredMask`
- Communicate through typed `Event<T>` payloads

## Game Design

- **Genre**: Turn-based tactical strategy with grid-based movement
- **Setting**: Time-travel theme across multiple eras (Wild West → Norse → Space Pirates)
- **Core Loop**: Control units, navigate grid, stop trains, steal cargo
- **Visual Style**: Isometric 2.5D with era-specific art that dynamically changes

See `GAME_DESIGN.md` for detailed game mechanics and design decisions.

## Testing

The project uses a comprehensive **two-tier test-driven development approach**:

```bash
make test         # Run unit tests (201 tests, ~3ms)
make integration  # Run integration tests (55 tests, ~300ms)
```

### Test Organization

- **Unit Tests**: Each module has its own `tests/` directory
  - **Mock implementations** for testing without external dependencies
  - **Fast execution** ideal for development iteration
  - **Comprehensive coverage** of ECS, logging, components, and system logic
- **Integration Tests**: SFML-specific tests validating real library integration
  - **SFMLRenderer testing** - Color conversion, rendering pipeline validation
  - **SFMLWindowManager testing** - Window management and event conversion
  - **Event handling verification** - SFML → Engine event abstraction layer

## Documentation

- **`README.md`** - This file, setup and getting started guide
- **`ROADMAP.md`** - Development roadmap, progress tracking, and next steps
- **`CLAUDE.md`** - AI assistant guidance and architectural overview
- **`DESIGN.md`** - High-level project design decisions
- **`GAME_DESIGN.md`** - Game mechanics and design philosophy
- **`engine/ecs/DESIGN.md`** - Detailed ECS technical documentation
- **`engine/CLAUDE.md`** - Engine-specific AI guidance

## Performance Considerations

The engine prioritizes performance through:

- **Cache-friendly iteration**: SoA component storage for optimal memory access patterns
- **Branch-free queries**: Bitmask-based component filtering
- **Stable memory layout**: Dead entities remain in storage, no memory compaction
- **Zero initialization**: Predictable component states and safe defaults
- **Efficient entity reuse**: O(1) entity creation/destruction with generation safety

## Threading

Currently single-threaded with planned thread-safety:
- **Planned approach**: Coarse-grained locking (single mutex protecting EntityManager)
- **Architecture benefits**: Mark-dead-and-reuse approach provides stable memory layout ideal for threading
- **Implementation priority**: Add after core systems are complete

## Contributing

1. Follow the existing code style and architectural patterns
2. Write tests before implementing functionality (TDD)
3. Update documentation when adding new features
4. Ensure all tests pass before submitting changes
5. Keep components as POD structs and systems as stateless functions

## License

*License information to be added*
