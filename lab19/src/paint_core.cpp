#include "paint_core.hpp"
#include <algorithm>
#include <cmath>
#include <stack>

extern std::stack<Canvas> undoStack;

void saveToUndo(const Canvas& canvas) {
    undoStack.push(canvas);
}

void clearCanvas(Canvas& canvas) {
    for (auto& row : canvas) {
        std::fill(row.begin(), row.end(), sf::Color::White);
    }
}

void drawPixel(Canvas& canvas, int x, int y, const sf::Color& color) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = color;
    }
}

void drawBrush(Canvas& canvas, int x, int y, const sf::Color& color, int radius) {
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (dx * dx + dy * dy <= radius * radius) {
                drawPixel(canvas, x + dx, y + dy, color);
            }
        }
    }
}

void drawRectangle(Canvas& canvas, int x1, int y1, int x2, int y2, const sf::Color& color) {
    int left = std::min(x1, x2);
    int right = std::max(x1, x2);
    int top = std::min(y1, y2);
    int bottom = std::max(y1, y2);

    for (int y = top; y <= bottom; ++y) {
        for (int x = left; x <= right; ++x) {
            drawPixel(canvas, x, y, color);
        }
    }
}

void drawLine(Canvas& canvas, int x1, int y1, int x2, int y2, const sf::Color& color) {
    int dx = std::abs(x2 - x1);
    int dy = -std::abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;

    while (true) {
        drawPixel(canvas, x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void undo(Canvas& canvas) {
    if (!undoStack.empty()) {
        canvas = undoStack.top();
        undoStack.pop();
    }
}

void applyGrayscale(Canvas& canvas) {
    saveToUndo(canvas);
    for (auto& row : canvas) {
        std::transform(row.begin(), row.end(), row.begin(), [](sf::Color c) {
            sf::Uint8 gray = static_cast<sf::Uint8>(c.r * 0.299 + c.g * 0.587 + c.b * 0.114);
            return sf::Color(gray, gray, gray);
        });
    }
}

void applyNegative(Canvas& canvas) {
    saveToUndo(canvas);
    for (auto& row : canvas) {
        std::transform(row.begin(), row.end(), row.begin(), [](sf::Color c) {
            return sf::Color(255 - c.r, 255 - c.g, 255 - c.b);
        });
    }
}

void applyBlur(Canvas& canvas) {
    saveToUndo(canvas);
    Canvas original = canvas;
    for (int y = 1; y < HEIGHT - 1; ++y) {
        for (int x = 1; x < WIDTH - 1; ++x) {
            int r = 0, g = 0, b = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    sf::Color n = original[y + dy][x + dx];
                    r += n.r; g += n.g; b += n.b;
                }
            }
            canvas[y][x] = sf::Color(r / 9, g / 9, b / 9);
        }
    }
}

void renderToWindow(sf::RenderWindow& window, const Canvas& canvas) {
    static sf::Image image;
    static sf::Texture texture;
    static sf::Sprite sprite;
    
    image.create(WIDTH, HEIGHT);
    for (int y = 0; y < HEIGHT; ++y)
        for (int x = 0; x < WIDTH; ++x)
            image.setPixel(x, y, canvas[y][x]);
    
    texture.loadFromImage(image);
    sprite.setTexture(texture);
    sprite.setScale(PIXEL_SIZE, PIXEL_SIZE);
    window.draw(sprite);
}