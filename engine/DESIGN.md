# Design Document - Isometric ECS Game Engine

## 1. Vision
A lightweight, ECS-driven 2D/2.5D engine for grid-based isometric games with simple physics and sprite rendering. Written in minimalist C++ with POD components and minimal OO overhead.  
**Performance and safety considerations:**
- Struct-of-Arrays (SoA) component storage for cache-friendly iteration.
- Component presence tracked via bitmasks for branch-free multi-component queries.
- Zero-initialization (ZII) for predictable component states.
- Memory arenas for fast, contiguous allocation of component arrays.

## 2. Architecture Overview
- **ECS Core:** Manages entities and components with automatic bit assignment, tracks component presence via bitmasks, and handles event dispatch using typed `Event<T>` payloads.  
- **Component Registry:** Automatic component bit assignment using `getComponentBit<T>()` eliminates manual bit management.
- **ZII Enforcement:** Static assertions ensure components are trivially copyable POD structs with default initialization.
- **Systems:** Stateless functions operating over ComponentArray storage, using bitmask filtering for branch-free multi-component queries.  
- **Event System:** Strongly typed EventQueue<T> with support for payload and no-payload events, including lambda-based processing.
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
- **ECS Core:** Entity/component management with automatic bit assignment, bitmask tracking, and ZII compliance enforcement.  
- **Component Registry:** Thread-safe automatic bit assignment using `getComponentBit<T>()` with type-based caching.
- **Event System:** Typed `EventQueue<T>` with strongly typed payloads, lambda-based processing, and EmptyPayload support for simple events.  
- **Rendering:** Sprite/tile drawing, camera transforms; queries Position + Sprite components via bitmask filtering.  
- **Physics:** Movement, collisions, grid alignment; updates Position components based on Velocity or actions.  
- **Input:** Maps user actions to events using EventQueue<T>.  
- **Resources:** Loading/storing textures, maps, animations.  
- **Game-Specific:** Scenes, entity prefabs, scripts.

## 5. Example Flow

**Setup Phase:**
```cpp
// Automatic component bit assignment
uint64_t positionBit = getComponentBit<Position>();
uint64_t velocityBit = getComponentBit<Velocity>();
uint64_t spriteBit = getComponentBit<Sprite>();

// Component storage with ZII compliance enforcement
ComponentArray<Position> positions;
ComponentArray<Velocity> velocities; 
ComponentArray<Sprite> sprites;

// Event queues for inter-system communication
EventQueue<MovementPayload> movementEvents;
EventQueue<EmptyPayload> soundEvents;
```

**Runtime Flow:**
1. **Input System** detects keypress (e.g., Arrow Right) → pushes MovementPayload{deltaX: 1.0f, deltaY: 0.0f} to movementEvents
2. **Movement System** processes events using lambda:
   ```cpp
   movementEvents.process([&](const Event<MovementPayload>& event) {
       Entity* entity = getEntity(event.source);
       Position* pos = positions.get(event.source);
       if (pos && entity->hasComponent(positionBit)) {
           pos->x += event.payload.deltaX;
           pos->y += event.payload.deltaY;
           soundEvents.emplace(event.source, {}); // Trigger footstep sound
       }
   });
   ```
3. **Sound System** processes soundEvents for audio feedback
4. **Rendering System** uses bitmask filtering to draw entities:
   ```cpp
   uint64_t renderMask = positionBit | spriteBit;
   for (Entity& entity : entities) {
       if ((entity.componentMask & renderMask) == renderMask) {
           Position* pos = positions.get(entity.id);
           Sprite* spr = sprites.get(entity.id);
           draw(pos->x, pos->y, spr->texture);
       }
   }
   ```
