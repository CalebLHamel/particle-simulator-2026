#include "particle.hpp"
#include "raymath.h"
#include <algorithm>
#include <vector>

/**
 * Constructor.
 * radius   : The particle's radius.
 * position : The particle's initial position.
 * velocity : The particle's initial velocity.
 * qualities: The particle's initial qualities.
 * color    : The particle's initial color (some noise will be added).
 */
Particle::Particle(float radius, Vector2 position, Vector2 velocity, Qualities qualities, Color color) : ParticleType(radius, qualities, color) {
    this->position = position;
    this->prior_position = position;
    this->velocity = velocity;
    this->qualities = qualities;

    // Adding some random noise to the color, constrained within 0:255 for each r,g,b.
    unsigned char r = color.r;
    unsigned char g = color.g;
    unsigned char b = color.b;
    r = std::min(255, std::max(0, r + GetRandomValue(-20,20)));
    b = std::min(255, std::max(0, b + GetRandomValue(-20,20)));
    g = std::min(255, std::max(0, g + GetRandomValue(-20,20)));
    this->color = Color{r,g,b, color.a};

};

/**
 * Makes a particle from a particle type. Helps set it up with some default values.
 * type     : The particle type to use as a template.
 * position : The initial position of the particle to use.
 * velocity : The initial velocity of the particle to use.
 */
Particle Particle::makeParticleFromType(ParticleType type, Vector2 position, Vector2 velocity) {
    return Particle(type.getRadius(), position, velocity, type.getQualities(), type.getDefaultColor());
};

/**
 * Gets the particle's current position.
 */
Vector2 Particle::getPosition() {
    return position;
};

/**
 * Sets the particle's current position.
 * position : The position to set the particle to.
 */
void Particle::setPosition(Vector2 position) {
    this->position = position;
};

/**
 * Gets the particle's prior position.
 */
Vector2 Particle::getPriorPosition() {
    return prior_position;
};

/**
 * Sets the particle's prior position.
 * position : The position to set the particle's prior position to.
 */
void Particle::setPriorPosition(Vector2 position) {
    this->position = prior_position;
};

/**
 * Gets the particle's current velocity.
 */
Vector2 Particle::getVelocity() {
    return velocity;
};

/**
 * Sets the particle's current velocity.
 * velocity : The velocity to set the particle to.
 */
void Particle::setVelocity(Vector2 velocity) {
    this->velocity = velocity;
};

/**
 * Gets the particle's radius.
 */
float Particle::getRadius() {
    return radius;
};

/**
 * Sets the prior position to the current position.
 * This is used after collision resolution once the new positions are confirmed.
 */
void Particle::readyForNewPosition() {
    this->prior_position = position;
};

/**
 * Gets a quality of the particle.
 * type : The quality type to get.
 */
float Particle::getQuality(QualityTypes type) {
    return this->qualities.getQuality(type);
};

/**
 * Sets a quality of the particle.
 * type  : The quality type to set.
 * value : The value to set the specified quality to.
 */
void Particle::setQuality(QualityTypes type, float value) {
    this->qualities.setQuality(type, value);
}

/**
 * Gets the particle's color.
 */
Color Particle::getColor() {
    return this->color;
}

/**
 * Sets the particle's color.
 */
void Particle::setColor(Color color) {
    this->color = color;
}