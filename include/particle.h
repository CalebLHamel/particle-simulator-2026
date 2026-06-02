#ifndef PARTICLE_H
#define PARTICLE_H

#include "raylib.h"

class Particle {
private:
    float radius;
    Vector2 position;

public:
    Particle(float radius, Vector2 position);

    Vector2 getPosition();
    void setPosition(Vector2 position);

    float getRadius();
};

#endif