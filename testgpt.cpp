```cpp
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

const int GRID_SIZE = 6;
const int CELL_SIZE = 80;
const int WINDOW_SIZE = GRID_SIZE * CELL_SIZE;

enum CellState {
    EMPTY,
    SELECTED,
    CORRECT,
    WRONG
};

struct Cell {
    CellState state = EMPTY;
};

class PathFinder {
private:
    Cell grid[GRID_SIZE][GRID_SIZE];
    std::vector<std::pair<int, int>> userPath;
    std::vector<std::pair<int, int>> correctPath;

public:
    PathFinder() {
        // Define correct path (row, col)
        correctPath = {
            {0,0}, {0,1}, {1,1}, {2,1}, {2,2}, {3,2}
        };
    }

    void reset() {
        userPath.clear();
        for (int i = 0; i < GRID_SIZE; i++)
            for (int j = 0; j < GRID_SIZE; j++)
                grid[i][j].state = EMPTY;
    }

    bool isAdjacent(int r1, int c1, int r2, int c2) {
        return (abs(r1 - r2) + abs(c1 - c2)) == 1;
    }

    void handleClick(int row, int col) {
        if (!userPath.empty()) {
            auto last = userPath.back();
            if (!isAdjacent(last.first, last.second, row, col)) {
                std::cout << "Invalid move!\n";
                reset();
                return;
            }
        }

        userPath.push_back({row, col});

        int index = userPath.size() - 1;

        if (index < correctPath.size() &&
            correctPath[index] == userPath[index]) {
            grid[row][col].state = CORRECT;
        } else {
            grid[row][col].state = WRONG;
            std::cout << "Wrong path! Resetting...\n";
            reset();
        }
    }

    void draw(sf::RenderWindow& window) {
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
                cell.setPosition(j * CELL_SIZE, i * CELL_SIZE);

                switch (grid[i][j].state) {
                    case EMPTY: cell.setFillColor(sf::Color::White); break;
                    case SELECTED: cell.setFillColor(sf::Color::Blue); break;
                    case CORRECT: cell.setFillColor(sf::Color::Green); break;
                    case WRONG: cell.setFillColor(sf::Color::Red); break;
                }

                cell.setOutlineThickness(1);
                cell.setOutlineColor(sf::Color::Black);

                window.draw(cell);
            }
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "6x6 Path Finder");

    PathFinder game;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                if (event.key.code == sf::Keyboard::R)
                    game.reset();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;

                int col = x / CELL_SIZE;
                int row = y / CELL_SIZE;

                if (row < GRID_SIZE && col < GRID_SIZE) {
                    game.handleClick(row, col);
                }
            }
        }

        window.clear(sf::Color::Black);
        game.draw(window);
        window.display();
    }

    return 0;
}