#include "particle_type.hpp"
#include "raymath.h"

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