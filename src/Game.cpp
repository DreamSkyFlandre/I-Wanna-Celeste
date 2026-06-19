#include "Game.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>

#include "Random.h"

Game::Game() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "I Wanna Celeste"), instShow(true), state(GameState::MENU) {
    window.setFramerateLimit(FRAME_RATE);

    if (!font.loadFromFile("../resources/arial.ttf")) {
        exit(EXIT_FAILURE);
    }

    if (!bgmMenu.openFromFile("../resources/Prologue.ogg") || !bgmGaming.openFromFile("../resources/First Steps.ogg")) {
        exit(EXIT_FAILURE);
    } else {
        bgmMenu.setLoop(true);
        bgmGaming.setLoop(true);
        bgmMenu.play();
        bgmMenu.setVolume(MAX_VOLUME);
    }

    if (!backgroundTexture.loadFromFile("../resources/background.png") || !landTexture.loadFromFile("../resources/land.png")) {
        exit(EXIT_FAILURE);
    } else {
        background.setTexture(backgroundTexture);
        land.setTexture(landTexture);
    }

    sf::Texture tmpTexture;

    for (int i = 0; i < 4; ++i) {
        if (!tmpTexture.loadFromFile("../resources/run" + std::to_string(i) + ".png")) {
            exit(EXIT_FAILURE);
        } else {
            runTextures.push_back(tmpTexture);
        }
    }

    for (int i = 0; i < 2; ++i) {
        if (!tmpTexture.loadFromFile("../resources/jump" + std::to_string(i) + ".png")) {
            exit(EXIT_FAILURE);
        } else {
            riseTextures.push_back(tmpTexture);
        }
    }

    for (int i = 0; i < 2; ++i) {
        if (!tmpTexture.loadFromFile("../resources/fall" + std::to_string(i) + ".png")) {
            exit(EXIT_FAILURE);
        } else {
            fallTextures.push_back(tmpTexture);
        }
    }

    if (!spikeTexture.loadFromFile("../resources/spike.png") || !snowTexture.loadFromFile("../resources/snowflake.png")) {
        exit(EXIT_FAILURE);
    }

    kid.setTexture(runTextures[0]);
    kid.setGroundPos(GROUND_POS);

    scoreText.setFont(font);
    scoreText.setCharacterSize(40);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20, 20);

    titleText.setFont(font);
    titleText.setCharacterSize(200);

    subText.setFont(font);
    subText.setCharacterSize(60);

    std::ifstream inputFile("highscore.txt");
    if (inputFile.is_open()) {
        inputFile >> highScore;
        inputFile.close();
    } else {
        highScore = 0;
    }
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (state == GameState::MENU) {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                resetGame();
                bgmMenu.stop();
                bgmGaming.play();
                bgmGaming.setVolume(MAX_VOLUME);
                currentVolume = MAX_VOLUME;
            }
        }

        if (state == GameState::GAME_OVER) {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                resetGame();
            }
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
            if (state == GameState::PLAYING) {
                state = GameState::PAUSED;
            }
            else if (state == GameState::PAUSED) {
                state = GameState::PLAYING;
            }
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            if (state == GameState::PAUSED || state == GameState::GAME_OVER) {
                state = GameState::MENU;
                bgmGaming.stop();
                bgmMenu.play();
                bgmMenu.setVolume(MAX_VOLUME);
            }
            else if (state == GameState::MENU) {
                window.close();
            }
        }
    }
}

void Game::update() {
    float dt = clock.getElapsedTime().asSeconds();
    clock.restart();

    if (state == GameState::PLAYING) {
        if (currentVolume < MAX_VOLUME) {
            currentVolume += VOLUME_CHANGE_SPEED * dt;
            if (currentVolume > MAX_VOLUME) currentVolume = MAX_VOLUME;
            bgmGaming.setVolume(currentVolume);
        }
    }
    else if (state == GameState::GAME_OVER) {
        if (currentVolume > 0.f) {
            currentVolume -= VOLUME_CHANGE_SPEED * dt;
            if (currentVolume < 0.f) currentVolume = 0.f;
            bgmGaming.setVolume(currentVolume);
        }
    }
    else if (state == GameState::PAUSED) {
        if (currentVolume > TARGET_PAUSED_VOLUME) {
            currentVolume -= VOLUME_CHANGE_SPEED * dt;
            if (currentVolume < TARGET_PAUSED_VOLUME) currentVolume = TARGET_PAUSED_VOLUME;
            bgmGaming.setVolume(currentVolume);
        }
        else if (currentVolume < TARGET_PAUSED_VOLUME) {
            currentVolume += VOLUME_CHANGE_SPEED * dt;
            if (currentVolume > TARGET_PAUSED_VOLUME) currentVolume = TARGET_PAUSED_VOLUME;
            bgmGaming.setVolume(currentVolume);
        }
    }

    if (state != GameState::PLAYING) return;

    if (instShow) {
        instTimer += dt;
        if (instTimer > 5.f) instShow = false;
    }

    kidTimer += dt;
    if (kidTimer >= KID_UPDATE_DELAY) {
        kidTimer -= KID_UPDATE_DELAY;

        sf::Texture* currentTexture = nullptr;

        switch (kid.getState()) {
            case Kid::KidState::RUNNING:
                currentTexture = &runTextures[runFrame];
                runFrame = (runFrame + 1) % runTextures.size();
                break;
            case Kid::KidState::JUMPING:
            case Kid::KidState::RISING:
                currentTexture = &riseTextures[riseFrame];
                riseFrame = (riseFrame + 1) % riseTextures.size();
                break;
            case Kid::KidState::FALLING:
                currentTexture = &fallTextures[fallFrame];
                fallFrame = (fallFrame + 1) % fallTextures.size();
                break;
        }

        if (currentTexture) {
            kid.setTexture(*currentTexture);
        }
    }

    kid.move(dt);

    spikeTimer += dt;
    if (spikeTimer >= spikeDelay) {
        spikeTimer -= spikeDelay;
        spikeDelay = std::max(INITIAL_SPIKE_DELAY - score / 1000.f + Random::nextInt(SPIKE_DELAY_RANGE) / 1000.f, MIN_SPIKE_DELAY);

        Spike newSpike;
        newSpike.setSpikeWidth(MIN_SPIKE_WIDTH + Random::nextInt(SPIKE_WIDTH_RANGE));
        newSpike.setSpikeHeight(MIN_SPIKE_HEIGHT + Random::nextInt(SPIKE_HEIGHT_RANGE));
        newSpike.setVelocityX(std::min(MIN_SPIKE_SPEED + score / 2 + Random::nextInt(SPIKE_SPEED_RANGE), MAX_SPIKE_SPEED));
        newSpike.setPassed(false);
        newSpike.spawn(spikeTexture, WINDOW_WIDTH, GROUND_POS);

        spikes.push_back(newSpike);
    }

    if (!spikes.empty()) {
        if (spikes[0].getSprite().getPosition().x < -spikes[0].getSpikeWidth()) {
            spikes.pop_front();
        }
    }

    for (auto& spike : spikes) {
        spike.move(dt);

        float spikeRight = spike.getSprite().getPosition().x + spike.getSpikeWidth();
        float kidLeft = kid.getSprite().getPosition().x;

        if (spikeRight < kidLeft && !spike.isPassed()) {
            spike.setPassed(true);
            score += 10;
            if (score > highScore) {
                highScore = score;
            }
        }
    }

    sf::FloatRect kidBounds = kid.getSprite().getGlobalBounds();
    kidBounds.width *= 0.35f;
    kidBounds.height *= 0.66f;
    kidBounds.left += kid.getSprite().getGlobalBounds().width * 0.34f;
    kidBounds.top += kid.getSprite().getGlobalBounds().height * 0.34f;

    for (auto& spike : spikes) {
        if (!kidBounds.intersects(spike.getSprite().getGlobalBounds())) {
            continue;
        }

        sf::Vector2f spikePos = spike.getSprite().getPosition();
        float spikeW = (float)spike.getSpikeWidth();
        float spikeH = (float)spike.getSpikeHeight();

        sf::Vector2f top(spikePos.x + spikeW / 2.f, spikePos.y);
        sf::Vector2f botLeft(spikePos.x, spikePos.y + spikeH);
        sf::Vector2f botRight(spikePos.x + spikeW, spikePos.y + spikeH);

        std::vector<sf::Vector2f> kidPoints;
        kidPoints.push_back(sf::Vector2f(kidBounds.left, kidBounds.top + kidBounds.height));
        kidPoints.push_back(sf::Vector2f(kidBounds.left + kidBounds.width, kidBounds.top + kidBounds.height));
        kidPoints.push_back(sf::Vector2f(kidBounds.left + kidBounds.width / 2.f, kidBounds.top + kidBounds.height));

        bool isDead = false;
        for (const auto& point : kidPoints) {
            if (isPointInTriangle(point, top, botLeft, botRight)) {
                isDead = true;
                break;
            }
        }

        if (isDead) {
            state = GameState::GAME_OVER;
            std::ofstream outputFile("highscore.txt");
            if (outputFile.is_open()) {
                outputFile << highScore;
                outputFile.close();
            }
        }
    }

    snowTimer += dt;
    if (snowTimer >= snowDelay) {
        snowTimer -= snowDelay;
        snowDelay = MIN_SNOW_DELAY + Random::nextInt(SNOW_DELAY_RANGE) / 1000.f;

        Snow newSnowflake;
        newSnowflake.setSnowWidth(MIN_SNOW_SIZE + Random::nextInt(SNOW_SIZE_RANGE));
        newSnowflake.setSnowHeight(newSnowflake.getSnowWidth());
        newSnowflake.setVelocityX(MIN_SNOW_XSPEED + Random::nextInt(SNOW_XSPEED_RANGE));
        newSnowflake.setVelocityY(MIN_SNOW_YSPEED + Random::nextInt(SNOW_YSPEED_RANGE));
        newSnowflake.setAngleVelocity(MIN_SNOW_ANGLE_SPEED + Random::nextInt(SNOW_ANGLE_SPEED_RANGE));
        newSnowflake.spawn(snowTexture, WINDOW_WIDTH);

        snowflakes.push_back(newSnowflake);
    }

    if (!snowflakes.empty()) {
        if (snowflakes[0].getSprite().getPosition().x < -snowflakes[0].getSnowWidth() / 2.f || snowflakes[0].getSprite().getPosition().y > GROUND_POS + snowflakes[0].getSnowHeight() / 2.f) {
            snowflakes.pop_front();
        }
    }

    for (auto& snow : snowflakes) {
        snow.move(dt);
    }
}

void Game::render() {
    window.clear();

    switch (state) {
        case GameState::MENU:
            window.draw(background);
            window.draw(land);

            drawTitle("I Wanna Celeste", sf::Color(0, 192, 255));
            drawSubtext("High Score: " + std::to_string(highScore) + "\nPress ENTER to Start\nPress ESC to Exit", sf::Color::White);
            break;
        case GameState::PLAYING:
            window.draw(background);
            drawSnow();
            window.draw(land);
            window.draw(kid.getSprite());
            drawSpikes();

            scoreText.setString("High Score: " + std::to_string(highScore) + "\nScore: " + std::to_string(score));
            window.draw(scoreText);
            if (instShow) drawSubtext("Press SPACE to Jump\nPress P to Pause", sf::Color::White);
            break;
        case GameState::GAME_OVER:
            window.draw(background);
            window.draw(land);

            drawTitle("Game Over", sf::Color(128, 0, 0));
            drawSubtext("Your Score: " + std::to_string(score) + "\nHigh Score: " + std::to_string(highScore) + "\nPress R to Restart\nPress ESC to Menu", sf::Color::White);
            break;
        case GameState::PAUSED:
            window.draw(background);
            drawSnow();
            window.draw(land);
            window.draw(kid.getSprite());
            drawSpikes();

            sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
            overlay.setFillColor(sf::Color(0, 0, 0, 150));
            window.draw(overlay);

            drawTitle("Paused", sf::Color(0, 192, 255));
            drawSubtext("Press P to Resume\nPress ESC to Menu", sf::Color::White);
            break;
    }

    window.display();
}

void Game::resetGame() {
    kid.reset();
    score = 0;
    state = GameState::PLAYING;
    kidTimer = 0.f;
    spikeTimer = 0.f;
    snowTimer = 0.f;
    instTimer = 0.f;
    clock.restart();
    spikeDelay = INITIAL_SPIKE_DELAY + Random::nextInt(SPIKE_DELAY_RANGE) / 1000.f;
    spikes.clear();
    snowDelay = MIN_SNOW_DELAY + Random::nextInt(SNOW_DELAY_RANGE) / 1000.f;
    snowflakes.clear();
    runFrame = 0;
    riseFrame = 0;
    fallFrame = 0;
}

void Game::drawTitle(std::string title, sf::Color color) {
    titleText.setString(title);
    titleText.setFillColor(color);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width / 2.f, titleBounds.height / 2.f);
    titleText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 3.6f);

    window.draw(titleText);
}

void Game::drawSubtext(std::string subtext, sf::Color color) {
    subText.setString(subtext);
    subText.setFillColor(color);
    sf::FloatRect subTextBounds = subText.getLocalBounds();
    subText.setOrigin(subTextBounds.width / 2.f, subTextBounds.height / 2.f);
    subText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 1.8f);

    window.draw(subText);
}

void Game::drawSpikes() {
    for (auto& spike : spikes) {
        window.draw(spike.getSprite());
    }
}

float Game::sign(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool Game::isPointInTriangle(sf::Vector2f pt, sf::Vector2f v1, sf::Vector2f v2, sf::Vector2f v3) {
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(pt, v1, v2);
    d2 = sign(pt, v2, v3);
    d3 = sign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

void Game::drawSnow() {
    for (auto& snow : snowflakes) {
        window.draw(snow.getSprite());
    }
}