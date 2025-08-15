# Train Heist Development Roadmap

This roadmap outlines the development strategy for the Train Heist game engine and prototype.

## 🎯 Development Strategy: Two-Track Approach

We're using a **two-track approach** that delivers value quickly while building foundational components:

- **Track 1**: Minimal Viable Display (get something visual running fast)
- **Track 2**: Core Components (build universal ECS components in parallel)

## 📊 Current Status

### ✅ Completed (Phase 0: ECS Foundation)
- **ECS Core**: Entity/component management with bitmask tracking
- **Systems Layer**: Priority-based system execution with dependency injection  
- **Event System**: Typed event queues for decoupled communication
- **Component Registry**: Automatic bit assignment for component types
- **SystemUtils**: Helper functions for common ECS operations
- **Logging System**: Multi-level logging with console/file output and global macros
- **Transform Components**: Position, Rotation, Scale, GridPosition with 2.5D coordinate utilities ✅ **COMPLETED**
- **Test Coverage**: 120 passing tests with comprehensive coverage

### 🚧 In Progress
- **Next Item**: Physics Components or Rendering Components (ready to choose direction)

## 🚀 Development Phases

### **Phase 1: Proof of Concept** (Target: Week 1)
**Goal**: Colored square you can move with arrow keys

#### Track 1: Minimal Viable Display
- [ ] **Window & Rendering Bootstrap**
  - Basic SFML window creation and main loop
  - Simple renderer that can draw colored rectangles  
  - Camera system for 2D positioning
- [ ] **Input Integration** 
  - SFML input manager implementing `IInputManager`
  - Basic keyboard/mouse input handling
  - Input system that generates events
- [ ] **Minimal Game Loop**
  - Integration point showing SystemManager + EntityManager + Renderer
  - Simple demo: controllable colored square on screen

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
- [ ] **Physics Components**
  - `Velocity { float dx, dy; }` for smooth movement
  - `Acceleration { float dx, dy; }` for physics-based motion
  - Grid-based movement components
- [ ] **Rendering Components**
  - `Sprite { TextureID texture; int width, height; }`
  - `Color { uint8_t r, g, b, a; }`
  - `Visible { bool enabled; }`

**Success Criteria Phase 1:**
- ✅ Window opens and displays content
- ✅ Keyboard input moves a visual element  
- ✅ SystemManager orchestrates updates properly

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
- Test coverage: Currently 120 passing tests (95 ECS + 15 Logging + 10 Systems)
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

- **Architecture Documentation**: See `engine/ecs/DESIGN.md` for detailed ECS design
- **Development Guidelines**: See `engine/CLAUDE.md` for coding standards and workflow
- **Project Overview**: See `README.md` for project introduction and build instructions

---

*Last Updated: 2025-01-15*
*Next Review: After Phase 1 completion*