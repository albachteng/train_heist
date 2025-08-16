## IRenderer Interface

The IRenderer provides dependency injection for rendering implementations while keeping the core ECS agnostic of graphics libraries.

### Current Interface (MVP)
```cpp
class IRenderer {
public:
    virtual ~IRenderer() = default;
    
    // Frame lifecycle
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void clear() = 0;
    
    // Drawing operations
    virtual void renderSprite(float x, float y, float z, float width, float height, int textureId) = 0;
    virtual void renderRect(float x, float y, float width, float height, 
                           float red, float green, float blue, float alpha = 1.0f) = 0;
    
    // Utility
    virtual void getScreenSize(int& width, int& height) const = 0;
};
```

### Usage Pattern
```cpp
// Rendering system using dependency injection
class RenderSystem : public ISystem {
private:
    IRenderer* renderer;

public:
    explicit RenderSystem(IRenderer* renderer) : renderer(renderer) {}
    
    void update(float deltaTime, EntityManager& entityManager) override {
        renderer->beginFrame();
        renderer->clear();
        
        // Render entities with Position + Sprite components
        // (Component access pattern depends on ECS integration decisions)
        
        renderer->endFrame();
    }
};
```

### Implementation Examples

**Mock Renderer (for testing):**
```cpp
class MockRenderer : public IRenderer {
    std::vector<SpriteCall> spriteCalls;
    std::vector<RectCall> rectCalls;
    
public:
    void renderSprite(float x, float y, float z, float w, float h, int textureId) override {
        spriteCalls.push_back({x, y, z, w, h, textureId});
    }
    // ... other methods record calls for verification
};
```

**SFML Renderer (concrete implementation):**
```cpp
class SFMLRenderer : public IRenderer {
    sf::RenderWindow& window;
    
public:
    explicit SFMLRenderer(sf::RenderWindow& window) : window(window) {}
    
    void renderSprite(float x, float y, float z, float w, float h, int textureId) override {
        // Convert textureId to sf::Texture and render
        // Handle coordinate transforms, depth sorting, etc.
    }
    // ... other methods use SFML APIs
};
```

### Module Organization

- **Interface**: `engine/rendering/include/IRenderer.hpp` (TODO: move from engine/ecs/systems/include/)
- **Components**: `engine/ecs/components/` (Sprite, Position, Renderable, etc.)
- **Systems**: `engine/rendering/src/` (RenderSystem, etc.)
- **Implementations**: `engine/rendering/src/` (SFMLRenderer, MockRenderer, etc.)

### Future Evolution

The current interface provides a solid MVP foundation. Planned enhancements include:
- Opaque resource handles (TextureHandle instead of int)
- Batch rendering API for performance
- Camera/transform pipeline integration
- Separate ResourceManager for asset loading

See `DESIGN.md` for detailed evolution roadmap.

