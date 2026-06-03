#include "particle.h"

// Unused. Was once used for trying some things out.

Particle::Particle(float radius, Vector2 position) {
    this->radius = radius;
    this->position = position;
};

Vector2 Particle::getPosition() {
    return position;
};

void Particle::setPosition(Vector2 position) {
    this->position = position;
};

float Particle::getRadius() {
    return radius;
}