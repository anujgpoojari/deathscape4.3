#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::Font font;
    font.openFromFile("C:/Windows/Fonts/arial.ttf");
    sf::Text text(font, "Test", 24);
    return 0;
}
