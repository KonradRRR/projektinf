#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>

const int TILE_SIZE = 32;
const int MAP_WIDTH = 21;
const int MAP_HEIGHT = 15;
const int WINDOW_WIDTH = MAP_WIDTH * TILE_SIZE;
const int WINDOW_HEIGHT = MAP_HEIGHT * TILE_SIZE;
const float CHASE_DURATION = 5.0f;  // Czas trwania trybu po¿igu w sekundach
const float CHASE_INTERVAL = 15.0f;  // Odstêp miêdzy trybami po¿igu

int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,2,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,2,0,1},
    {1,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,0,1,1,1,0,0,0,0,0,1,1,1,0,1,1,1,1},
    {1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1},
    {1,0,2,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,2,0,1},
    {1,0,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

struct Point {
    sf::Vector2i position;
    sf::CircleShape shape;
    bool active;

    Point(sf::Vector2i pos) : position(pos), active(true) {
        shape = sf::CircleShape(5);
        shape.setFillColor(sf::Color::White);
        shape.setPosition(pos.x * TILE_SIZE + TILE_SIZE / 2 - 5,
            pos.y * TILE_SIZE + TILE_SIZE / 2 - 5);
    }
};

sf::Vector2i randomDirection() {
    int dir = rand() % 4;
    if (dir == 0) return { 0, -1 };
    if (dir == 1) return { 0, 1 };
    if (dir == 2) return { -1, 0 };
    return { 1, 0 };
}

float distance(sf::Vector2i a, sf::Vector2i b) {
    return std::sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

sf::Vector2i getDirectionToTarget(sf::Vector2i current, sf::Vector2i target) {
    std::vector<sf::Vector2i> possibleDirections = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0}
    };

    float minDist = 999999.0f;
    sf::Vector2i bestDir = randomDirection();

    for (const auto& dir : possibleDirections) {
        sf::Vector2i newPos = current + dir;
        if (newPos.x >= 0 && newPos.x < MAP_WIDTH &&
            newPos.y >= 0 && newPos.y < MAP_HEIGHT &&
            map[newPos.y][newPos.x] != 1) {
            float dist = distance(newPos, target);
            if (dist < minDist) {
                minDist = dist;
                bestDir = dir;
            }
        }
    }

    return bestDir;
}

struct Ghost {
    sf::CircleShape shape;
    sf::Vector2i position;
    sf::Vector2i direction;
    bool isChasing;
    sf::Color normalColor;

    Ghost(sf::Color color, sf::Vector2i startPos, sf::Vector2i startDir)
        : normalColor(color), isChasing(false) {
        shape = sf::CircleShape(TILE_SIZE / 2 - 2);
        shape.setFillColor(color);
        position = startPos;
        direction = startDir;
    }

    void updateDirection(const sf::Vector2i& pacmanPos, bool chaseMode) {
        if (chaseMode) {
            direction = getDirectionToTarget(position, pacmanPos);
            shape.setFillColor(sf::Color::Red);
        }
        else {
            if (map[position.y + direction.y][position.x + direction.x] == 1) {
                direction = randomDirection();
            }
            shape.setFillColor(normalColor);
        }
    }
};

class Player {
public:
    sf::CircleShape shape;
    sf::Vector2i position;
    sf::Vector2i direction;
    int score;
    std::string profileName;

    Player(const std::string& name) : position(1, 1), direction(0, 0), score(0), profileName(name) {
        shape = sf::CircleShape(TILE_SIZE / 2 - 2);
        shape.setFillColor(sf::Color::Yellow);
    }

    void move() {
        sf::Vector2i newPos = position + direction;
        if (map[newPos.y][newPos.x] != 1) {
            position = newPos;
        }
    }

    void updateScore(Point& point) {
        if (point.active && point.position == position) {
            point.active = false;
            score += 10;
        }
    }

    void saveScore() {
        std::ofstream file(profileName + ".txt", std::ios::app);
        if (file.is_open()) {
            file << "Score: " << score << std::endl;
            file.close();
        }
    }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    std::string profileName;
    std::cout << "Enter your profile name: ";
    std::getline(std::cin, profileName);

    Player player(profileName);

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pac-Man SFML");
    window.setFramerateLimit(60);

    sf::Clock gameClock;
    float lastChaseTime = 0.0f;
    bool chaseMode = false;
    float chaseStartTime = 0.0f;
    bool isPaused = false;

    sf::RectangleShape wall(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    wall.setFillColor(sf::Color::Blue);

    std::vector<Point> points;
    int totalPoints = 0;
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (map[y][x] == 0) {
                points.push_back(Point(sf::Vector2i(x, y)));
                totalPoints++;
            }
        }
    }

    int frameCounter = 0, moveDelay = 15;

    std::vector<Ghost> ghosts = {
        Ghost(sf::Color::Magenta, {10, 7}, {0, -1}),
        Ghost(sf::Color::Cyan, {10, 8}, {0, 1}),
        Ghost(sf::Color::Green, {1, 13}, {1, 0}),
        Ghost(sf::Color(255, 165, 0), {19, 1}, {-1, 0})
    };

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Font loading error!" << std::endl;
        return -1;
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(0, 0);

    sf::Text modeText;
    modeText.setFont(font);
    modeText.setCharacterSize(24);
    modeText.setFillColor(sf::Color::White);
    modeText.setPosition(WINDOW_WIDTH - 200, 10);

    sf::Text pauseText;
    pauseText.setFont(font);
    pauseText.setCharacterSize(32);
    pauseText.setFillColor(sf::Color::Yellow);
    pauseText.setString("Game Paused\nPress R to Resume\nPress Q to Quit");
    pauseText.setPosition(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 50);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    isPaused = !isPaused;
                }
                if (isPaused) {
                    if (event.key.code == sf::Keyboard::R) {
                        isPaused = false;
                    }
                    if (event.key.code == sf::Keyboard::Q) {
                        player.saveScore();
                        window.close();
                    }
                }
                else {
                    if (event.key.code == sf::Keyboard::W) player.direction = { 0, -1 };
                    if (event.key.code == sf::Keyboard::S) player.direction = { 0, 1 };
                    if (event.key.code == sf::Keyboard::A) player.direction = { -1, 0 };
                    if (event.key.code == sf::Keyboard::D) player.direction = { 1, 0 };
                }
            }
        }

        if (isPaused) {
            window.clear();
            window.draw(pauseText);
            window.display();
            continue;
        }

        float currentTime = gameClock.getElapsedTime().asSeconds();

        if (!chaseMode && currentTime - lastChaseTime >= CHASE_INTERVAL) {
            chaseMode = true;
            chaseStartTime = currentTime;
            lastChaseTime = currentTime;
        }
        else if (chaseMode && currentTime - chaseStartTime >= CHASE_DURATION) {
            chaseMode = false;
        }

        if (++frameCounter >= moveDelay) {
            frameCounter = 0;
            player.move();
            for (auto& point : points) {
                player.updateScore(point);
            }

            if (std::all_of(points.begin(), points.end(), [](const Point& p) { return !p.active; })) {
                player.saveScore();
                window.close();
                std::cout << "You win! Score: " << player.score << std::endl;
            }

            for (auto& ghost : ghosts) {
                ghost.updateDirection(player.position, chaseMode);
                sf::Vector2i nextPos = ghost.position + ghost.direction;
                if (map[nextPos.y][nextPos.x] != 1) {
                    ghost.position = nextPos;
                }
                if (ghost.position == player.position) {
                    player.saveScore();
                    window.close();
                    std::cout << "Game Over! Score: " << player.score << std::endl;
                }
            }
        }

        window.clear();

        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                if (map[y][x] == 1) {
                    wall.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                    window.draw(wall);
                }
            }
        }

        for (const auto& point : points) {
            if (point.active) {
                window.draw(point.shape);
            }
        }

        player.shape.setPosition(player.position.x * TILE_SIZE + 2, player.position.y * TILE_SIZE + 2);
        window.draw(player.shape);

        for (auto& ghost : ghosts) {
            ghost.shape.setPosition(ghost.position.x * TILE_SIZE + 2, ghost.position.y * TILE_SIZE + 2);
            window.draw(ghost.shape);
        }

        scoreText.setString("Score: " + std::to_string(player.score));
        window.draw(scoreText);

        modeText.setString(chaseMode ? "CHASE MODE!" : "Normal Mode");
        modeText.setFillColor(chaseMode ? sf::Color::Red : sf::Color::White);
        window.draw(modeText);

        window.display();
    }
    return 0;
}
