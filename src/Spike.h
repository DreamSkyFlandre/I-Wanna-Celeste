#pragma once

#include <SFML/Graphics.hpp>

class Spike {
public:
    int spikeWidth;
    int spikeHeight;
    int velocityX;
    bool passed;

    sf::Sprite spikeSprite;

    void spawn(const sf::Texture& texture, int initialPosX, int groundPos);
    void move(float dt);
};