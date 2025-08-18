#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

/**
 * Simple engine test - uses SFML directly like the working minimal test
 * but with our entity simulation
 */

int main() {
    std::cout << "Simple Engine Test\n";
    std::cout << "==================\n";
    
    try {
        // Create window exactly like the working minimal test
        sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600)), "Simple Engine Test");
        
        std::cout << "Window created successfully!\n";
        
        // Create some rectangles to draw (like our entities)
        sf::RectangleShape redRect(sf::Vector2f(100, 100));
        redRect.setPosition(sf::Vector2f(50, 50));
        redRect.setFillColor(sf::Color(255, 0, 0));
        
        sf::RectangleShape greenRect(sf::Vector2f(100, 100));
        greenRect.setPosition(sf::Vector2f(650, 50));
        greenRect.setFillColor(sf::Color(0, 255, 0));
        
        sf::RectangleShape blueRect(sf::Vector2f(100, 100));
        blueRect.setPosition(sf::Vector2f(50, 450));
        blueRect.setFillColor(sf::Color(0, 0, 255));
        
        sf::RectangleShape yellowRect(sf::Vector2f(100, 100));
        yellowRect.setPosition(sf::Vector2f(350, 250));
        yellowRect.setFillColor(sf::Color(255, 255, 0));
        
        sf::RectangleShape purpleRect(sf::Vector2f(200, 200));
        purpleRect.setPosition(sf::Vector2f(300, 200));
        purpleRect.setFillColor(sf::Color(255, 0, 255, 128)); // Semi-transparent
        
        std::cout << "Created 5 rectangles (simulating our entities)\n";
        std::cout << "Starting render loop...\n";
        
        int frameCount = 0;
        
        // Render loop exactly like the working minimal test
        while (window.isOpen() && frameCount < 300) { // Run for 5 seconds max
            // Event handling like the working minimal test
            auto event = window.pollEvent();
            if (event.has_value()) {
                if (event->getIf<sf::Event::Closed>()) {
                    window.close();
                    break;
                }
                if (auto keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    std::cout << "Key pressed: " << static_cast<int>(keyPressed->code) << "\n";
                }
            }
            
            // Simple animation (move yellow rectangle)
            if (frameCount % 60 == 0) { // Every second
                float angle = (frameCount / 60.0f) * 3.14159f * 2.0f / 10.0f;
                float x = 350.0f + 80.0f * std::cos(angle);
                float y = 250.0f + 80.0f * std::sin(angle);
                yellowRect.setPosition(sf::Vector2f(x, y));
                std::cout << "Frame " << frameCount << " - Yellow moved to (" << x << ", " << y << ")\n";
            }
            
            // Rendering exactly like the working minimal test
            window.clear(sf::Color::Black);
            
            // Draw all rectangles (simulating our ECS rendering)
            window.draw(redRect);
            window.draw(greenRect);
            window.draw(blueRect);
            window.draw(yellowRect);
            window.draw(purpleRect);
            
            window.display();
            
            frameCount++;
        }
        
        std::cout << "Test completed successfully! (" << frameCount << " frames)\n";
        std::cout << "This proves SFML works fine with entity-like rendering\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Simple Engine Test failed: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Simple Engine Test failed with unknown error\n";
        return 1;
    }
}