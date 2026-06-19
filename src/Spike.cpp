#include "Spike.h"

int Spike::getSpikeWidth() const {
    return spikeWidth;
}

void Spike::setSpikeWidth(int width) {
    spikeWidth = width;
}

int Spike::getSpikeHeight() const {
    return spikeHeight;
}

void Spike::setSpikeHeight(int height) {
    spikeHeight = height;
}

int Spike::getVelocityX() const {
    return velocityX;
}

void Spike::setVelocityX(int velocity) {
    velocityX = velocity;
}

bool Spike::isPassed() const {
    return passed;
}

void Spike::setPassed(bool value) {
    passed = value;
}

sf::Sprite& Spike::getSprite() {
    return spikeSprite;
}

const sf::Sprite& Spike::getSprite() const {
    return spikeSprite;
}

void Spike::spawn(const sf::Texture& texture, int initialPosX, int groundPos) {
    spikeSprite.setTexture(texture);
    float scaleX = (float)spikeWidth / texture.getSize().x;
    float scaleY = (float)spikeHeight / texture.getSize().y;
    spikeSprite.setScale(scaleX, scaleY);
    spikeSprite.setPosition(initialPosX, groundPos - spikeHeight);
}

void Spike::move(float dt) {
    spikeSprite.move(-velocityX * dt, 0.f);
}