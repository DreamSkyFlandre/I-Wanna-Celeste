#include "Snow.h"

#include "Random.h"

int Snow::getSnowWidth() const {
    return snowWidth;
}

void Snow::setSnowWidth(int width) {
    snowWidth = width;
}

int Snow::getSnowHeight() const {
    return snowHeight;
}

void Snow::setSnowHeight(int height) {
    snowHeight = height;
}

int Snow::getVelocityX() const {
    return velocityX;
}

void Snow::setVelocityX(int velocity) {
    velocityX = velocity;
}

int Snow::getVelocityY() const {
    return velocityY;
}

void Snow::setVelocityY(int velocity) {
    velocityY = velocity;
}

int Snow::getAngleVelocity() const {
    return angleVelocity;
}

void Snow::setAngleVelocity(int velocity) {
    angleVelocity = velocity;
}

sf::Sprite& Snow::getSprite() {
    return snowSprite;
}

const sf::Sprite& Snow::getSprite() const {
    return snowSprite;
}

void Snow::spawn(const sf::Texture& texture, int windowWidth) {
    snowSprite.setTexture(texture);
    float scaleX = (float)snowWidth / texture.getSize().x;
    float scaleY = (float)snowHeight / texture.getSize().y;
    snowSprite.setScale(scaleX, scaleY);

    int posX, posY;
    int choice = Random::nextInt(10);
    if (choice < 7) {
        posY = -snowHeight;
        posX = LEFT_BORDER + Random::nextInt(windowWidth - LEFT_BORDER);
    } else {
        posX = windowWidth;
        posY = Random::nextInt(LOW_BORDER) - snowHeight;
    }

    snowSprite.setPosition(posX, posY);
    snowSprite.setOrigin(snowWidth / 2.f, snowHeight / 2.f);
}

void Snow::move(float dt) {
    snowSprite.move(-velocityX * dt, velocityY * dt);
    snowSprite.rotate(angleVelocity * dt);
}