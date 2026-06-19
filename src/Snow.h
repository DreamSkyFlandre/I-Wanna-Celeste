#pragma once

#include <SFML/Graphics.hpp>

class Snow {
private:
    const int LEFT_BORDER = 300;
    const int LOW_BORDER = 600;
    int snowWidth = 0;
    int snowHeight = 0;
    int velocityX = 0;
    int velocityY = 0;
    int angleVelocity = 0;

    sf::Sprite snowSprite;

public:

    void spawn(const sf::Texture& texture, int windowWidth);
    void move(float dt);
    int getSnowWidth() const;
    void setSnowWidth(int width);
    int getSnowHeight() const;
    void setSnowHeight(int height);
    int getVelocityX() const;
    void setVelocityX(int velocity);
    int getVelocityY() const;
    void setVelocityY(int velocity);
    int getAngleVelocity() const;
    void setAngleVelocity(int velocity);
    sf::Sprite& getSprite();
    const sf::Sprite& getSprite() const;
};