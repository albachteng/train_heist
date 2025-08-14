## Directory Structure

/train_heist/
├─ engine/
│   ├─ ecs/
│   ├─ physics/
│   ├─ rendering/ # IRenderer necessary for cross-platform compatibility
│   ├─ input/
│   ├─ resources/
│   └─ utils/
├─ third_party/
│   ├─ SFML/        # a git submodule, build binary from source per platform 
│   ├─ OpenGL/      # Mostly header + system-installed libs
│   └─ googletest/  # a git submodule
├─ tests/
├─ src/            # Top-level main game executables (e.g., demo, game)
├─ Makefile        # Top-level Makefile
├─ DESIGN.md       # You are here
└─ GAME_DESIGN.md  

## Makefile
- Each module has its own Makefile for isolated compilation and testing
- Top-level Makefile includes all subdirectories, links against OpenGL and SFML
- Each third_party library is included as a submodule and built from source on a per-platform basis for consistency
- Rendering module will receive an IRenderer abstract interface for dependency injection
- Uses OpenGL/GLAD for shaders and effects, SFML for window management and input handling
