#pragma once

#include <SFML/Graphics.hpp>

class Kid {
public:
    enum class KidState {
        RUNNING,
        JUMPING,
        RISING,
        FALLING
    };

private:
    const int KID_WIDTH = 128;
    const int KID_HEIGHT = 128;

    const int X_POS = 300;
    float posY = 0.f;
    float velocityY = 0.f;

    const float GRAVITY_JUMP_HOLD = 400.f;
    const float GRAVITY_NATURAL = 3200.f;
    const float INITIAL_VELOCITY = -1150.f;
    const float MAX_JUMP_TIME = 0.24f;
    float jumpTimer = 0.f;
    bool wasJumpPressed = false;
    sf::Sprite kidSprite;
    int groundPos = 0;

    KidState kidState = KidState::RUNNING;

public:

    void reset();
    void setTexture(const sf::Texture& texture);
    void move(float dt);
    sf::Sprite& getSprite();
    const sf::Sprite& getSprite() const;
    KidState getState() const;
    void setState(KidState state);
    int getGroundPos() const;
    void setGroundPos(int position);

private:
    void startJump();
};