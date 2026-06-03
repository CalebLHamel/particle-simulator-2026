#ifndef QUALITIES_H
#define QUALITIES_H

//std::vector<float> qualities[QualityTypes::COUNT]; // This pattern may be useful for a future idea.

/**
 * All the different qualities that it is possible for a particle to hold.
 */
enum QualityTypes {
    Mass,
    Charge,
    COUNT // Not a type, just used to count how many qualities there are. Always keep this enum last, as it's used to count the number of quality types.
};

/**
 * A set of qualities and values.
 */
class DefaultQualities {
private:
    // Each quality type has a floating point value.
    float qualities[QualityTypes::COUNT];

public:
    // Constructor.
    DefaultQualities();

    // Sets a quality value and returns it again.
    void SetQuality(QualityTypes quality, float value);

    // Gets the value of a quality type.
    float getDefaultQuality(QualityTypes type);
};

#endif