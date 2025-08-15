#pragma once

namespace ECS {

/**
 * IInputManager - Interface for input handling systems
 * 
 * Provides platform-independent input abstraction for keyboard, mouse, and other devices.
 * Implementations should handle platform-specific input APIs (SFML, SDL, etc.)
 */
class IInputManager {
public:
    virtual ~IInputManager() = default;
    
    // Keyboard input
    virtual bool isKeyPressed(int keyCode) const = 0;
    virtual bool wasKeyJustPressed(int keyCode) const = 0;
    virtual bool wasKeyJustReleased(int keyCode) const = 0;
    
    // Mouse input
    virtual bool isMouseButtonPressed(int button) const = 0;
    virtual bool wasMouseButtonJustPressed(int button) const = 0;
    virtual bool wasMouseButtonJustReleased(int button) const = 0;
    virtual void getMousePosition(int& x, int& y) const = 0;
    
    // Input state management
    virtual void update() = 0;  // Called each frame to update input state
    virtual void reset() = 0;   // Reset input state (useful for tests)
};

// Common key codes (platform implementations should map to these)
namespace Keys {
    constexpr int A = 0;
    constexpr int B = 1;
    constexpr int C = 2;
    constexpr int D = 3;
    constexpr int E = 4;
    constexpr int F = 5;
    constexpr int G = 6;
    constexpr int H = 7;
    constexpr int I = 8;
    constexpr int J = 9;
    constexpr int K = 10;
    constexpr int L = 11;
    constexpr int M = 12;
    constexpr int N = 13;
    constexpr int O = 14;
    constexpr int P = 15;
    constexpr int Q = 16;
    constexpr int R = 17;
    constexpr int S = 18;
    constexpr int T = 19;
    constexpr int U = 20;
    constexpr int V = 21;
    constexpr int W = 22;
    constexpr int X = 23;
    constexpr int Y = 24;
    constexpr int Z = 25;
    
    // Arrow keys
    constexpr int Up = 26;
    constexpr int Down = 27;
    constexpr int Left = 28;
    constexpr int Right = 29;
    
    // Common keys
    constexpr int Space = 30;
    constexpr int Enter = 31;
    constexpr int Escape = 32;
    constexpr int Shift = 33;
    constexpr int Control = 34;
    constexpr int Alt = 35;
}

// Common mouse buttons
namespace MouseButtons {
    constexpr int Left = 0;
    constexpr int Right = 1;
    constexpr int Middle = 2;
}

} // namespace ECS