#include "particle.h"


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