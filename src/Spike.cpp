#include "Spike.h"

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