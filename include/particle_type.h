#ifndef PARTICLE_TYPE_H
#define PARTICLE_TYPE_H

#include "raylib.h"
#include "qualities.h"

/**
 * Holds a type of particle that can be replicated a lot.
 */
class ParticleType {
protected:
    // The particle's radius. Consistent across all particles of the same type.
    float radius;

    // The default qualities, such as mass, or charge, of all particles of this type.
    Qualities default_qualities;

    Color default_color;

public:
    // Constructor.
    ParticleType(float radius, Qualities qualities, Color color);

    // Gets the value of a quality of the particle type.
    float getQuality(QualityTypes type);

    Qualities getQualities();

    // Get the radius of this type of particle.
    float getRadius();

    Color getDefaultColor();
};

#endif