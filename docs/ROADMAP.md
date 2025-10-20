# Train Heist Development Roadmap

This roadmap outlines the development strategy for the Train Heist game engine and prototype.

## 🎯 Development Strategy: Two-Track Approach

We're using a **two-track approach** that delivers value quickly while building foundational components:

- **Track 1**: Minimal Viable Display (get something visual running fast)
- **Track 2**: Core Components (build universal ECS components in parallel)

## 📊 Current Status

### ✅ Completed (Phase 0-1: ECS Foundation + Rendering + Input + Physics Implementation)
- **ECS Core**: Entity/component management with bitmask tracking ✅ **COMPLETED**
- **Systems Layer**: Priority-based system execution with dependency injection ✅ **COMPLETED**
- **Event System**: Typed event queues for decoupled communication ✅ **COMPLETED**
- **Component Registry**: Automatic bit assignment for component types ✅ **COMPLETED**
- **SystemUtils**: Helper functions for common ECS operations ✅ **COMPLETED**
- **Entity Iteration**: Fixed getAllEntitiesForIteration() to return pointers for current componentMask access ✅ **COMPLETED**
- **Logging System**: Multi-level logging with console/file output and global macros ✅ **COMPLETED**
- **Transform Components**: Position, Rotation, Scale, GridPosition with 2.5D coordinate utilities ✅ **COMPLETED**
- **Rendering Components**: Sprite and Renderable POD components with ZII compliance ✅ **COMPLETED**
- **Rendering System**: RenderSystem with dependency injection and comprehensive tests ✅ **COMPLETED**
- **Interface Abstractions**: IRenderer, IWindowManager, IResourceManager, IInputManager interfaces ✅ **COMPLETED**
- **SFML Integration**: Complete SFMLRenderer, SFMLWindowManager, SFMLResourceManager, SFMLInputManager implementations ✅ **COMPLETED**
- **Mock Testing Infrastructure**: MockRenderer, MockWindowManager, MockResourceManager, MockInputManager ✅ **COMPLETED**
- **Event Conversion Layer**: SFML → Engine event abstraction with comprehensive testing ✅ **COMPLETED**
- **Input System**: SFMLInputManager with keyboard/mouse processing, InputSystem with ECS integration ✅ **COMPLETED**
- **Physics Components**: GridMovement, Velocity, Acceleration, MovementConstraints with full ZII compliance ✅ **COMPLETED**
- **Movement System**: Grid-based movement with queued actions, bounds validation, smooth interpolation ✅ **COMPLETED**
- **Turn-Based Support**: Manual movement execution, queued movement system for turn-based gameplay ✅ **COMPLETED**
- **Interactive Demo**: Working keyboard-controlled entity with mouse logging via complete input pipeline ✅ **COMPLETED**
- **Two-Tier Testing**: Unit tests + Integration tests with separate build targets ✅ **COMPLETED**
- **Test Coverage**: 276 comprehensive tests (100% pass rate) covering all engine systems ✅ **CURRENT**

### 🚧 In Progress (Phase 2: Main.cpp Integration & Multi-Entity Rendering)
- **Current Status**: Physics system fully implemented and tested, ready for main.cpp integration
- **Next Priority**: Update main.cpp demo to use MovementSystem with grid-based movement
- **Focus**: Demonstrate discrete grid movement with smooth visual interpolation

## 🚀 Development Phases

### **Phase 1: Proof of Concept** (Target: Week 1)
**Goal**: Colored square you can move with arrow keys

#### Track 1: Minimal Viable Display
- [x] **Window & Rendering Bootstrap** ✅ **COMPLETED**
  - Basic SFML window creation and main loop
  - Simple renderer that can draw colored rectangles  
  - Camera system for 2D positioning
- [x] **Input Integration** ✅ **COMPLETED**
  - SFML input manager implementing `IInputManager`
  - Basic keyboard/mouse input handling
  - Input system that generates events
- [x] **Minimal Game Loop** ✅ **COMPLETED**
  - Integration point showing SystemManager + EntityManager + Renderer
  - Working interactive demo: keyboard-controlled colored square on screen

#### Track 2: Core Components (Parallel)
- [x] **Logging System** (`engine/logging/`) ✅ **COMPLETED**
  - Simple, fast logging with multiple levels (DEBUG, INFO, WARN, ERROR)
  - File and console output options
  - Component for runtime visibility and debugging
- [x] **Transform Components** (`engine/ecs/components/`) ✅ **COMPLETED**
  - `Position { float x, y, z; }` with 3D coordinates for 2.5D rendering
  - `Rotation { float angle; }` for sprite rotation
  - `Scale { float x, y; }` for non-uniform scaling
  - `GridPosition { int x, y; }` for discrete tile-based logic
  - Coordinate conversion utilities (grid↔world↔isometric)
  - Distance calculations, interpolation, and angle utilities
- [x] **Rendering Components** (`engine/ecs/components/`) ✅ **COMPLETED**
  - `Sprite { int textureId; float width, height; }` for texture-based rendering
  - `Renderable { float width, height, red, green, blue, alpha; }` for colored shapes
  - ZII compliance with alpha=0.0f (transparent by default)
  - Component registry integration and comprehensive test coverage
- [x] **Rendering System** (`engine/rendering/`) ✅ **COMPLETED**
  - `RenderSystem` with dependency injection and component filtering
  - `MockRenderer` and `SFMLRenderer` implementations with comprehensive testing
  - `SFMLWindowManager` and `SFMLResourceManager` with interface abstractions
  - Frame lifecycle management (beginFrame → clear → endFrame)
  - Entity filtering for Position + (Sprite OR Renderable) components
  - Complete SFML integration with event conversion layer
  - Two-tier testing system (unit + integration tests)
- [x] **Physics Components** ✅ **COMPLETED**
  - `GridMovement { int targetX, targetY; float progress; }` for discrete grid-based movement
  - `Velocity { float dx, dy; }` for smooth visual transitions between grid cells
  - `Acceleration { float dx, dy; }` for physics-based motion effects
  - Grid validation and collision detection components
- [x] **Grid-Based Movement Demo** ✅ **COMPLETED**
  - Main.cpp demonstrates grid-based movement with 10x8 grid (64px cells)
  - Arrow keys control entity on discrete grid cells
  - Smooth visual interpolation between cells
  - MovementSystem integrated with full physics pipeline

**Success Criteria Phase 1:**
- ✅ Window opens and displays content ✅ **ACHIEVED**
- ✅ Keyboard input moves a visual element ✅ **ACHIEVED**
- ✅ SystemManager orchestrates updates properly ✅ **ACHIEVED**
- ✅ Grid-based movement with smooth visual transitions ✅ **ACHIEVED**

**🎉 PHASE 1 COMPLETE! All success criteria met.**

### **Phase 2: Game Foundations** (Target: Week 2)
**Goal**: Grid-based movement with multiple entities

#### Deliverables
- [ ] **Grid-Based Physics System**
  - Movement validation and collision detection
  - Smooth interpolation between grid positions
  - **2.5D Preparation**: Grid coordinates designed for isometric projection
- [ ] **Multi-Entity Rendering**
  - Render multiple sprites simultaneously
  - Z-ordering and layering support (critical for isometric depth)
  - **2.5D Foundation**: Depth sorting and isometric sprite positioning
- [ ] **Basic Animation System**
  - Smooth movement between grid cells
  - Simple state-based animations
- [ ] **Asset Loading System**
  - Texture loading and management
  - Resource handles and caching

**Success Criteria Phase 2:**
- ✅ Multiple colored squares on a grid
- ✅ Turn-based or smooth movement
- ✅ Clean separation between engine and game logic

### **Phase 3: Train Heist Prototype** (Target: Week 3+)
**Goal**: Basic train heist gameplay

#### Deliverables
- [ ] **Train System**
  - Train entities moving on predetermined paths
  - Multiple train cars with different properties
- [ ] **Player Unit System**
  - Multiple controllable units
  - Action-based gameplay (move, board, steal)
- [ ] **Game Rules Engine**
  - Turn management and win conditions
  - Basic AI for train behavior
- [ ] **Asset Pipeline**
  - Sprite sheets and animations
  - Level data loading (JSON/XML)

### **Phase 4: Advanced Rendering** (Target: Week 4+)
**Goal**: Enhanced visual effects and 2.5D isometric rendering

#### Deliverables
- [ ] **OpenGL Integration**
  - OpenGL context setup alongside SFML
  - Shader system for effects and animations
  - Particle effects for train smoke, explosions, etc.
  - Advanced lighting and post-processing effects
- [ ] **2.5D Isometric Rendering**
  - Isometric camera projection and transformations
  - Grid-to-screen coordinate conversion utilities
  - Depth-based sprite sorting for proper layering
  - Parallax scrolling for background layers
- [ ] **Enhanced Animation System**
  - Smooth camera transitions and following
  - Advanced sprite animations with shader effects
  - UI animations and transitions

**Success Criteria Phase 3:**
- ✅ Trains move along predefined tracks
- ✅ Players can control multiple units
- ✅ Basic win/lose conditions implemented

**Success Criteria Phase 4:**
- ✅ OpenGL effects enhance visual appeal (shaders, particles)
- ✅ True 2.5D isometric view with proper depth sorting
- ✅ Smooth camera system for isometric gameplay

### **Phase 5: Engine Library Export** (Target: Future)
**Goal**: Package the engine as a reusable library for other projects

#### Deliverables
- [ ] **Library Packaging**
  - CMake build system for library compilation
  - Separate engine library from game executable
  - Header-only option for template-heavy components
  - Shared/static library build options
- [ ] **Public API Definition**
  - Clean public headers in `engine/include/`
  - Hide implementation details (PIMPL pattern where appropriate)
  - Versioned API with semantic versioning
  - API documentation generation (Doxygen)
- [ ] **Example Projects**
  - Minimal "Hello World" ECS project
  - Grid-based movement demo (extracted from current main.cpp)
  - Complete game template with all systems
- [ ] **Distribution**
  - Package manager support (vcpkg, conan)
  - Installation scripts for multiple platforms
  - Pre-built binaries for common platforms
  - Comprehensive usage documentation

**Success Criteria Phase 5:**
- ✅ Engine compiles as standalone library
- ✅ Example projects build against installed library
- ✅ Other projects can use engine without modifying engine source
- ✅ Clear upgrade path for library updates

**Note**: This phase will be tackled after core game functionality is complete, ensuring the API is stable and battle-tested.

## 🏗️ Architecture Decisions

### **Component Organization Strategy**
- **Include in `engine/ecs/components/`**: Transform, basic physics, core rendering components
- **Include in `engine/logging/`**: Logging system for runtime visibility and debugging
- **Keep in `game/components/`**: Train-specific logic, game rules, level-specific data
- **Clean Separation**: Engine components are generic and reusable, game components are domain-specific

### **Development Principles**
1. **Test-Driven Development**: Write failing tests before implementation
2. **Visual Feedback First**: Prioritize getting something on screen quickly
3. **Incremental Integration**: Add complexity gradually while maintaining working state
4. **Game-Driven Design**: Every component gets tested in actual gameplay usage
5. **Visibility-First Debugging**: Comprehensive logging for understanding runtime behavior
6. **2.5D-Ready Architecture**: Design coordinate systems and rendering from the start to accommodate isometric projection

### **Grid-Based Physics Architecture**
Key design principles for turn-based tactical movement:

**Dual-Layer Movement System:**
- **Logical Layer**: Discrete grid positions for game rules, turn management, collision detection
- **Visual Layer**: Smooth interpolation between grid cells for polished animations
- **Grid Authority**: All gameplay logic operates on grid coordinates, visuals follow

**Core Components for Grid Movement:**
```cpp
struct GridPosition { int x, y; };              // Authoritative logical position
struct GridMovement {                           // Movement state tracking
    int targetX, targetY;                       // Destination grid cell
    float progress;                             // Animation progress (0.0 → 1.0)
    float speed;                                // Movement speed modifier
    bool isMoving;                              // Currently in transition?
};
struct Position { float x, y, z; };            // Visual rendering position (interpolated)
```

**Movement Pipeline:**
1. **Input Processing**: Arrow keys queue movement commands to adjacent grid cells
2. **Grid Validation**: Check if target cell is valid (bounds, obstacles, game rules)
3. **Movement Initiation**: Set GridMovement target and begin visual interpolation
4. **Visual Interpolation**: Update Position component based on GridMovement progress
5. **Movement Completion**: Snap to exact grid coordinates when progress reaches 1.0

**Turn-Based Integration Ready:**
- Movement commands can be queued and executed in discrete turns
- Animation system allows for simultaneous multi-entity movement
- Grid validation supports complex game rules (movement points, obstacles, etc.)

### **2.5D Isometric Architecture**
Key design considerations built into the foundation:

**Coordinate Systems:**
- **Grid Coordinates**: Logical game world positions (e.g., `GridPosition { int x, y; }`)
- **World Coordinates**: 3D positions for rendering depth (`Position { float x, y, z; }`) 
- **Screen Coordinates**: Final 2D pixel positions after isometric projection

**Component Design for 2.5D:**
```cpp
struct Position { float x, y, z; };        // 3D world position
struct GridPosition { int x, y; };         // Logical grid coordinates  
struct Depth { float value; };             // Explicit depth for sorting
struct IsometricSprite {                   // Isometric-aware rendering
    TextureID texture;
    float anchorX, anchorY;                // Anchor point for isometric alignment
};
```

**Rendering Pipeline Design:**
1. **Grid Logic**: Game rules operate on grid coordinates
2. **World Transform**: Grid → 3D world coordinates  
3. **Isometric Projection**: 3D world → 2D screen coordinates
4. **Depth Sorting**: Z-order sprites for proper layering
5. **SFML Rendering**: 2D sprite drawing (Phase 1-3)
6. **OpenGL Effects**: Shaders and effects (Phase 4)

### **Logging System Design**
The logging system will provide essential runtime visibility:

**Core Features:**
- **Multiple Log Levels**: DEBUG, INFO, WARN, ERROR with runtime filtering
- **Multiple Outputs**: Console, file, or both simultaneously
- **Performance**: Minimal overhead with compile-time level filtering
- **Thread-Safe**: Prepared for future multi-threading support
- **ECS Integration**: Easy logging from systems with entity context

**Usage Pattern:**
```cpp
LOG_INFO("EntityManager", "Created entity {} with {} components", entity.id, componentCount);
LOG_WARN("Physics", "Entity {} moved out of bounds at ({}, {})", entity.id, pos.x, pos.y);
LOG_ERROR("Renderer", "Failed to load texture: {}", filename);
```

## 📈 Metrics and Success Tracking

### **Technical Metrics**
- Test coverage: Currently 276 passing tests (221 unit + 55 integration tests)
  - Unit tests: ECS core, logging, systems, components, physics, and mock implementations
  - Integration tests: SFML wrapper validation, color conversion, event handling
- Build system: Two-tier testing (`make test` for fast unit tests, `make integration` for SFML tests)
- Build time: Target <10 seconds for incremental builds
- Frame rate: Target 60 FPS for simple scenes

### **Feature Milestones**
- **Milestone 1**: Moving square (validates full stack integration)
- **Milestone 2**: Grid-based game with multiple entities
- **Milestone 3**: Playable train heist prototype

## 🔄 Roadmap Updates

This roadmap will be updated as development progresses:
- ✅ Completed items marked and dated
- 🚧 In-progress items tracked with current status
- 📝 New discoveries and scope changes documented
- 🎯 Success criteria evaluated and updated based on learnings

## 📚 References

- **Architecture Documentation**: See `../engine/ecs/DESIGN.md` for detailed ECS design
- **Development Guidelines**: See `../engine/CLAUDE.md` for coding standards and workflow
- **Project Overview**: See `../README.md` for project introduction and build instructions

---

*Last Updated: 2025-08-19*
*Next Review: After Input System completion*