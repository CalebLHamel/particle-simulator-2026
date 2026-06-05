#ifndef PARTICLE_TYPE_H
#define PARTICLE_TYPE_H

#include "raylib.h"
#include "qualities.h"

/**
 * Holds a type of particle that can be used to replicated a lot of similar particles.
 */
class ParticleType {
protected:
    // The particle's radius. Consistent across all particles of the same type.
    float radius;

    // The default qualities, such as mass, or charge, of all particles of this type.
    // A particle's qualities may change from this, but they'll start here.
    Qualities default_qualities;

    // The default color of this type of particle.
    Color default_color;

public:
    // Constructor.
    ParticleType(float radius, Qualities qualities, Color color);

    // Gets the value of a default quality of the particle type.
    float getQuality(QualityTypes type);

    // Gets the whole set of default qualities of this type.
    Qualities getQualities();

    // Get the radius of this type of particle.
    float getRadius();

    // Gets the default color of this type of particle.
    Color getDefaultColor();
};

#endif