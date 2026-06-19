#pragma once

#include <SFML/Graphics.hpp>

class Spike {
private:
    int spikeWidth = 0;
    int spikeHeight = 0;
    int velocityX = 0;
    bool passed = false;

    sf::Sprite spikeSprite;

public:

    void spawn(const sf::Texture& texture, int initialPosX, int groundPos);
    void move(float dt);
    int getSpikeWidth() const;
    void setSpikeWidth(int width);
    int getSpikeHeight() const;
    void setSpikeHeight(int height);
    int getVelocityX() const;
    void setVelocityX(int velocity);
    bool isPassed() const;
    void setPassed(bool value);
    sf::Sprite& getSprite();
    const sf::Sprite& getSprite() const;
};