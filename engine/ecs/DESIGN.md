# ECS Design Overview

## Vision
Lightweight, C-style Entity-Component-System (ECS) for a turn-based 2.5D isometric grid game. 
Focus on readability, testability, and modularity, with performance optimizations using memory arenas, bitsets, and zero-initialization.

## Directory Structure

ecs/
├─ include/          # Public ECS headers (for outside modules)
│   ├─ Entity.h
│   ├─ ComponentArray.hpp
│   ├─ Event.hpp
│   └─ ComponentRegistry.hpp
├─ src/              # Core ECS implementation
│   ├─ Entity.cpp
│   └─ ComponentRegistry.cpp
├─ components/       # POD components (Position, Velocity, Sprite, etc.) - Future
│   ├─ Position.h
│   ├─ Velocity.h
│   └─ Sprite.h
├─ systems/          # Pure ECS systems - Future
│   ├─ MovementSystem.h/.cpp
│   └─ RenderSystem.h/.cpp
├─ tests/            # ECS unit tests
│   ├─ ComponentArrayTests.cpp
│   ├─ EntityTests.cpp
│   └─ EventTests.cpp

## Entities
- Represented by simple `EntityID` (uint32_t) with generation counter for reuse safety.
- Each entity has a **bitmask** tracking which components are present.
- Supports efficient multi-component queries via bitwise AND.
- **EntityHandle** provides safe external references without exposing internal ECS state.
- **Mark-dead-and-reuse lifecycle**: Entities are marked dead but remain in storage for efficient reuse.

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

## Entity Management

The EntityManager implements a **mark-dead-and-reuse** approach that balances performance with memory efficiency:

### Core Architecture
- **Indexed storage**: All entities stored in `std::vector<Entity>` indexed by entity ID
- **Alive tracking**: `std::vector<bool> alive` tracks which entity slots are active
- **Generation counters**: Each entity ID has a generation that increments on reuse
- **Free ID queue**: `std::queue<EntityID> freeIds` maintains pool of reusable IDs

### Lifecycle Operations
```cpp
// Entity creation - reuses dead slots or creates new ones
Entity entity = entityManager.createEntity();

// Entity destruction - marks dead but keeps in storage
entityManager.destroyEntity(entity);

// Validation - checks generation matches current
bool valid = entityManager.isValid(entity);

// Lookup - returns current entity or nullptr if dead
Entity* current = entityManager.getEntityByID(entityId);
```

### System Iteration Pattern
```cpp
// Systems iterate over all entities (including dead ones)
for (const Entity& entity : entityManager.getAllEntitiesForIteration()) {
    if (!entityManager.isValid(entity)) {
        continue; // Skip dead entities
    }
    
    // Process only living entities with required components
    if ((entity.componentMask & requiredMask) == requiredMask) {
        // Update entity...
    }
}
```

### Performance Characteristics
- **Entity creation**: O(1) with dead slot reuse, O(1) amortized for new slots
- **Entity destruction**: O(1) mark as dead, add to free queue
- **Entity validation**: O(1) array lookup with generation check
- **Memory growth**: Grows with peak entity count, never shrinks
- **Cache performance**: Entities stored contiguously for system iteration

## Components
- Plain Old Data (POD) structs.
- Stored in **Struct-of-Arrays (SoA)** layout for cache efficiency.
- **FUTURE**: Stored in **memory arenas** for fast allocation and swap-remove.
- Zero-initialized on creation (ZII).
- **ZII Compliance Enforced**: ComponentArray uses static_assert to enforce component requirements.

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
class ComponentArray {
    // ZII (Zero-is-Initialization) Compliance Enforcement
    static_assert(std::is_trivially_copyable_v<Component>, 
                  "Component types must be trivially copyable (POD structs only)");
    static_assert(std::is_default_constructible_v<Component>, 
                  "Component types must be default constructible");
    
private:
    std::vector<Component> components;
    std::vector<EntityID> entityIDs;
    std::unordered_map<EntityID, size_t> entityIndex;

public:
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

### ZII Compliance Requirements
Components must follow Zero-is-Initialization principles:
- All members must have default initializers (e.g., `float x = 0.0f`)
- No uninitialized pointers or complex objects
- Must be trivially copyable (POD structs only)
- No constructors, destructors, or virtual functions

**Good ZII Component Example:**
```cpp
struct Position {
    float x = 0.0f;  // ✅ Zero-initialized
    float y = 0.0f;  // ✅ Zero-initialized
    float z = 0.0f;  // ✅ Zero-initialized
};
```

**Bad Component Example:**
```cpp
struct BadComponent {
    float* ptr;      // ❌ Uninitialized pointer
    int value;       // ❌ No default value
    std::string name; // ❌ Not trivially copyable
};
```

## Component Registry

The ComponentRegistry provides **automatic component bit assignment**, eliminating manual bit management and reducing errors.

**Features:**
- Thread-safe automatic bit assignment using atomic counters
- Type-based bit caching for performance
- Compile-time component validation (ZII compliance)
- Supports up to 64 component types (uint64_t bitmask)

**Usage:**
```cpp
#include "ComponentRegistry.hpp"

// Automatic bit assignment
uint64_t positionBit = getComponentBit<Position>();
uint64_t velocityBit = getComponentBit<Velocity>();

// Use with ComponentArray
ComponentArray<Position> positions;
positions.add(entityId, position, positionBit, entity);

// Same type always returns same bit
uint64_t sameBit = getComponentBit<Position>(); // Returns cached bit
assert(positionBit == sameBit);
```

**Implementation:**
- Each component type gets a unique bit position on first call
- Subsequent calls return cached bit for performance
- Thread-safe using std::atomic for bit counter
- Static template specialization for per-type storage

## Systems
- Implemented as stateless functions operating on component arrays.
- Systems query component presence using entity bitmasks.
- Designed to be unit-testable and isolated.

## Events

The Event system provides **strongly typed inter-system communication** using `Event<T>` with typed payloads.

**Features:**
- Type-safe event payloads prevent runtime errors
- Decoupled event queues for each event type
- Support for events with and without payloads
- Efficient queue operations (push, pop, peek, process)
- Built-in event aliases for common patterns

**Core Types:**
```cpp
template <typename T>
struct Event {
    EntityID source = INVALID_ENTITY;
    T payload;
    
    Event() = default;
    Event(EntityID src, T data) : source(src), payload(data) {}
    explicit Event(T data) : payload(data) {}
};

template <typename T>
class EventQueue {
    std::vector<Event<T>> events;
    
public:
    void push(const Event<T>& event);
    void emplace(EntityID source, const T& payload);
    std::vector<Event<T>> popAll();
    const Event<T>* peek() const;
    void clear();
    bool empty() const;
    size_t size() const;
    
    template<typename Func>
    void process(Func&& processor);
};
```

**Usage Examples:**
```cpp
// Events with payloads
EventQueue<MovementPayload> movementEvents;
movementEvents.emplace(entity.id, {deltaX, deltaY});

// Events without payloads (using EmptyPayload)
EventQueue<EmptyPayload> collisionEvents;
collisionEvents.emplace(entity.id, {});

// Simple event aliases
using CollisionEvent = Event<EmptyPayload>;
using MovementEvent = Event<MovementPayload>;

// Processing events
movementEvents.process([&](const Event<MovementPayload>& event) {
    // Handle movement event
    updatePosition(event.source, event.payload.deltaX, event.payload.deltaY);
});
```

## Iteration / Queries
- Multi-component queries performed via bitmask filtering (branch-free where possible).
- Optional helper templates provide filtered views over component arrays.

```cpp
void MovementSystem(ComponentArray<Position>& positions,
                    const ComponentArray<Velocity>& velocities,
                    std::vector<Entity>& entities) 
{
    // Use component registry for automatic bit assignment
    const uint64_t positionBit = getComponentBit<Position>();
    const uint64_t velocityBit = getComponentBit<Velocity>();
    const uint64_t mask = positionBit | velocityBit; // Position + Velocity
    
    for (auto& e : entities) {
        // Branch-free bitmask query
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
- **Bitmasks** → branch-free component checks (14x faster than hash map lookups).
- **ZII compliance** → safe defaults, predictable behavior, and optimal memory layout.
- **Component registry** → eliminates manual bit management overhead.
- **Static assertions** → compile-time validation prevents runtime errors.

## Threading Considerations
**TODO**: EntityManager currently uses a mark-dead-and-reuse approach for entity lifecycle management, which has specific threading implications:

- **Current State**: Single-threaded design with no thread safety
- **Planned Threading Strategy**: Coarse-grained locking (single mutex protecting EntityManager operations)
- **Advantages of mark-dead-and-reuse for threading**:
  - Stable memory layout (dead entities stay in storage)
  - Entity pointers remain valid longer
  - No memory compaction during iteration
  - Read-heavy operations can use reader-writer locks
- **Implementation Priority**: Add threading support after core ECS functionality is complete

## Notes
- ECS is **agnostic to rendering** (SFML/OpenGL handled in a separate rendering system).
- Supports modular testing by isolating each system and component in its own folder.
