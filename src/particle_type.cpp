#include "particle_type.hpp"
#include "raymath.h"
#include <stdio.h>

Vector2 partialForce(Qualities qualities, Vector2 position_difference) {
    float distance_squared = Vector2LengthSqr(position_difference);
    Vector2 direction = Vector2Normalize(position_difference);
    float scale = qualities.getQuality(Mass) / distance_squared;

    return Vector2Scale(direction, scale);
}

Vector2 finalizeForce(Qualities particle_qualities, Vector2 partial_force) {
    return Vector2Scale(partial_force, particle_qualities.getQuality(Mass));
}

// Constructor.
ParticleType::ParticleType(float radius, Qualities qualities, Color color) {
    this->radius = radius;
    this->default_qualities = qualities;
    this->default_color = color;
};

/**
 * Gets the value of the default quality of this type.
 */
float ParticleType::getQuality(QualityTypes type) {
    return this->default_qualities.getQuality(type);
};

/**
 * Gets the default qualities set of this type of particle.
 */
Qualities ParticleType::getQualities() {
    return this->default_qualities;
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