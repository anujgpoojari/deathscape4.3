#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <optional>
#include <windows.h> // Required for Serial COM APIs

// ==========================================
// SERIAL COMMUNICATION
// ==========================================
HANDLE hSerial = INVALID_HANDLE_VALUE;

void initSerial(const char* portName) {
    hSerial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(hSerial == INVALID_HANDLE_VALUE) {
        std::cout << "[Serial] Warning: Could not open port " << portName << ". Game will run in PC-only mode.\n";
        return;
    }
    
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams)) {
        dcbSerialParams.BaudRate = CBR_9600;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;
        SetCommState(hSerial, &dcbSerialParams);
    }
    
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);
    std::cout << "[Serial] Successfully hooked to " << portName << "\n";
}

void sendData(const std::string& data) {
    if (hSerial == INVALID_HANDLE_VALUE) return;
    DWORD bytesWritten;
    WriteFile(hSerial, data.c_str(), data.length(), &bytesWritten, NULL);
}

// ==========================================
// GAME PROPERTIES
// ==========================================
const unsigned int GRID_SIZE = 6;
const unsigned int CELL_SIZE = 80;
const unsigned int MARGIN = 40;

const unsigned int GRID_PIXEL_TOTAL = GRID_SIZE * CELL_SIZE;

const unsigned int WINDOW_WIDTH = GRID_PIXEL_TOTAL * 2 + MARGIN * 3;
const unsigned int WINDOW_HEIGHT = GRID_PIXEL_TOTAL + MARGIN * 2 + 80;

const unsigned int BUTTON_WIDTH = 120;
const unsigned int BUTTON_HEIGHT = 40;

const unsigned int RIGHT_GRID_OFFSET_X = MARGIN * 2 + GRID_PIXEL_TOTAL;

enum class CellState {
    Empty,
    Selected,   
    Correct,    
    Incorrect   
};

int main()
{
    // Boot up the serial
    initSerial("COM5");

    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Dual Window Memory Puzzle");
    window.setFramerateLimit(60);

    sf::Font font;
    bool hasFont = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    if (!hasFont) {
        std::cout << "Warning: Could not load Arial font. Text might not render.\n";
    }

    std::vector<sf::Vector2i> definedPath;   
    std::vector<sf::Vector2i> testingPath;   

    CellState leftGrid[GRID_SIZE][GRID_SIZE];
    CellState rightGrid[GRID_SIZE][GRID_SIZE];

    bool phase1_Drawing = true;
    bool showRightError = false; 
    sf::Vector2i currentErrorPoint(-1, -1); 
    sf::Vector2i lastResetErrorTile(-1, -1); 

    auto resetLeftGrid = [&]() {
        definedPath.clear();
        for (unsigned int y = 0; y < GRID_SIZE; ++y) {
            for (unsigned int x = 0; x < GRID_SIZE; ++x) {
                leftGrid[y][x] = CellState::Empty;
            }
        }
        phase1_Drawing = true;
    };

    auto resetRightGrid = [&]() {
        testingPath.clear();
        for (unsigned int y = 0; y < GRID_SIZE; ++y) {
            for (unsigned int x = 0; x < GRID_SIZE; ++x) {
                rightGrid[y][x] = CellState::Empty;
            }
        }
        showRightError = false;
        currentErrorPoint = sf::Vector2i(-1, -1);
        sendData("R\n"); // Command Arduino to drop state
    };

    auto fullReset = [&]() {
        resetLeftGrid();
        resetRightGrid();
        lastResetErrorTile = sf::Vector2i(-1, -1);
    };

    fullReset();

    while (window.isOpen())
    {
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                    window.close();
                if (keyPressed->code == sf::Keyboard::Key::R)
                    fullReset();
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    
                    int mouseX = mousePressed->position.x;
                    int mouseY = mousePressed->position.y;

                    // ============================================
                    // RESET BUTTON LOGIC
                    // ============================================
                    if (!phase1_Drawing && 
                        mouseX >= (int)RIGHT_GRID_OFFSET_X && mouseX <= (int)(RIGHT_GRID_OFFSET_X + BUTTON_WIDTH) &&
                        mouseY >= (int)(MARGIN + GRID_PIXEL_TOTAL + 20) && mouseY <= (int)(MARGIN + GRID_PIXEL_TOTAL + 20 + BUTTON_HEIGHT)) {
                        
                        if (showRightError) {
                            lastResetErrorTile = currentErrorPoint;
                        } else {
                            lastResetErrorTile = sf::Vector2i(-1, -1);
                        }

                        resetRightGrid();
                        continue;
                    }

                    // ============================================
                    // UNDO BUTTON LOGIC
                    // ============================================
                    if (mouseX >= (int)(RIGHT_GRID_OFFSET_X + BUTTON_WIDTH + 20) && mouseX <= (int)(RIGHT_GRID_OFFSET_X + 2*BUTTON_WIDTH + 20) &&
                        mouseY >= (int)(MARGIN + GRID_PIXEL_TOTAL + 20) && mouseY <= (int)(MARGIN + GRID_PIXEL_TOTAL + 20 + BUTTON_HEIGHT)) {
                        
                        if (phase1_Drawing) {
                            if (!definedPath.empty()) {
                                sf::Vector2i back = definedPath.back();
                                leftGrid[back.y][back.x] = CellState::Empty;
                                definedPath.pop_back();
                            }
                        } else {
                            if (showRightError) {
                                rightGrid[currentErrorPoint.y][currentErrorPoint.x] = CellState::Empty;
                                showRightError = false;
                                sendData("U " + std::to_string(currentErrorPoint.x) + " " + std::to_string(currentErrorPoint.y) + "\n");
                                currentErrorPoint = sf::Vector2i(-1, -1);
                            } else {
                                if (!testingPath.empty()) {
                                    sf::Vector2i back = testingPath.back();
                                    rightGrid[back.y][back.x] = CellState::Empty;
                                    testingPath.pop_back();
                                    sendData("U " + std::to_string(back.x) + " " + std::to_string(back.y) + "\n");
                                }
                            }
                        }
                        
                        continue;
                    }

                    // ============================================
                    // LOGIC FOR LEFT GRID (Phase 1)
                    // ============================================
                    if (phase1_Drawing && 
                        mouseX >= (int)MARGIN && mouseX < (int)(MARGIN + GRID_PIXEL_TOTAL) &&
                        mouseY >= (int)MARGIN && mouseY < (int)(MARGIN + GRID_PIXEL_TOTAL)) {
                        
                        int gridX = (mouseX - MARGIN) / CELL_SIZE;
                        int gridY = (mouseY - MARGIN) / CELL_SIZE;
                        sf::Vector2i clicked(gridX, gridY);

                        bool alreadyInPath = false;
                        for (auto const& p : definedPath) if (p == clicked) alreadyInPath = true;

                        if (!alreadyInPath) {
                            bool isAdjacent = false;
                            
                            if (definedPath.empty()) {
                                if (clicked.x == 0 && clicked.y == GRID_SIZE - 1) {
                                    isAdjacent = true;
                                } else {
                                    std::cout << "You must start at the Bottom-Left cell!\n";
                                }
                            } else {
                                sf::Vector2i last = definedPath.back();
                                int dist = std::abs(last.x - clicked.x) + std::abs(last.y - clicked.y);
                                if (dist == 1) { 
                                    isAdjacent = true;
                                }
                            }

                            if (isAdjacent) {
                                definedPath.push_back(clicked);
                                leftGrid[gridY][gridX] = CellState::Selected;

                                if (clicked.x == GRID_SIZE - 1 && clicked.y == 0) {
                                    std::cout << "Path successfully defined! Now repeat it on the right grid!\n";
                                    phase1_Drawing = false; 
                                }
                            }
                        }
                    }

                    // ============================================
                    // LOGIC FOR RIGHT GRID (Phase 2)
                    // ============================================
                    if (!phase1_Drawing && !showRightError &&
                        mouseX >= (int)RIGHT_GRID_OFFSET_X && mouseX < (int)(RIGHT_GRID_OFFSET_X + GRID_PIXEL_TOTAL) &&
                        mouseY >= (int)MARGIN && mouseY < (int)(MARGIN + GRID_PIXEL_TOTAL)) {
                        
                        int gridX = (mouseX - RIGHT_GRID_OFFSET_X) / CELL_SIZE;
                        int gridY = (mouseY - MARGIN) / CELL_SIZE;
                        sf::Vector2i clicked(gridX, gridY);

                        bool alreadyInPath = false;
                        for (auto const& p : testingPath) if (p == clicked) alreadyInPath = true;

                        if (!alreadyInPath) {
                            bool isAdjacent = false;
                            
                            if (testingPath.empty()) {
                                isAdjacent = true;
                            } else {
                                sf::Vector2i last = testingPath.back();
                                int dist = std::abs(last.x - clicked.x) + std::abs(last.y - clicked.y);
                                if (dist == 1) { 
                                    isAdjacent = true;
                                }
                            }

                            if (isAdjacent) {
                                if (testingPath.size() < definedPath.size() && clicked == definedPath[testingPath.size()]) {
                                    testingPath.push_back(clicked);
                                    rightGrid[gridY][gridX] = CellState::Correct; 
                                    
                                    sendData("C " + std::to_string(clicked.x) + " " + std::to_string(clicked.y) + "\n");

                                    if (testingPath.size() == definedPath.size()) {
                                        std::cout << "Task completed successfully! Press 'R' to play again!\n";
                                        lastResetErrorTile = sf::Vector2i(-1, -1);
                                        sendData("D\n"); // Board verified!
                                    }
                                } else {
                                    rightGrid[gridY][gridX] = CellState::Incorrect;
                                    showRightError = true;
                                    currentErrorPoint = clicked;
                                    
                                    sendData("W " + std::to_string(clicked.x) + " " + std::to_string(clicked.y) + "\n");
                                }
                            }
                        }
                    }
                }
            }
        }

        // --- Render ---
        window.clear(sf::Color(230, 230, 230));

        // Draw Left Grid
        for (unsigned int y = 0; y < GRID_SIZE; ++y) {
            for (unsigned int x = 0; x < GRID_SIZE; ++x) {
                sf::RectangleShape cell(sf::Vector2f((float)CELL_SIZE - 2.f, (float)CELL_SIZE - 2.f));
                cell.setPosition(sf::Vector2f(MARGIN + x * CELL_SIZE + 1.f, MARGIN + y * CELL_SIZE + 1.f));
                
                if (leftGrid[y][x] == CellState::Selected) {
                    cell.setFillColor(sf::Color(100, 200, 255)); 
                } else {
                    cell.setFillColor(sf::Color::White); 
                    
                    if (definedPath.empty()) {
                        if (x == 0 && y == GRID_SIZE - 1) {
                            cell.setFillColor(sf::Color(220, 255, 220)); 
                        } else if (x == GRID_SIZE - 1 && y == 0) {
                            cell.setFillColor(sf::Color(255, 220, 220)); 
                        }
                    }
                }
                
                cell.setOutlineColor(sf::Color(80, 80, 80));
                cell.setOutlineThickness(1.f);
                window.draw(cell);
            }
        }

        // Draw Right Grid (locked during phase 1)
        for (unsigned int y = 0; y < GRID_SIZE; ++y) {
            for (unsigned int x = 0; x < GRID_SIZE; ++x) {
                sf::RectangleShape cell(sf::Vector2f((float)CELL_SIZE - 2.f, (float)CELL_SIZE - 2.f));
                cell.setPosition(sf::Vector2f(RIGHT_GRID_OFFSET_X + x * CELL_SIZE + 1.f, MARGIN + y * CELL_SIZE + 1.f));
                
                if (phase1_Drawing) {
                    cell.setFillColor(sf::Color(190, 190, 190));
                } else {
                    if (rightGrid[y][x] == CellState::Correct) {
                        cell.setFillColor(sf::Color(100, 255, 100)); 
                    } else if (rightGrid[y][x] == CellState::Incorrect) {
                        cell.setFillColor(sf::Color(255, 100, 100)); 
                    } else {
                        cell.setFillColor(sf::Color::White);
                        
                        if (testingPath.empty() && !showRightError) {
                            if (x == 0 && y == GRID_SIZE - 1) {
                                cell.setFillColor(sf::Color(220, 255, 220)); 
                            }
                        }
                    }
                }
                
                cell.setOutlineColor(sf::Color(80, 80, 80));
                cell.setOutlineThickness(1.f);

                if (!phase1_Drawing && (int)x == lastResetErrorTile.x && (int)y == lastResetErrorTile.y) {
                    if (rightGrid[y][x] != CellState::Incorrect) {
                        cell.setFillColor(sf::Color(100, 150, 255));
                    }
                }

                window.draw(cell);
            }
        }

        // --- Draw UI ---
        if (!phase1_Drawing) {
            sf::RectangleShape resetBtn(sf::Vector2f((float)BUTTON_WIDTH, (float)BUTTON_HEIGHT));
            resetBtn.setPosition(sf::Vector2f(RIGHT_GRID_OFFSET_X, MARGIN + GRID_PIXEL_TOTAL + 20.f));
            
            if (showRightError) resetBtn.setFillColor(sf::Color(255, 100, 100)); 
            else resetBtn.setFillColor(sf::Color(180, 180, 180)); 
            
            resetBtn.setOutlineThickness(2.f);
            resetBtn.setOutlineColor(sf::Color(100, 100, 100));
            window.draw(resetBtn);

            if (hasFont) {
                sf::Text rText(font, "RESET", 18);
                sf::FloatRect bounds = rText.getLocalBounds();
                rText.setPosition(sf::Vector2f(RIGHT_GRID_OFFSET_X + (BUTTON_WIDTH - bounds.size.x) / 2.f, 
                                            MARGIN + GRID_PIXEL_TOTAL + 20.f + (BUTTON_HEIGHT - bounds.size.y) / 2.f - 4.f));
                rText.setFillColor(sf::Color::Black);
                window.draw(rText);
            }
        }

        sf::RectangleShape undoBtn(sf::Vector2f((float)BUTTON_WIDTH, (float)BUTTON_HEIGHT));
        undoBtn.setPosition(sf::Vector2f(RIGHT_GRID_OFFSET_X + BUTTON_WIDTH + 20.f, MARGIN + GRID_PIXEL_TOTAL + 20.f));
        undoBtn.setFillColor(sf::Color(180, 180, 180)); 
        undoBtn.setOutlineThickness(2.f);
        undoBtn.setOutlineColor(sf::Color(100, 100, 100));
        window.draw(undoBtn);

        if (hasFont) {
            sf::Text uText(font, "UNDO", 18);
            sf::FloatRect bounds = uText.getLocalBounds();
            uText.setPosition(sf::Vector2f(RIGHT_GRID_OFFSET_X + BUTTON_WIDTH + 20.f + (BUTTON_WIDTH - bounds.size.x) / 2.f, 
                                        MARGIN + GRID_PIXEL_TOTAL + 20.f + (BUTTON_HEIGHT - bounds.size.y) / 2.f - 4.f));
            uText.setFillColor(sf::Color::Black);
            window.draw(uText);
        }

        window.display();
    }

    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
    }

    return 0;
}
