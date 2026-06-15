#ifndef PARTICLE_TYPE_HPP
#define PARTICLE_TYPE_HPP

#include "raylib.h"
#include "raymath.h"
#include "qualities.hpp"
#include <vector>

// A ForceFunction takes a set of qualities and a position difference to compute something used for forces.
typedef Vector2 (*ForceFunction)(Qualities qualities, Vector2 position_difference);

/**
 * A basic template for force functions.
 * 
 * This one computes the 'potential' force that could exist.
 * E.g. A mass, on its own, may cause a gravitational force
 *      if there is another particle. This calculation is
 *      used with a basicFinalForce that then determines what
 *      happens to the particle.
 * 
 * E             : The quality type associated with this force.
 * quality_scale : How much to scale the quality type value in this calculation.
 */
template <QualityTypes E, int quality_scale>
Vector2 basicPartialForce(Qualities qualities, Vector2 position_difference) {
    float distance_squared = Vector2LengthSqr(position_difference);
    Vector2 direction = Vector2Normalize(position_difference);
    // To avoid extremes, don't include really close forces.
    if (distance_squared < 4) { // 4 is arbitrary.
        return {0,0};
    }
    float scale = quality_scale * qualities.getQuality(E) / distance_squared;
    
    
    return Vector2Scale(direction, scale);
}

/**
 * A basic template for force functions.
 * 
 * This one computes the force that exists, taking the results of the
 * partial calculation and then accounting for what it does to an
 * actual particle.
 * E.g. The partial force might be from a bunch of other particles with charge,
 *      and the final calculation then considers a particle's own charge to
 *      determine the force it experiences.
 * 
 * E             : The quality type associated with this force.
 * quality_scale : How much to scale the quality type value in this calculation.
 */
template <QualityTypes E, int quality_scale>
Vector2 basicFinalForce(Qualities particle_qualities, Vector2 partial_force) {
    return Vector2Scale(partial_force, quality_scale*particle_qualities.getQuality(E));
}

// Defining a constant list of force calculations.
#define FORCE_COUNT (1)
/*
 * Each partial force function determines the potential created by certain qualities.
 * The respective final force function determines how other qualities react.
 * E.g. mass reacts with mass, charge reacts with charge, and we can set up arbitrary,
 *      and even symmetric, forces. E.g. we may set up some funny_quality_1 that creates
 *      potential, and only particles with funny_quality_2 attract.
 * 
 * Forces typically take the bulk of the calculation time, so I tried to use a constexpr list of
 * functions with the aim of making it easier for the compiler to optimize things.
 * An older attempt used polymorphism, but the slowdowns were drastic.
 */

// TODO: This feels inelegant and could probably be further refined.  
constexpr ForceFunction PARTIAL_FORCE_FUNCTIONS[FORCE_COUNT] = {
//    basicPartialForce<Mass,1>,
    basicPartialForce<Charge,1>,
//    basicPartialForce<Strange_1,1>,
//    basicPartialForce<Strange_2,1>,
//    basicPartialForce<Strange_3,1>,
//    basicPartialForce<Strange_1,1>,
//    basicPartialForce<Strange_2,1>,
//    basicPartialForce<Strange_3,1>,
//    basicPartialForce<Strange_1,1>,
//    basicPartialForce<Strange_2,1>,
//    basicPartialForce<Strange_3,1>
};
constexpr ForceFunction FINAL_FORCE_FUNCTIONS[FORCE_COUNT] = {
//    basicFinalForce<Mass,1>,
    basicFinalForce<Charge,-1>,
//    basicFinalForce<Strange_1,1>,
//    basicFinalForce<Strange_1,-1>,
//    basicFinalForce<Strange_1,1>,
//    basicFinalForce<Strange_2,1>,
//    basicFinalForce<Strange_2,1>,
//    basicFinalForce<Strange_2,-1>,
//    basicFinalForce<Strange_3,-1>,
//    basicFinalForce<Strange_3,1>,
//    basicFinalForce<Strange_3,1>    
};

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