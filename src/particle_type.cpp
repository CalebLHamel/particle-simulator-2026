#include "particle_type.h"

// Constructor.
ParticleType::ParticleType(float radius, DefaultQualities qualities, Color color) {
    this->radius = radius;
    this->default_qualities = qualities;

    this->positions = std::vector<Vector2>();
    this->velocities = std::vector<Vector2>();
    this->default_color = color;
};

/**
 * Adds a particle with initial position and velocity.
 */
void ParticleType::addParticle(Vector2 position, Vector2 velocity) {
    this->positions.push_back(position);
    this->prior_positions.push_back(position);
    this->velocities.push_back(velocity);
};

/**
 * Gets the value of the default quality of this type.
 */
float ParticleType::getQuality(QualityTypes type) {
    return this->default_qualities.getDefaultQuality(type);
};

/**
 * Gets the position of a particle of this type.
 */
Vector2 ParticleType::getPosition(int index) {
    return this->positions[index];
};

/**
 * Sets the position of a particle of this type.
 */
void ParticleType::setPosition(int index, Vector2 position) {
    this->positions[index] = position;
};

/**
 * Gets the prior position of a particle of this type.
 */
Vector2 ParticleType::getPriorPosition(int index) {
    return this->prior_positions[index];
};

/**
 * Sets the prior position of a particle of this type.
 */
void ParticleType::setPriorPosition(int index, Vector2 position) {
    this->prior_positions[index] = position;
};

/**
 * Sets the prior positions to the current positions.
 * Intended to track where a particle was as the simulation attempts to figure out where it will end up.
 */
void ParticleType::readyForNextPosition() {
    this->prior_positions = this->positions;
}

/**
 * Gets the velocity of a particle of this type.
 */
Vector2 ParticleType::getVelocity(int index) {
    return this->velocities[index];
};

/**
 * Sets the velocity of a particle of this type.
 */
void ParticleType::setVelocity(int index, Vector2 velocity) {
    this->velocities[index] = velocity;
};

/**
 * Gets the radius of this type of particle.
 */
float ParticleType::getRadius() {
    return this->radius;
};

/**
 * Gets the default color of this type of particle.
 */
Color ParticleType::getDefaultColor() {
    return this->default_color;
}