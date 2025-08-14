## Directory Structure

/train_heist/
├─ engine/
│   ├─ ecs/
│   ├─ physics/
│   ├─ rendering/
│   ├─ input/
│   ├─ resources/
│   └─ utils/
├─ third_party/
│   ├─ SFML/        # Could be a git submodule or installed library
│   └─ OpenGL/      # Mostly header + system-installed libs
├─ tests/
├─ src/            # Top-level main game executables (e.g., demo, game)
├─ Makefile        # Top-level Makefile
└─ DESIGN.md

## Makefile
- Each module has its own Makefile for isolated compilation and testing
- Top-level Makefile includes all subdirectories, links against OpenGL and SFML
- Rendering module will receive an IRenderer abstract interface for dependency injection
