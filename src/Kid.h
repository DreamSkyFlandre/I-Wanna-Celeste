#pragma once

#include <SFML/Graphics.hpp>

class Kid {
private:
    const int KID_WIDTH = 128;
    const int KID_HEIGHT = 128;

    const int X_POS = 300;
    float posY;
    float velocityY;

    const float GRAVITY_JUMP_HOLD = 400.f;
    const float GRAVITY_NATURAL = 3200.f;
    const float INITIAL_VELOCITY = -1150.f;
    const float MAX_JUMP_TIME = 0.24f;
    float jumpTimer;
    bool wasJumpPressed;

public:
    sf::Sprite kidSprite;
    int groundPos;

    enum class KidState {
        RUNNING,
        JUMPING,
        RISING,
        FALLING
    };

    KidState kidState;

    void reset();
    void setTexture(const sf::Texture& texture);
    void move(float dt);

private:
    void startJump();
};