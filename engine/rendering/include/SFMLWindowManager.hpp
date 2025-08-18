#pragma once

#include "IWindowManager.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

namespace ECS {

/**
 * SFMLWindowManager - Real SFML implementation of IWindowManager
 * 
 * Creates and manages actual SFML windows with event handling.
 * Provides render target access to SFMLRenderer for real graphics output.
 * 
 * Features:
 * - Real sf::RenderWindow creation and management
 * - SFML event conversion to WindowEvent format
 * - Window lifecycle management (create, close, display)
 * - Native render target access for SFMLRenderer
 */
class SFMLWindowManager : public IWindowManager {
public:
    SFMLWindowManager();
    ~SFMLWindowManager() override;

    // IWindowManager interface implementation
    bool createWindow(int width, int height, const std::string& title) override;
    void closeWindow() override;
    bool isWindowOpen() const override;
    bool pollEvent(WindowEvent& event) override;
    void display() override;
    void getWindowSize(int& width, int& height) const override;
    void setWindowTitle(const std::string& title) override;
    void* getNativeRenderTarget() override;
    size_t getEventCount() const override;
    void resetEventCount() override;

private:
    std::unique_ptr<sf::RenderWindow> window;
    size_t eventCount;
    
    /**
     * Convert SFML event to WindowEvent format
     * @param sfmlEvent SFML event from window polling
     * @param windowEvent Output WindowEvent structure
     * @return true if event was successfully converted
     */
    bool convertSFMLEvent(const sf::Event& sfmlEvent, WindowEvent& windowEvent);
};

} // namespace ECS