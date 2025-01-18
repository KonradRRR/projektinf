#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>

const int TILE_SIZE = 32;
const int MAP_WIDTH = 21;
const int MAP_HEIGHT = 21;

const int WINDOW_WIDTH = MAP_WIDTH * TILE_SIZE;
const int WINDOW_HEIGHT = MAP_HEIGHT * TILE_SIZE;

int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,2,0,0,1,0,0,0,1,1,1,0,0,0,1,0,0,2,0,1},
    {1,0,1,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,1,0,0,0,1,1,1,0,0,0,1,0,0,0,0,1},
    {1,1,1,1,0,1,1,1,0,0,0,0,0,1,1,1,0,1,1,1,1},
    {1,0,0,1,0,0,0,1,1,1,0,1,1,1,0,0,0,1,0,0,1},
    {1,0,2,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,2,0,1},
    {1,0,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

sf::Vector2i randomDirection() {
    int dir = rand() % 4;
    if (dir == 0) return { 0, -1 };
    if (dir == 1) return { 0, 1 };
    if (dir == 2) return { -1, 0 };
    return { 1, 0 };
}

struct Ghost {
    sf::CircleShape shape;
    sf::Vector2i position;
    sf::Vector2i direction;

    Ghost(sf::Color color, sf::Vector2i startPos, sf::Vector2i startDir) {
        shape = sf::CircleShape(TILE_SIZE / 2 - 2);
        shape.setFillColor(color);
        position = startPos;
        direction = startDir;
    }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pac-Man SFML");
    window.setFramerateLimit(60);

    sf::RectangleShape wall(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    wall.setFillColor(sf::Color::Blue);

    sf::CircleShape pacman(TILE_SIZE / 2 - 2);
    pacman.setFillColor(sf::Color::Yellow);
    sf::Vector2i pacmanPos(1, 1);

    sf::Vector2i direction(0, 0);
    int frameCounter = 0, moveDelay = 15, score = 0;

    std::vector<Ghost> ghosts = {
        Ghost(sf::Color::Red, {10, 7}, {0, -1}),
        Ghost(sf::Color::Cyan, {10, 8}, {0, 1}),
        Ghost(sf::Color::Magenta, {1, 13}, {1, 0}),
        Ghost(sf::Color::Green, {19, 1}, {-1, 0})
    };

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "B³¹d ³adowania czcionki!" << std::endl;
        return -1;  // Zakoñcz program, jeœli nie uda siê za³adowaæ czcionki
    }
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(WINDOW_WIDTH - 150, 10);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) direction = { 0, -1 };
                if (event.key.code == sf::Keyboard::S) direction = { 0, 1 };
                if (event.key.code == sf::Keyboard::A) direction = { -1, 0 };
                if (event.key.code == sf::Keyboard::D) direction = { 1, 0 };
            }
        }

        if (++frameCounter >= moveDelay) {
            frameCounter = 0;
            sf::Vector2i newPos = pacmanPos + direction;
            if (map[newPos.y][newPos.x] != 1) {
                if (map[newPos.y][newPos.x] == 2) {
                    score++;
                    map[newPos.y][newPos.x] = 0;
                }
                pacmanPos = newPos;
            }
            for (auto& ghost : ghosts) {
                sf::Vector2i nextPos = ghost.position + ghost.direction;
                if (map[nextPos.y][nextPos.x] == 1) ghost.direction = randomDirection();
                else ghost.position = nextPos;
                if (ghost.position == pacmanPos) window.close();
            }
        }

        window.clear();
        for (int y = 0; y < MAP_HEIGHT; ++y)
            for (int x = 0; x < MAP_WIDTH; ++x)
                if (map[y][x] == 1) { wall.setPosition(x * TILE_SIZE, y * TILE_SIZE); window.draw(wall); }
                else if (map[y][x] == 2) { // Dodajmy rysowanie punktów
                    sf::CircleShape point(5);
                    point.setFillColor(sf::Color::White);
                    point.setPosition(x * TILE_SIZE + TILE_SIZE / 2 - 5, y * TILE_SIZE + TILE_SIZE / 2 - 5);
                    window.draw(point);
                }
        pacman.setPosition(pacmanPos.x * TILE_SIZE + 2, pacmanPos.y * TILE_SIZE + 2);
        window.draw(pacman);
        for (auto& ghost : ghosts) {
            ghost.shape.setPosition(ghost.position.x * TILE_SIZE + 2, ghost.position.y * TILE_SIZE + 2);
            window.draw(ghost.shape);
        }

        // Wyœwietlanie wyniku
        scoreText.setString("Score: " + std::to_string(score));
        window.draw(scoreText);

        window.display();
    }
    return 0;
}
