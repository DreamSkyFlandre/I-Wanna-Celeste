#include "Kid.h"

void Kid::reset() {
    posY = groundPos - KID_HEIGHT;
    velocityY = 0.f;
    kidState = KidState::RUNNING;
    wasJumpPressed = false;
    kidSprite.setPosition(X_POS, groundPos - KID_HEIGHT);
}

sf::Sprite& Kid::getSprite() {
    return kidSprite;
}

const sf::Sprite& Kid::getSprite() const {
    return kidSprite;
}

Kid::KidState Kid::getState() const {
    return kidState;
}

void Kid::setState(KidState state) {
    kidState = state;
}

int Kid::getGroundPos() const {
    return groundPos;
}

void Kid::setGroundPos(int position) {
    groundPos = position;
}

void Kid::setTexture(const sf::Texture& texture) {
    kidSprite.setTexture(texture);
    float scaleX = (float)KID_WIDTH / texture.getSize().x;
    float scaleY = (float)KID_HEIGHT / texture.getSize().y;
    kidSprite.setScale(scaleX, scaleY);
}

void Kid::move(float dt) {
    bool isJumpPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

    switch (kidState) {
        case KidState::RUNNING:
            if (isJumpPressed && !wasJumpPressed) {
                startJump();
            }
            break;
        case KidState::JUMPING:
            if (isJumpPressed && jumpTimer < MAX_JUMP_TIME) {
                velocityY += GRAVITY_JUMP_HOLD * dt;
                posY += velocityY * dt;
                jumpTimer += dt;
            } else {
                velocityY += GRAVITY_NATURAL * dt;
                posY += velocityY * dt;
                kidState = KidState::RISING;
            }
            break;
        case KidState::RISING:
            velocityY += GRAVITY_NATURAL * dt;
            posY += velocityY * dt;
            if (velocityY >= 0.f) {
                kidState = KidState::FALLING;
            }
            break;
        case KidState::FALLING:
            velocityY += GRAVITY_NATURAL * dt;
            posY += velocityY * dt;
            if (posY >= groundPos - KID_HEIGHT) {
                posY = groundPos - KID_HEIGHT;
                velocityY = 0.f;
                kidState = KidState::RUNNING;
            }
            break;
    }

    wasJumpPressed = isJumpPressed;
    kidSprite.setPosition(X_POS, posY);
}

void Kid::startJump() {
    velocityY = INITIAL_VELOCITY;
    kidState = KidState::JUMPING;
    jumpTimer = 0.f;
}