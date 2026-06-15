#ifndef FORCE_FUNCTIONS_HPP
#define FORCE_FUNCTIONS_HPP

#include "qualities.hpp"
#include "particle.hpp"
#include <algorithm>

// A ForceFunction takes a set of qualities and a position difference to compute something used for forces.
typedef Vector2 (*PartialForceFunction)(Qualities qualities, Vector2 position_difference);
typedef Vector2 (*FinalForceFunction)(Particle* particle, Vector2 partial_force);
typedef Vector2 (*ParticleForceFunction)(Particle* particle, Particle* other_particle);

/**
 * A basic template for force functions.
 * 
 * This one computes the 'potential' force that could exist.
 * E.g. A mass, on its own, may cause a gravitational force
 *      if there is another particle. This calculation is
 *      used with a basicFinalForce that then determines what
 *      happens to the particle.
 * 
 * E    : The quality type associated with this force.
 */
template <QualityTypes E>
Vector2 basicPartialForce(Qualities qualities, Vector2 position_difference) {
    float distance_squared = Vector2LengthSqr(position_difference);
    Vector2 direction = Vector2Normalize(position_difference);

    float scale = qualities.getQuality(E) / distance_squared;
    
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
 * E     : The quality type associated with this force.
 * scale : How much to scale the force by.
 */
template <QualityTypes E, int quality_scale>
Vector2 basicFinalForce(Particle* particle, Vector2 partial_force) {
    return Vector2Scale(partial_force, quality_scale*(particle->getQuality(E)));
}

/**
 * A basic template for force functions.
 * 
 * This one computes the force between two particles directly.
 * It can have more complex behaviour for this reason, such as
 * behaviours that only occur locally.
 * E             : The quality type associated with this force.
 * OtherE        : The other particle's type to associate with this force.
 * scale : How much to scale the force by.
 */
template <QualityTypes E, QualityTypes OtherE, int quality_scale>
Vector2 particleForce(Particle* particle, Particle* other_particle) {
    Vector2 position_difference = Vector2Subtract(other_particle->getPosition(), particle->getPosition());
    float distance = Vector2Length(position_difference);
    if (distance < particle->getRadius() + other_particle->getRadius()) {
        return {0,0};
    }
    Vector2 direction = Vector2Normalize(position_difference);
    float final_scale = quality_scale * particle->getQuality(E) * other_particle->getQuality(OtherE) / (distance*distance);
    return Vector2Scale(direction, final_scale);
}



Vector2 funnyPartial(Qualities qualities, Vector2 position_difference) {
    float x = Vector2Length(position_difference);
    float funny_magnitude = qualities.getQuality(Charge)*(x-40)*pow(2.71, -((x-40)*(x-40))/2000)/10000;

    Vector2 direction = Vector2Normalize(position_difference);
    return Vector2Scale(direction, funny_magnitude);
}
Vector2 funnyFinal(Particle* particle, Vector2 partial_force) {
    return Vector2Scale(partial_force, particle->getQuality(Charge));
}
Vector2 particleFunny(Particle* particle, Particle* other_particle) {
    Vector2 position_difference = Vector2Subtract(particle->getPosition(), other_particle->getPosition());

    if (Vector2Length(position_difference) < 1.25 * (particle->getRadius() + other_particle->getRadius())) {
        return {0,0};
    }
    
    Vector2 partial = funnyPartial(other_particle->getQualities(), position_difference);
    Vector2 complete = funnyFinal(particle, partial);

    float charge = particle->getQuality(Charge);
    float other_charge = other_particle->getQuality(Charge);
    float x = Vector2Length(position_difference);
    float funny_magnitude = (x-40)*pow(2.71, -((x-40)*(x-40))/2000)/10000;

    Vector2 direction = Vector2Normalize(position_difference);
    float damper = 0;
    if (true || (charge > 0 && other_charge < 0) || (charge < 0 && other_charge < 0)) {
        damper = funny_magnitude * Vector2DotProduct(Vector2Subtract(other_particle->getVelocity(), particle->getVelocity()), direction);
    }

    Vector2 damper_vector = Vector2Scale(direction, damper);
    return Vector2Add(damper_vector, complete);
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
// It works, but is just tedious and requires being careful with ordering.
constexpr PartialForceFunction PARTIAL_FORCE_FUNCTIONS[FORCE_COUNT] = {
    //basicPartialForce<Mass>,
    //funnyPartial,
    basicPartialForce<Charge>,
    //    basicPartialForce<Strange_1>,
    //    basicPartialForce<Strange_2>,
    //    basicPartialForce<Strange_3>,
    //    basicPartialForce<Strange_1>,
    //    basicPartialForce<Strange_2>,
    //    basicPartialForce<Strange_3>,
    //    basicPartialForce<Strange_1>,
    //    basicPartialForce<Strange_2>,
    //    basicPartialForce<Strange_3>
};
constexpr FinalForceFunction FINAL_FORCE_FUNCTIONS[FORCE_COUNT] = {
    //basicFinalForce<Mass,1>,
    //funnyFinal,
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
constexpr ParticleForceFunction PARTICLE_FORCE_FUNCTIONS[FORCE_COUNT] = {
    //particleForce<Mass,Mass,1>,
    //particleFunny,
    particleForce<Charge,Charge,-1>,
    //    particleForce<Strange_1,1>,
    //    particleForce<Strange_1,-1>,
    //    particleForce<Strange_1,1>,
    //    particleForce<Strange_2,1>,
    //    particleForce<Strange_2,1>,
    //    particleForce<Strange_2,-1>,
    //    particleForce<Strange_3,-1>,
    //    particleForce<Strange_3,1>,
    //    particleForce<Strange_3,1>    
};

#endif