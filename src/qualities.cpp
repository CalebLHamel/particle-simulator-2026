#include "qualities.hpp"
#include <cstddef>

/**
 * Constructor.
 * Initializes all qualities to 0, other than restitution and mass which are set to 1.
 */
Qualities::Qualities() {
    for (size_t i=0; i<QualityTypes::COUNT; i+=1) {
        this->qualities[i] = 0;
    }
    this->qualities[Mass] = 1;
    this->qualities[Restitution] = 1;
};

/**
 * Sets a quality type to a specified value.
 * quality  : The type of quality to set.
 * value    : The value to set it to.
 */
void Qualities::setQuality(QualityTypes quality, float value) {
    this->qualities[quality] = value;
};

/**
 * Gets the value of a quality type.
 * type : The type of quality to get.
 */
float Qualities::getQuality(QualityTypes type) {
    return this->qualities[type];
};

/**
 * Add the qualities of another set to this one.
 * qualities : The other set to add to this one.
 */
void Qualities::addQualities(Qualities qualities) {
    for (size_t i=0; i<COUNT; i+=1) {
        this->qualities[i] += qualities.qualities[i];
    }
}