# Game Design Document – “Train Heist” (working title)

## Game Overview

- Genre: Turn-based tactical strategy / grid-based combat
- Perspective: Isometric 2.5D
- Core Loop: Control multiple units per level, navigate a grid, interact with obstacles/enemies, stop a train, and steal its cargo
- Progression: Unlock new units, abilities, and mechanics as levels progress
- Visual Identity: Stylized, mutable, reflecting time-travel — art style, units, environments, and UI change significantly across eras

## Core Gameplay Mechanics

- Grid-Based Movement: Turn-based; units move on tiles; movement rules vary by unit
- Combat & Abilities: Units have distinct abilities consuming action points or energy per turn
- Train Mechanics: Predetermined path; levels are won by stopping the train and boarding it; obstacles/defenses vary by level and timeline
- Level Variations: Core grid layout reused; size, obstacles, enemy positions, and decoration vary
- Progression: Unlock new units, abilities, and mechanics; possible meta-progression between levels

## Units

- Attributes: Health, movement range, attack range, speed, special abilities
- Time-Period Specific Design: Wild West units → Norse warriors → Space pirates (example progression)
- Animations: Idle, walking, attacking, special ability, and death animations; simple sprite-based animations or tile effects

## Environment & Tiles

- Tiles: Core grid-based logic; properties include traversable, blocked, hazard, interactable; optional tile animations
- Background/Scenery: Stylized, dynamic per level, supports dramatic visual shifts between eras

## UI Design

- Components: Buttons, unit info panels, stats screens, pause/menu screens
- Requirements: Responsive; easily themeable per timeline; selection highlights and active unit feedback

## Dialogue & Narrative (Optional MVP)

- Branching dialogue trees and story elements can be added after MVP
- Engine should support text panels and branching conditions

## Level / Scenario System

- Level Definition: External data-driven format (JSON/XML/custom) defining grid layout, train path, initial units, obstacles, enemies, tileset, visual theme
- Reusability: Core engine handles grid, movement, and events; level data drives scenario diversity

## Event System

- Centralized event queue to handle:
- Movement, combat, ability usage
- Unit deaths or spawning
- Train progress and interactions with obstacles
- UI notifications
- Decouples ECS core from gameplay logic

## Time Travel / Visual Themes

- Engine supports dynamic replacement of:
- Tile sets
- Unit sprites and animations
- Backgrounds
- UI themes
- Allows the same level layout to feel visually unique across timelines

## Audio

- Music and sound effects tied to time period
- Events trigger SFX for abilities, movement, UI clicks, train noises

## Software Design Considerations

### ECS Core

- Keep pure ECS logic independent of rendering or gameplay mechanics
- Use component bitmasks and struct-of-arrays for performance

### Systems

- Systems as pure functions over ECS components: movement, combat, train, abilities
- Rendering/UI systems are platform-specific and injected

### Event System

- Template-based Event<T> for strong typing
- Handles in-game actions, notifications, and decoupling of systems

### Data-Driven Design

- Levels, units, abilities, tiles defined in external files
- Simplifies adding new time periods and mechanics without engine code changes

### Asset Management

- Abstracted resource loader handles multiple spritesets, tilesets, audio
- Supports dynamic swapping for time-travel mechanic

### UI

- Themeable for era-specific visuals
- Componentized for unit selection, stats, and menus

### Testing / TDD

- Core ECS logic, event handling, and AI/unit behavior fully testable in isolation
- Rendering/UI tests may be headless initially

### Extensibility

- Add new unit types, abilities, or visual themes with minimal engine changes
- Supports future dialogue systems, branching paths, or additional mechanics

### Key Design Principles

Data-Driven: Engine code handles logic; data drives content
Decoupled Systems: ECS, rendering, UI, and gameplay logic separated
Testable: Core systems testable without UI or graphics
Flexible: Time-period changes and level variations handled via configuration/assets
Lightweight: Minimal classes/OO; POD components and systems as functions
