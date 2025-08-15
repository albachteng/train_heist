#pragma once

namespace ECS {

/**
 * IInputManager - Abstract input interface for dependency injection
 * 
 * Allows systems to handle input without depending on specific input implementations.
 * Can be swapped for different backends (SFML, SDL, mock input for tests).
 */
class IInputManager {
public:
    virtual ~IInputManager() = default;
    
    /**
     * Check if a key is currently pressed
     * @param keyCode Platform-agnostic key code
     * @return true if key is currently pressed
     */
    virtual bool isKeyPressed(int keyCode) const = 0;
    
    /**
     * Check if a key was just pressed this frame
     * @param keyCode Platform-agnostic key code
     * @return true if key was pressed this frame
     */
    virtual bool wasKeyPressed(int keyCode) const = 0;
    
    /**
     * Check if a key was just released this frame
     * @param keyCode Platform-agnostic key code
     * @return true if key was released this frame
     */
    virtual bool wasKeyReleased(int keyCode) const = 0;
    
    /**
     * Get mouse position in screen coordinates
     * @param x Output parameter for mouse X coordinate
     * @param y Output parameter for mouse Y coordinate
     */
    virtual void getMousePosition(int& x, int& y) const = 0;
    
    /**
     * Check if a mouse button is currently pressed
     * @param button Mouse button (0=left, 1=right, 2=middle)
     * @return true if button is currently pressed
     */
    virtual bool isMouseButtonPressed(int button) const = 0;
    
    /**
     * Check if a mouse button was just pressed this frame
     * @param button Mouse button (0=left, 1=right, 2=middle)
     * @return true if button was pressed this frame
     */
    virtual bool wasMouseButtonPressed(int button) const = 0;
    
    /**
     * Update input state (call once per frame before system updates)
     */
    virtual void update() = 0;
};

/**
 * Common key codes for cross-platform input
 */
namespace KeyCode {
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
    
    constexpr int Space = 32;
    constexpr int Enter = 13;
    constexpr int Escape = 27;
    constexpr int Tab = 9;
    constexpr int Backspace = 8;
    
    constexpr int Left = 37;
    constexpr int Up = 38;
    constexpr int Right = 39;
    constexpr int Down = 40;
}

} // namespace ECS