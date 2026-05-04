#pragma once

#include <SFML/Graphics.hpp>

class Snow {
private:
    const int LEFT_BORDER = 300;
    const int LOW_BORDER = 600;

public:
    int snowWidth;
    int snowHeight;
    int velocityX;
    int velocityY;
    int angleVelocity;

    sf::Sprite snowSprite;

    void spawn(const sf::Texture& texture, int windowWidth);
    void move(float dt);
};