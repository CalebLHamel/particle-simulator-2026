#ifndef QUALITIES_HPP
#define QUALITIES_HPP

/**
 * All the different qualities that it is possible for a particle to hold.
 * This basically means any number we want a particle to possess, for any reason (with the exception of radius, position, and velocity).
 */
enum QualityTypes {
    Mass,
    Charge,
    Restitution,
    Compression,
    COUNT // Not a type, just used to count how many qualities there are. Always keep this enum last, as it's used to count the number of quality types.
};

/**
 * A set of qualities and values.
 */
class Qualities {
private:
    // Each quality type has a floating point value.
    float qualities[QualityTypes::COUNT];

public:
    // Constructor.
    Qualities();

    // Sets a quality value and returns it again.
    void setQuality(QualityTypes quality, float value);

    // Gets the value of a quality type.
    float getQuality(QualityTypes type);

    // Element-wise add another set of qualities into this one.
    void addQualities(Qualities qualities);
};

#endif