#ifndef PARTICLE_TYPE_H
#define PARTICLE_TYPE_H

#include "raylib.h"
#include "particle.h"
#include "qualities.h"
#include <vector>

/**
 * Holds a type of particle that can be replicated a lot.
 */
class ParticleType {
private:
    // The particle's radius. Consistent across all particles of the same type.
    float radius;
    
    // The positions and velocities of all particles of this type.
    std::vector<Vector2> positions;
    std::vector<Vector2> velocities;
    
    // The default qualities, such as mass, or charge, of all particles of this type.
    DefaultQualities default_qualities;

public:
    // Constructor.
    ParticleType(float radius, DefaultQualities qualities);

    // Adds a particle with initial position and velocity.
    void addParticle(Vector2 position, Vector2 velocity);

    // Gets the value of a quality of the particle type.
    float getQuality(QualityTypes type);

    // Get a particles's position.
    Vector2 getPosition(int index);
    // Set a particle's position.
    void setPosition(int index, Vector2 position);

    // Get a particle's velocity.
    Vector2 getVelocity(int index);
    // Set a particle's velocity.
    void setVelocity(int index, Vector2 velocity);

    // Get the radius of this type of particle.
    float getRadius();
};

#endif