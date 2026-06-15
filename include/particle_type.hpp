#ifndef PARTICLE_TYPE_HPP
#define PARTICLE_TYPE_HPP

#include "raylib.h"
#include "qualities.hpp"
#include <vector>

typedef Vector2 (*ForceFunction)(Qualities qualities, Vector2 position_difference);

#define FORCE_COUNT (1)
Vector2 partialForce(Qualities qualities, Vector2 position_difference);
Vector2 finalizeForce(Qualities particle_qualities, Vector2 partial_force);
constexpr ForceFunction PARTIAL_FORCE_FUNCTIONS[] = {partialForce};
constexpr ForceFunction FINAL_FORCE_FUNCTIONS[] = {finalizeForce};
/*
class IForceEffects {
    public:
    virtual ~IForceEffects() {}
    virtual Vector2 partialForce(Qualities qualities, Vector2 position_difference) { return {0,0}; };
    virtual Vector2 finalizeForce(Qualities particle_qualities, Vector2 partial_force) { return {0,0}; };
};

class GravityForce : public IForceEffects {
    public:
    Vector2 partialForce(Qualities qualities, Vector2 position_difference);
    Vector2 finalizeForce(Qualities particle_qualities, Vector2 partial_force);
};
*/

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