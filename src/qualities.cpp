#include "qualities.h"

/**
 * Constructor.
 */
DefaultQualities::DefaultQualities() {};

/**
 * Takes a DefaultQualities, sets the value of a quality type, and returns it again.
 * Intended for chained useage after the constructor.
 */
DefaultQualities* DefaultQualities::with(QualityTypes quality, float value) {
    this->qualities[quality] = value;
    return this;
};

/**
 * Gets the value of a quality type.
 */
float DefaultQualities::getDefaultQuality(QualityTypes type) {
    return this->qualities[type];
};