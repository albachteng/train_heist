#pragma once

#include <string>

namespace ECS {

/**
 * Window event types for input handling
 */
enum class WindowEventType {
    None,
    Closed,
    Resized,
    KeyPressed,
    KeyReleased,
    MousePressed,
    MouseReleased,
    MouseMoved
};

/**
 * Window event data structure
 * Simple event representation for window management
 */
struct WindowEvent {
    WindowEventType type = WindowEventType::None;
    
    // Event-specific data (union could be used for optimization later)
    int keyCode = 0;           // For keyboard events
    int mouseButton = 0;       // For mouse button events  
    int mouseX = 0;            // For mouse events
    int mouseY = 0;            // For mouse events
    int width = 0;             // For resize events
    int height = 0;            // For resize events
};

/**
 * IWindowManager - Abstract interface for window management
 * 
 * Handles window creation, event polling, and basic window operations.
 * Designed to abstract away SFML window details for clean testing and future backend flexibility.
 * 
 * Key Features:
 * - Window creation with configurable size and title
 * - Event polling and processing
 * - Window state queries (open/closed, dimensions)
 * - Display/presentation control
 * - Clean separation from rendering logic
 * 
 * Design Philosophy:
 * - WindowManager owns the actual window (sf::RenderWindow)
 * - SFMLRenderer receives render target reference from WindowManager
 * - Events can be processed independently of rendering
 * - Easy to mock for unit testing
 */
class IWindowManager {
public:
    virtual ~IWindowManager() = default;
    
    /**
     * Create and open a window with specified parameters
     * @param width Window width in pixels
     * @param height Window height in pixels
     * @param title Window title string
     * @return true if window was successfully created
     */
    virtual bool createWindow(int width, int height, const std::string& title) = 0;
    
    /**
     * Close the window and cleanup resources
     */
    virtual void closeWindow() = 0;
    
    /**
     * Check if the window is currently open
     * @return true if window is open and valid
     */
    virtual bool isWindowOpen() const = 0;
    
    /**
     * Poll for window events
     * @param event Output parameter for the next event
     * @return true if an event was retrieved, false if no events pending
     */
    virtual bool pollEvent(WindowEvent& event) = 0;
    
    /**
     * Display/present the current frame to the screen
     * Should be called after all rendering is complete
     */
    virtual void display() = 0;
    
    /**
     * Get current window dimensions
     * @param width Output parameter for window width
     * @param height Output parameter for window height
     */
    virtual void getWindowSize(int& width, int& height) const = 0;
    
    /**
     * Set window title
     * @param title New window title
     */
    virtual void setWindowTitle(const std::string& title) = 0;
    
    /**
     * Get the native render target for rendering operations
     * Returns a void* that concrete implementations can cast to their specific type
     * (e.g., sf::RenderTarget* for SFML implementation)
     * @return Pointer to native render target, or nullptr if no window
     */
    virtual void* getNativeRenderTarget() = 0;
    
    /**
     * Get count of events processed since last reset
     * Useful for testing and debugging
     * @return Number of events processed
     */
    virtual size_t getEventCount() const = 0;
    
    /**
     * Clear event count statistics
     */
    virtual void resetEventCount() = 0;
};

} // namespace ECS