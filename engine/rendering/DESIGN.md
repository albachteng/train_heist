# Rendering Module Design

## Current Architecture (v1 - SFML Integration Implementation)

### Goals
- **Library-agnostic engine**: No SFML/OpenGL includes in core ECS/game logic
- **Modularity**: Swappable rendering backends through interfaces
- **Testability**: Mock implementations for unit/integration tests
- **Composable design**: Clear separation between rendering, resource management, and ECS components

### Design Decisions (Finalized 2025-01-17)

**1. IRenderer Location**: ✅ **DECIDED**
- **Decision**: Move `IRenderer` from `engine/ecs/systems/include/` to `engine/rendering/include/`
- **Rationale**: Better module ownership - rendering interface belongs in rendering module
- **Impact**: Update include statements in RenderSystem and tests

**2. Resource Management**: ✅ **DECIDED** 
- **Decision**: Implement ResourceManager from the beginning
- **Approach**: Simple initial implementation with extensibility planning
- **Interface**: ResourceManager handles texture loading, IRenderer consumes texture handles
- **Benefits**: Clean separation of concerns, future optimization path

**3. Window Management**: ✅ **DECIDED**
- **Decision**: Separate WindowManager from SFMLRenderer
- **Rationale**: Allows migration away from SFML without touching rendering logic
- **Interface**: WindowManager owns sf::RenderWindow, SFMLRenderer receives render target
- **Benefits**: Better abstraction, cleaner testing, future backend flexibility

**4. Testing Strategy**: ✅ **DECIDED**
- **Decision**: Keep unit-based testing for now, plan integration tests for future
- **Approach**: Mock implementations for all interfaces (MockRenderer, MockResourceManager, MockWindowManager)
- **Future**: Integration tests will be added in separate phase

### Current Module Organization

**Interface Locations:**
- `engine/rendering/include/IRenderer.hpp` - Core rendering interface ✅ **MOVED**
- `engine/rendering/include/IResourceManager.hpp` - Resource management interface ✅ **NEW**
- `engine/rendering/include/IWindowManager.hpp` - Window management interface ✅ **NEW**  
- `engine/ecs/components/` - Rendering components (Sprite, Renderable, Position)

**Current IRenderer Interface (MVP):**
```cpp
class IRenderer {
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void clear() = 0;
    virtual void renderSprite(float x, float y, float z, float width, float height, int textureId) = 0;
    virtual void renderRect(float x, float y, float width, float height, float r, float g, float b, float a) = 0;
    virtual void getScreenSize(int& width, int& height) const = 0;
};
```

### Component Architecture
- **Rendering components** live in `engine/ecs/components/` (reusable across systems)
- **Rendering systems** live in `engine/rendering/src/` (specific to rendering module)
- **Resource management** will be separate ResourceManager (future implementation)

### Architecture Overview

**Component Dependencies:**
```
Core ECS ← Rendering Components (Sprite, Renderable, Position)
    ↑
RenderSystem ← IRenderer Interface ← SFMLRenderer
    ↑              ↑                      ↑
    ↑              ↑                 IResourceManager ← ResourceManager
    ↑              ↑                      ↑
    ↑              ↑                 IWindowManager ← WindowManager
    ↑              ↑                                      ↑
    ↑              MockRenderer                      sf::RenderWindow
    ↑
SystemManager
```

**Module Boundaries:**
- **ECS Core**: No SFML dependencies, only component interfaces
- **Rendering System**: Depends only on abstract interfaces
- **SFML Implementation**: Contains all SFML imports and concrete implementations
- **Testing Layer**: Mock implementations for all interfaces

## Future Evolution (v2+ - Performance & Features)

### Planned Enhancements
1. **Opaque Resource Handles**
   ```cpp
   using TextureHandle = uint32_t;
   virtual TextureHandle loadTexture(const std::string& path) = 0;
   virtual void renderSprite(..., TextureHandle texture) = 0;
   ```

2. **Batch Submission API**
   ```cpp
   struct SpriteBatch {
       TextureHandle texture;
       std::vector<SpriteInstance> instances;
   };
   virtual void renderBatch(const SpriteBatch& batch) = 0;
   ```

3. **Camera/Transform Pipeline**
   ```cpp
   virtual void setViewMatrix(const Matrix4& view) = 0;
   virtual void setProjectionMatrix(const Matrix4& projection) = 0;
   ```

4. **Resource Management Integration**
   - Separate ResourceManager for textures, fonts, shaders
   - IRenderer consumes resource handles, doesn't manage loading
   - Clear separation of concerns

### Migration Strategy
- Current interface provides solid MVP foundation
- Future enhancements will be additive (no breaking changes to existing methods)
- Component design already supports future features (Position.z for depth, etc.)

## Implementation Priorities

### Phase 1 (Current): SFML Integration
- [x] IRenderer interface defined and moved to rendering module
- [x] MockRenderer for testing with call order verification
- [x] Basic rendering components (Sprite, Renderable) with ZII compliance
- [x] RenderSystem with dependency injection and comprehensive tests
- [ ] **RED PHASE**: IResourceManager interface and MockResourceManager tests
- [ ] **RED PHASE**: IWindowManager interface and MockWindowManager tests  
- [ ] **RED PHASE**: SFMLRenderer tests (unit-based, no actual SFML window)
- [ ] **GREEN PHASE**: ResourceManager implementation
- [ ] **GREEN PHASE**: WindowManager implementation  
- [ ] **GREEN PHASE**: SFMLRenderer implementation

### Phase 2 (Next): Integration & Testing
- [ ] Integration tests with actual SFML window
- [ ] Main application demonstrating full stack
- [ ] Performance benchmarking and optimization

### Phase 3 (Future): Performance
- [ ] Batch rendering API
- [ ] Camera/transform pipeline
- [ ] Advanced rendering features (layers, effects)

## Design Principles

1. **Start simple, evolve thoughtfully** - Current immediate API → future batching
2. **Modular composition** - Separate concerns (rendering, resources, components)
3. **Interface stability** - Add features without breaking existing code
4. **Component reusability** - ECS components usable beyond just rendering
5. **Clear boundaries** - No third-party dependencies leak into core ECS
