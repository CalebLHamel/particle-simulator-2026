#ifndef PARTICLE_H
#define PARTICLE_H

#include "raylib.h"
#include "particle_type.h"

/**
 * The particle object used in the simulation.
 */
class Particle : public ParticleType {
private:
    // Where the particle is in the simulation.
    Vector2 position;

    // Where the particle was on the previous frame of the simulation.
    Vector2 prior_position;

    // The particle's velocity.
    Vector2 velocity;

    // The particle's qualities.
    Qualities qualities;

    // The particle's color.
    Color color;

public:
    // Constructor.
    Particle(float radius, Vector2 position, Vector2 velocity, Qualities qualities, Color color);

    // Makes a particle out of a particle type, meant for easy replication of similar particles.
    static Particle makeParticleFromType(ParticleType type, Vector2 position, Vector2 velocity);

    // Gets the current position of the particle.
    Vector2 getPosition();

    // Gets the particle's position from the previous frame.
    Vector2 getPriorPosition();

    // Gets the particle's current velocity.
    Vector2 getVelocity();

    // Sets the particle's position.
    void setPosition(Vector2 position);

    // Sets the particle's prior position.
    void setPriorPosition(Vector2 position);

    // Sets the particle's velocity.
    void setVelocity(Vector2 velocity);

    // Sets the prior position to the present position.
    void readyForNewPosition();

    // Gets the particle's color.
    Color getColor();

    // Sets the particle's color.
    void setColor(Color color);

    // Gets a quality of the particle.
    float getQuality(QualityTypes type);

    // Sets a quality of the particle.
    void setQuality(QualityTypes type, float value);

    // Gets the particle's radius.
    float getRadius();
};

#endif