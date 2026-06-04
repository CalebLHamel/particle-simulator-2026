#include "particle_type.h"
#include <raymath.h>

// Constructor.
ParticleType::ParticleType(float radius, DefaultQualities qualities, Color color) {
    this->radius = radius;
    this->default_qualities = qualities;

    this->positions = std::vector<Vector2>();
    this->prior_positions = std::vector<Vector2>();
    this->velocities = std::vector<Vector2>();
    this->momentum_changes = std::vector<Vector2>();
    this->default_color = color;
};

/**
 * Adds a particle with initial position and velocity.
 */
void ParticleType::addParticle(Vector2 position, Vector2 velocity) {
    this->positions.push_back(position);
    this->prior_positions.push_back(position);
    this->velocities.push_back(velocity);
    this->momentum_changes.push_back(Vector2{0,0});
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

void ParticleType::addMomentumChange(int index, Vector2 change) {
    this->momentum_changes[index] = Vector2Add(this->momentum_changes[index], change);
};

void ParticleType::applyMomentumChanges() {
    for (int i=0; i<this->velocities.size(); i++) {
        this->velocities[i] = Vector2Add(this->velocities[i], this->momentum_changes[i]);
        this->momentum_changes[i] = Vector2{0,0};
    }
};

/**
 * Gets the default color of this type of particle.
 */
Color ParticleType::getDefaultColor() {
    return this->default_color;
}