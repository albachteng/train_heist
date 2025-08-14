## IRenderer
- IRenderer allows for dependency injection in the rendering system
- Keeps ECS agnostic of external libraries
```cpp
struct IRenderer {
    virtual void drawSprite(int spriteIndex, int x, int y) = 0;
    virtual ~IRenderer() = default;
};
```

Example IRenderer child class: 
```cpp
struct SFMLRenderer : IRenderer {
    sf::RenderWindow& window;
    SFMLRenderer(sf::RenderWindow& win) : window(win) {}
    void drawSprite(int spriteIndex, int x, int y) override {
        // Map spriteIndex to sf::Sprite and draw
    }
};
```

Example RenderSystem accepting renderer:
```cpp
void RenderSystem(const ComponentArray<Position>& positions,
                  const ComponentArray<Sprite>& sprites,
                  const std::vector<Entity>& entities,
                  IRenderer& renderer)
{
    const uint64_t mask = (1ULL << 0) | (1ULL << 2);
    for (auto& e : entities) {
        if ((e.componentMask & mask) == mask) {
            auto pos = positions.get(e.id);
            auto spr = sprites.get(e.id);
            if (pos && spr) {
                renderer.drawSprite(spr->spriteIndex, pos->x, pos->y);
            }
        }
    }
}
```

