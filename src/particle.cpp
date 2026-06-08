#include "particle.hpp"

Particle::Particle(float radius, Vector2 position, Vector2 velocity, Qualities qualities, Color color) : ParticleType(radius, qualities, color) {
    this->position = position;
    this->prior_position = position;
    this->velocity = velocity;
    this->color = color;
    this->qualities = qualities;
};

/**
 * Makes a particle with initial position and velocity.
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
 */
void Particle::readyForNewPosition() {
    this->prior_position = position;
};

/**
 * Gets a quality of the particle.
 */
float Particle::getQuality(QualityTypes type) {
    return this->qualities.getQuality(type);
};

/**
 * Sets a quality of the particle.
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