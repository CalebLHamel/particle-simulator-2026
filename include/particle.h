#ifndef PARTICLE_H
#define PARTICLE_H

#include "raylib.h"
#include "qualities.h"
#include "particle_type.h"

class Particle : public ParticleType {
private:
    Vector2 position;
    Vector2 prior_position;
    Vector2 velocity;
    Qualities qualities;
    Color color;

public:
    Particle(float radius, Vector2 position, Vector2 velocity, Qualities qualities, Color color);

    static Particle makeParticleFromType(ParticleType type, Vector2 position, Vector2 velocity);

    Vector2 getPosition();
    Vector2 getPriorPosition();
    Vector2 getVelocity();

    void setPosition(Vector2 position);
    void setPriorPosition(Vector2 position);
    void setVelocity(Vector2 velocity);

    void readyForNewPosition();

    Color getColor();
    void setColor(Color color);

    float getQuality(QualityTypes type);
    void setQuality(QualityTypes type, float value);

    float getRadius();
};

#endif