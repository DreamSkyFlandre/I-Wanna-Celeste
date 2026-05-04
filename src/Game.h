#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio/Music.hpp>
#include <vector>
#include <deque>
#include <string>

#include "Kid.h"
#include "Spike.h"
#include "Snow.h"

class Game {
private:
    const int WINDOW_WIDTH = 1920;
    const int WINDOW_HEIGHT = 1080;
    sf::RenderWindow window;
    const int FRAME_RATE = 50;
    const int GROUND_POS = 913;

    std::vector<sf::Texture> runTextures;
    std::vector<sf::Texture> riseTextures;
    std::vector<sf::Texture> fallTextures;
    Kid kid;
    sf::Sprite background;
    sf::Texture backgroundTexture;
    sf::Sprite land;
    sf::Texture landTexture;
    sf::Texture spikeTexture;
    std::deque<Spike> spikes;
    sf::Texture snowTexture;
    std::deque<Snow> snowflakes;

    enum class GameState {
        MENU,
        PLAYING,
        GAME_OVER,
        PAUSED
    };

    GameState state;
    sf::Clock clock;
    float kidTimer;
    float spikeTimer;
    float snowTimer;
    float instTimer;
    const float KID_UPDATE_DELAY = 0.1f;
    float spikeDelay;
    float snowDelay;
    int score;
    int highScore;
    bool instShow;

    int runFrame;
    int riseFrame;
    int fallFrame;

    const float INITIAL_SPIKE_DELAY = 1.5f;
    const int SPIKE_DELAY_RANGE = 600;
    const float MIN_SPIKE_DELAY = 0.7f;
    const int MIN_SPIKE_WIDTH = 40;
    const int SPIKE_WIDTH_RANGE = 200;
    const int MIN_SPIKE_HEIGHT = 50;
    const int SPIKE_HEIGHT_RANGE = 250;
    const int MIN_SPIKE_SPEED = 400;
    const int SPIKE_SPEED_RANGE = 150;
    const int MAX_SPIKE_SPEED = 1200;

    const float MIN_SNOW_DELAY = 0.1f;
    const int SNOW_DELAY_RANGE = 300;
    const int MIN_SNOW_SIZE = 20;
    const int SNOW_SIZE_RANGE = 60;
    const int MIN_SNOW_XSPEED = 300;
    const int SNOW_XSPEED_RANGE = 900;
    const int MIN_SNOW_YSPEED = 200;
    const int SNOW_YSPEED_RANGE = 600;
    const int MIN_SNOW_ANGLE_SPEED = -120;
    const int SNOW_ANGLE_SPEED_RANGE = 240;

    sf::Font font;
    sf::Text scoreText;
    sf::Text titleText;
    sf::Text subText;

    sf::Music bgmMenu;
    sf::Music bgmGaming;
    float currentVolume;
    const float MAX_VOLUME = 50.f;
    const float TARGET_PAUSED_VOLUME = 15.f;
    const float VOLUME_CHANGE_SPEED = 20.f;

public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();
    void resetGame();
    void drawTitle(std::string title, sf::Color color);
    void drawSubtext(std::string subtext, sf::Color color);
    void drawSpikes();
    float sign(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3);
    bool isPointInTriangle(sf::Vector2f pt, sf::Vector2f v1, sf::Vector2f v2, sf::Vector2f v3);
    void drawSnow();
};