## Directory Structure

/train_heist/
├─ engine/
│   ├─ ecs/          # Core ECS implementation (fully implemented)
│   ├─ physics/      # Movement, collisions, grid alignment (planned)
│   ├─ rendering/    # IRenderer necessary for cross-platform compatibility (planned)
│   ├─ input/        # User input mapping to game events (planned)
│   ├─ resources/    # Asset loading and management (planned)
│   └─ utils/        # Shared utilities and helper functions (planned)
├─ game/
│   ├─ components/   # Game-specific component definitions
│   ├─ systems/      # Game-specific system implementations
│   ├─ src/          # Main game executables (e.g., demo, game)
│   └─ data/         # Game data and level definitions
├─ third_party/
│   ├─ SFML/         # Git submodule, built from source per platform 
│   ├─ OpenGL/       # GLAD headers and source (embedded)
│   └─ googletest/   # Git submodule, built from source per platform
├─ tests/
│   └─ integration/  # Integration tests (planned)
├─ tools/            # Development utilities and scripts
├─ build/            # Platform-specific build outputs
├─ docs/             # Additional documentation
├─ Makefile          # Cross-platform build system
├─ README.md         # Getting started and setup guide
├─ CLAUDE.md         # AI assistant guidance and architecture overview
├─ DESIGN.md         # You are here - high-level project design
└─ GAME_DESIGN.md    # Game mechanics and design philosophy  

## Build System
- Single cross-platform Makefile that automatically detects OS (Linux/macOS/Windows)
- Platform-specific build directories: `build/linux/`, `build/macos/`, `build/windows/`
- Compiles and links against OpenGL/GLAD and SFML libraries
- Third-party libraries (SFML, GoogleTest) built from source as git submodules for consistency
- Primary commands: `make test` (ECS unit tests), `make run` (main executable), `make clean`
- Future rendering module will use IRenderer interface for cross-platform compatibility
- OpenGL/GLAD for advanced rendering, SFML for window management and input handling
