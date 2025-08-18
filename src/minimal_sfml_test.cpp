#include <SFML/Graphics.hpp>
#include <iostream>

/**
 * Minimal SFML test to isolate WSLg compatibility issue
 */

int main() {
    std::cout << "Minimal SFML Test\n";
    std::cout << "==================\n";
    
    try {
        // Create the simplest possible SFML window
        sf::RenderWindow window(sf::VideoMode(sf::Vector2u(400, 300)), "SFML Test");
        
        std::cout << "Window created successfully!\n";
        std::cout << "Window size: " << window.getSize().x << "x" << window.getSize().y << "\n";
        
        int frameCount = 0;
        
        // Simple event loop
        while (window.isOpen() && frameCount < 60) { // Run for 1 second max
            auto event = window.pollEvent();
            if (event.has_value()) {
                if (event->getIf<sf::Event::Closed>()) {
                    window.close();
                    break;
                }
            }
            
            // Simple rendering
            window.clear(sf::Color::Blue);
            window.display();
            
            frameCount++;
            if (frameCount % 10 == 0) {
                std::cout << "Frame " << frameCount << " rendered successfully\n";
            }
        }
        
        std::cout << "Test completed successfully! (" << frameCount << " frames)\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "SFML Test failed: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "SFML Test failed with unknown error\n";
        return 1;
    }
}