#include <SFML/Graphics.hpp>
int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) window.close();
            }
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                int x = mousePressed->position.x;
                int y = mousePressed->position.y;
                if (mousePressed->button == sf::Mouse::Button::Left) window.close();
            }
        }
    }
    return 0;
}
