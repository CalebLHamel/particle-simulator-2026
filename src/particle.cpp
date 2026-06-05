#include "particle.h"

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

Vector2 Particle::getPosition() {
    return position;
};

void Particle::setPosition(Vector2 position) {
    this->position = position;
};

Vector2 Particle::getPriorPosition() {
    return prior_position;
};

void Particle::setPriorPosition(Vector2 position) {
    this->position = prior_position;
};

Vector2 Particle::getVelocity() {
    return velocity;
};

void Particle::setVelocity(Vector2 velocity) {
    this->velocity = velocity;
};


float Particle::getRadius() {
    return radius;
};

void Particle::readyForNewPosition() {
    this->prior_position = position;
};

float Particle::getQuality(QualityTypes type) {
    return this->qualities.getQuality(type);
};

void Particle::setQuality(QualityTypes type, float value) {
    this->qualities.setQuality(type, value);
}

Color Particle::getColor() {
    return this->color;
}

void Particle::setColor(Color color) {
    this->color = color;
}