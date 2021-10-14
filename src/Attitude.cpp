//
// Attitude
// Attitude Computation
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#include "Attitude.h"
#include <Arduino.h>

#define NOMINAL_G (16384)

#define NOMINAL_G2 (((int32_t)NOMINAL_G)*((int32_t)NOMINAL_G))

#define QUALITY_COEFF (1.5) // Removes any measurements that aren't within ~0.5g of 1g

namespace Attitude{

// In this context x and y are not cartesian coordinates.
// We are solving the equation:
//      y[k] = A*x[k] + B*y[k-1]
static float applyIIR(float xCurrent, float yLast, float coeff) {
    return coeff * xCurrent + (1.0f - coeff) * yLast;
}

bool Attitude::update(const GravityMeasurement& g) {
    const int32_t g2 = ((int32_t)g.x * (int32_t)g.x) + ((int32_t)g.y * (int32_t)g.y) + ((int32_t)g.z * (int32_t)g.z);
    int32_t diff2 = NOMINAL_G2 - g2;
    diff2 = diff2 < 0 ? -diff2 : diff2;
    double  measQuality = 1.25 - QUALITY_COEFF * ((double)diff2 / (double)NOMINAL_G2);
    if (measQuality < 0.0) {
        return false; // Measurement is out of bounds - reject it!
    }
    if (measQuality > 1.0) {
        measQuality = 1.0f;
    }

    const float rollMeasurement = -(float)atan2(g.y, g.z);
    const float pitchMeasurement = -(float)atan2(g.x, g.z);
    if (_reset) {
        _roll = rollMeasurement;
        _pitch = pitchMeasurement;
        _reset = false;
    } else {
        _roll = applyIIR(rollMeasurement, _roll, _filterCoeff * measQuality);
        _pitch = applyIIR(pitchMeasurement, _pitch, _filterCoeff * measQuality);
    }
    return true;
}

void Attitude::reset() {
    _roll = 0.0;
    _pitch = 0.0;
    _reset = true;
}

}

