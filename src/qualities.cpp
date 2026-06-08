#include "qualities.hpp"
#include <cstddef>

/**
 * Constructor.
 */
Qualities::Qualities() {};

/**
 * Takes a DefaultQualities, sets the value of a quality type, and returns it again.
 * Intended for chained useage after the constructor.
 */
void Qualities::setQuality(QualityTypes quality, float value) {
    this->qualities[quality] = value;
};

/**
 * Gets the value of a quality type.
 */
float Qualities::getQuality(QualityTypes type) {
    return this->qualities[type];
};

/**
 * Add the qualities of another set to this one.
 */
void Qualities::addQualities(Qualities qualities) {
    for (size_t i=0; i<COUNT; i+=1) {
        this->qualities[i] += qualities.qualities[i];
    }
}