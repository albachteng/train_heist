# ECS Design Overview

## Vision
Lightweight, C-style Entity-Component-System (ECS) for a turn-based 2.5D isometric grid game. 
Focus on readability, testability, and modularity, with performance optimizations using memory arenas, bitsets, and zero-initialization.

## Entities
- Represented by simple `EntityID` (uint32_t) with optional generation counter.
- Each entity has a **bitmask** tracking which components are present.
- Supports efficient multi-component queries via bitwise AND.

## Components
- Plain Old Data (POD) structs.
- Stored in **Struct-of-Arrays (SoA)** layout for cache efficiency.
- Stored in **memory arenas** for fast allocation and swap-remove.
- Zero-initialized on creation (ZII).

## Systems
- Implemented as stateless functions operating on component arrays.
- Systems query component presence using entity bitmasks.
- Designed to be unit-testable and isolated.

## Events
- `Event<T>` system with **strongly typed payloads**.
- Event queues are decoupled from systems.
- Supports type-safe subscriptions for sound, combat, or game effects.

## Iteration / Queries
- Multi-component queries performed via bitmask filtering (branch-free where possible).
- Optional helper templates provide filtered views over component arrays.

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
