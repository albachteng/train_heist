# Rendering Module Design

## Current Architecture (v1 - MVP Implementation)

### Goals
- **Library-agnostic engine**: No SFML/OpenGL includes in core ECS/game logic
- **Modularity**: Swappable rendering backends through interfaces
- **Testability**: Mock implementations for unit/integration tests
- **Composable design**: Clear separation between rendering, resource management, and ECS components

### Current Module Organization

**Interface Location:**
- `engine/rendering/include/IRenderer.hpp` - Core rendering interface (TODO: move from engine/ecs/systems/include/)
- `engine/ecs/components/` - Rendering components (Sprite, Position, etc.)

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

### Dependencies & Boundaries
```
Core ECS ← Rendering Components
    ↑
Rendering Systems ← IRenderer Interface ← Concrete Renderers (SFML, OpenGL, Mock)
    ↑
ResourceManager Interface ← Concrete Resource Managers
```

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

### Phase 1 (Current): Basic Rendering
- [x] IRenderer interface defined
- [ ] MockRenderer for testing
- [ ] Basic rendering components (Sprite, Renderable)
- [ ] SFMLRenderer implementation
- [ ] Simple RenderSystem using immediate API

### Phase 2 (Future): Resource Management
- [ ] ResourceManager interface
- [ ] Texture loading and management
- [ ] Resource handle integration with IRenderer

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
