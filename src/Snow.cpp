#include "Snow.h"

#include "Random.h"

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