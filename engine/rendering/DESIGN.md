# Rendering & Input Design

## Goals (Why this exists)

- Library-agnostic engine: no #include <SFML/...> (or SDL/GL) in core ECS/game logic.
- Modularity: rendering and input are swappable “services.”
- Predictable surfaces for agentic AI: narrow interfaces, clear contracts, black-box backends.
- Testability: headless mocks for unit/integration tests.
- Performance: batch-friendly draw API, no per-frame heap churn, minimize virtual calls in tight loops.

## Boundaries & Ownership Layers

- Core Engine/Game (ECS, systems, gameplay): depends only on interfaces.
- Adapter(s): SFMLRenderer, SFMLInputHandler, GLRenderer, etc. Implement interfaces, own backend resources.
- Third-party: SFML, OpenGL/GLAD, etc. Only visible inside adapters.
- Dependencies (one-way only)
- Core → Interfaces → Adapters → Third-party
- No adapter → core coupling. No third-party includes outside adapters.

## Modules & Interfaces

### Rendering

Interface location

engine/rendering/include/IWindow.hpp
engine/rendering/include/IRenderer.hpp
engine/rendering/include/IRenderBackend.hpp (optional lower-level split)
engine/rendering/include/Types.hpp (opaque handles, POD structs)

### Responsibilities

- Window creation & lifecycle (delegated to an IWindow).
- Frame orchestration: beginFrame(), endFrame(), vsync control.
- Camera transforms (view/projection).
- Drawing primitives: sprites, text, lines/quads, instanced sprites.
- Resource handle management: textures, fonts, shader programs (opaque to callers).

### Key design choices

- Opaque handles (e.g., TextureHandle, FontHandle, ShaderHandle) returned by the renderer. Callers never see sf::Texture (or GL ids).
- POD draw descriptors (SoA-friendly): SpriteDraw{ TextureHandle, Vec2 pos, Vec2 scale, float rotation, Color, UVs, Layer, Z }.
- Batch submission: systems collect draw calls into contiguous buffers; renderer consumes them in one go to reduce virtual overhead and maximize cache locality.
- No per-frame allocations: use arenas/ring buffers provided by the caller or inside the backend; zero-init (ZII) defaults.
