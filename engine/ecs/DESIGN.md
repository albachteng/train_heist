# ECS Design Overview

## Vision
Lightweight, C-style Entity-Component-System (ECS) for a turn-based 2.5D isometric grid game. 
Focus on readability, testability, and modularity, with performance optimizations using memory arenas, bitsets, and zero-initialization.

## Directory Structure

ecs/
├─ include/          # Public ECS headers (for outside modules)
│   ├─ Entity.h
│   ├─ ComponentArray.h
│   ├─ Event.h
│   └─ ECSUtils.h
├─ src/              # Core ECS implementation
│   ├─ Entity.cpp
│   ├─ ComponentArray.cpp
│   └─ Event.cpp
├─ components/       # POD components (Position, Velocity, Sprite, etc.)
│   ├─ Position.h
│   ├─ Velocity.h
│   └─ Sprite.h
├─ systems/          # Pure ECS systems
│   ├─ MovementSystem.h/.cpp
│   └─ RenderSystem.h/.cpp
├─ events/           # Typed Event structs & queues
│   ├─ Event.h
│   └─ EventQueue.h
├─ tests/            # ECS unit tests, one test file per system/component
│   ├─ ComponentArrayTests.cpp
│   └─ MovementSystemTests.cpp

## Entities
- Represented by simple `EntityID` (uint32_t) with optional generation counter.
- Each entity has a **bitmask** tracking which components are present.
- Supports efficient multi-component queries via bitwise AND.
- **EntityHandle** provides safe external references without exposing internal ECS state.

```cpp
using EntityID = uint32_t;
const EntityID INVALID_ENTITY = 0;

struct Entity {
    EntityID id;
    uint32_t generation = 0;
    uint64_t componentMask = 0; // bit i set if entity has component i
};

struct EntityHandle {
    EntityID id;
    uint32_t generation;
    // Opaque reference for external systems - prevents direct component mask access
    // Generational safety: detects stale references when entity IDs are reused
    // API encapsulation: forces validation through ECS manager entry points
};
```

## Components
- Plain Old Data (POD) structs.
- Stored in **Struct-of-Arrays (SoA)** layout for cache efficiency.
- **FUTURE**: Stored in **memory arenas** for fast allocation and swap-remove.
- Zero-initialized on creation (ZII).

**Current Implementation**: ComponentArray uses std::vector for storage to prioritize 
correctness and debugging during initial development. This provides:
- Well-understood behavior and debugging support
- Automatic memory management  
- Standard library reliability

**Planned Migration**: Memory arena implementation will replace std::vector to achieve:
- 2-3x performance improvement for component iteration
- Zero malloc calls during gameplay (single arena allocation)
- Better cache locality through contiguous memory layout
- Reduced memory fragmentation
- The public ComponentArray API is designed to support this migration without changes.

```cpp
template <typename Component>
struct ComponentArray {
    std::vector<Component> components;
    std::vector<EntityID> entityIDs;
    std::unordered_map<EntityID, size_t> entityIndex;

    void add(EntityID e, const Component& c, uint64_t componentBit, Entity& entity) {
        components.push_back(c);
        entityIDs.push_back(e);
        entityIndex[e] = components.size() - 1;
        entity.componentMask |= componentBit; // update entity bitmask
    }

    bool has(EntityID e) const {
        return entityIndex.find(e) != entityIndex.end();
    }

    Component* get(EntityID e) {
        auto it = entityIndex.find(e);
        return it != entityIndex.end() ? &components[it->second] : nullptr;
    }

    void remove(EntityID e, uint64_t componentBit, Entity& entity) {
        auto it = entityIndex.find(e);
        if (it == entityIndex.end()) return;

        size_t idx = it->second;
        EntityID lastEntity = entityIDs.back();

        components[idx] = components.back();
        entityIDs[idx] = lastEntity;
        entityIndex[lastEntity] = idx;

        components.pop_back();
        entityIDs.pop_back();
        entityIndex.erase(it);

        entity.componentMask &= ~componentBit; // clear bit in entity mask
    }
};
```

## Systems
- Implemented as stateless functions operating on component arrays.
- Systems query component presence using entity bitmasks.
- Designed to be unit-testable and isolated.

## Events
- `Event<T>` system with **strongly typed payloads**.
- Event queues are decoupled from systems.
- Supports type-safe subscriptions for sound, combat, or game effects.

```cpp
template <typename T>
struct Event {
    EntityID source;
    T payload;
};

template <typename T>
struct EventQueue {
    std::vector<Event<T>> events;

    void push(const Event<T>& e) { events.push_back(e); }

    std::vector<Event<T>> popAll() {
        auto copy = events;
        events.clear();
        return copy;
    }
};
```

## Iteration / Queries
- Multi-component queries performed via bitmask filtering (branch-free where possible).
- Optional helper templates provide filtered views over component arrays.

```cpp
void MovementSystem(ComponentArray<Position>& positions,
                    const ComponentArray<Velocity>& velocities,
                    std::vector<Entity>& entities) 
{
    const uint64_t mask = (1ULL << 0) | (1ULL << 1); // Position + Velocity
    for (auto& e : entities) {
        if ((e.componentMask & mask) == mask) {
            Position* pos = positions.get(e.id);
            Velocity* vel = velocities.get(e.id);
            if (pos && vel) {
                pos->x += vel->dx;
                pos->y += vel->dy;
            }
        }
    }
}
```

## Debugging & Safety
- Entity-component mapping can be printed via debug helpers.
- Generations prevent stale entity references.
- Swap-remove keeps arrays dense without invalidating bitmask checks.

## Performance Considerations
- **SoA + memory arenas** → cache-friendly iteration.
- **Bitsets** → branch-free component checks.
- **ZII** → safe defaults and predictable behavior.

## Notes
- ECS is **agnostic to rendering** (SFML/OpenGL handled in a separate rendering system).
- Supports modular testing by isolating each system and component in its own folder.
