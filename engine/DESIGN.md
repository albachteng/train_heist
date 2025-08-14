# Design Document - Isometric ECS Game Engine

## 1. Vision
A lightweight, ECS-driven 2D/2.5D engine for grid-based isometric games with simple physics and sprite rendering. Written in minimalist C++ with POD components and minimal OO overhead.  
**Performance and safety considerations:**
- Struct-of-Arrays (SoA) component storage for cache-friendly iteration.
- Component presence tracked via bitmasks for branch-free multi-component queries.
- Zero-initialization (ZII) for predictable component states.
- Memory arenas for fast, contiguous allocation of component arrays.

## 2. Architecture Overview
- **ECS Core:** Manages entities and components, tracks component presence via bitmasks, and handles event dispatch using typed `Event<T>` payloads.  
- **Systems:** Stateless functions operating over component arrays, using bitmask filtering for multi-component queries.  
- **Separation of Concerns:** Rendering, physics, input, and resources live in independent modules for modularity and testability.  
- **Data-Driven:** Game content (maps, entity archetypes, etc.) is loaded from external data files.

## 3. Key Design Rules
1. No system depends on game-specific logic.  
2. Each system is testable in isolation.  
3. All components are POD structs with no behavior.  
4. TDD: Tests and header stubs are written before implementation.  
5. Documentation required for all public APIs.  
6. Component presence tracked via bitmasks; swap-remove keeps arrays dense.  
7. Components are zero-initialized (ZII) and optionally allocated in memory arenas for performance.  

## 4. Systems & Responsibilities
- **ECS Core:** Entity/component management, bitmask tracking, and event dispatch.  
- **Event System:** Typed `Event<T>` payloads for decoupled, type-safe communication between systems (e.g., input → movement → sound).  
- **Rendering:** Sprite/tile drawing, camera transforms; queries Position + Sprite components via bitmask filtering.  
- **Physics:** Movement, collisions, grid alignment; updates Position components based on Velocity or actions.  
- **Input:** Maps user actions to events.  
- **Resources:** Loading/storing textures, maps, animations.  
- **Game-Specific:** Scenes, entity prefabs, scripts.

## 5. Example Flow
1. Input system detects keypress (e.g., Arrow Right).  
2. Input system pushes a `MoveEvent<EntityID>` onto the EventQueue.  
3. MovementSystem processes `MoveEvent`, updates Position component for the entity.  
4. MovementSystem may push additional events (e.g., `SoundEvent`) that downstream systems subscribe to.  
5. Rendering system queries Position + Sprite components (via bitmask filtering) and draws entities at updated positions.
