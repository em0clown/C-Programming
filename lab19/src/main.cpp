#include "paint_core.hpp"
#include <iostream>
#include <stack>

std::stack<Canvas> undoStack;
enum class Tool { BRUSH, RECTANGLE, LINE, ERASER };

int main() {
    const int UI_HEIGHT = 120;
    sf::RenderWindow window(sf::VideoMode(WIDTH * PIXEL_SIZE, HEIGHT * PIXEL_SIZE + UI_HEIGHT), "Paint++");
    
    Canvas canvas(HEIGHT, std::vector<sf::Color>(WIDTH, sf::Color::White));
    clearCanvas(canvas);

    Tool currentTool = Tool::BRUSH;
    sf::Color currentColor = sf::Color::Black;
    int brushRadius = 3;

    std::vector<sf::Color> palette = {
        sf::Color::Black, sf::Color::White, sf::Color::Red, sf::Color::Green,
        sf::Color::Blue, sf::Color::Yellow, sf::Color::Cyan, sf::Color::Magenta,
        sf::Color(255, 128, 0), sf::Color(128, 0, 128)
    };
    int selectedPaletteIndex = 0;
    bool isDrawing = false;
    int startX = 0, startY = 0, lastX = 0, lastY = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::B: currentTool = Tool::BRUSH; break;
                    case sf::Keyboard::R: currentTool = Tool::RECTANGLE; break;
                    case sf::Keyboard::L: currentTool = Tool::LINE; break;
                    case sf::Keyboard::E: currentTool = Tool::ERASER; break;
                    case sf::Keyboard::C: saveToUndo(canvas); clearCanvas(canvas); break;
                    case sf::Keyboard::Z: if (event.key.control) undo(canvas); break;
                    
                    case sf::Keyboard::G: applyGrayscale(canvas); break;
                    case sf::Keyboard::N: applyNegative(canvas); break;
                    case sf::Keyboard::M: applyBlur(canvas); break;

                    case sf::Keyboard::Equal: brushRadius = std::min(20, brushRadius + 1); break;
                    case sf::Keyboard::Hyphen: brushRadius = std::max(1, brushRadius - 1); break;

                    case sf::Keyboard::Num0: case sf::Keyboard::Num1: 
                    case sf::Keyboard::Num9: {
                        int idx = event.key.code - sf::Keyboard::Num0;
                        if (idx < palette.size()) {
                            selectedPaletteIndex = idx;
                            currentColor = palette[idx];
                        }
                        break;
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mouse = sf::Mouse::getPosition(window);
                int paletteY = HEIGHT * PIXEL_SIZE + 50;
                if (mouse.y >= paletteY && mouse.y <= paletteY + 30) {
                    for (int i = 0; i < palette.size(); ++i) {
                        int boxX = 10 + i * 35;
                        if (mouse.x >= boxX && mouse.x <= boxX + 30) {
                            selectedPaletteIndex = i;
                            currentColor = palette[i];
                        }
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mouse = sf::Mouse::getPosition(window);
                if (mouse.y < HEIGHT * PIXEL_SIZE) {
                    isDrawing = true;
                    startX = mouse.x / PIXEL_SIZE;
                    startY = mouse.y / PIXEL_SIZE;
                    saveToUndo(canvas);
                    if (currentTool == Tool::BRUSH || currentTool == Tool::ERASER) {
                        sf::Color col = (currentTool == Tool::ERASER) ? sf::Color::White : currentColor;
                        drawBrush(canvas, startX, startY, col, brushRadius);
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                if (isDrawing && (currentTool == Tool::RECTANGLE || currentTool == Tool::LINE)) {
                    sf::Vector2i mouse = sf::Mouse::getPosition(window);
                    sf::Color col = (currentTool == Tool::ERASER) ? sf::Color::White : currentColor;
                    if (currentTool == Tool::RECTANGLE) drawRectangle(canvas, startX, startY, mouse.x / PIXEL_SIZE, mouse.y / PIXEL_SIZE, col);
                    else drawLine(canvas, startX, startY, mouse.x / PIXEL_SIZE, mouse.y / PIXEL_SIZE, col);
                }
                isDrawing = false;
            }

            if (event.type == sf::Event::MouseMoved && isDrawing) {
                if (currentTool == Tool::BRUSH || currentTool == Tool::ERASER) {
                    sf::Vector2i mouse = sf::Mouse::getPosition(window);
                    sf::Color col = (currentTool == Tool::ERASER) ? sf::Color::White : currentColor;
                    drawBrush(canvas, mouse.x / PIXEL_SIZE, mouse.y / PIXEL_SIZE, col, brushRadius);
                }
            }
        }

        window.clear(sf::Color(200, 200, 200));
        renderToWindow(window, canvas);

        for (size_t i = 0; i < palette.size(); ++i) {
            sf::RectangleShape box(sf::Vector2f(30, 30));
            box.setFillColor(palette[i]);
            box.setPosition(10 + i * 35, HEIGHT * PIXEL_SIZE + 50);
            box.setOutlineThickness(2);
            box.setOutlineColor((i == selectedPaletteIndex) ? sf::Color::Red : sf::Color::Black);
            window.draw(box);
        }
        window.display();
    }
    return 0;
}