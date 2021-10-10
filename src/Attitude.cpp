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

#define QUALITY_COEFF (2.0) // Removes any measurements that aren't within 0.25g of 1g

namespace Attitude{

// In this context x and y are not cartesian coordinates.
// We are solving the equation:
//      y[k] = A*x[k] + B*y[k-1]
static float applyIIR(float xCurrent, float yLast, float coeff) {
    return coeff * xCurrent + (1.0f - coeff) * yLast;
}

void Attitude::update(const GravityMeasurement& g) {
    const int32_t g2 = ((int32_t)g.x * (int32_t)g.x) + ((int32_t)g.y * (int32_t)g.y) + ((int32_t)g.z * (int32_t)g.z);
    int32_t diff2 = NOMINAL_G2 - g2;
    diff2 = diff2 < 0 ? -diff2 : diff2;
    const double  measQuality = 1.0 - QUALITY_COEFF * ((double)diff2 / (double)NOMINAL_G2);
    if (measQuality < 0.0) {
        return; // Measurement is out of bounds - reject it!
    }

    _roll = applyIIR(-(float)atan2(g.y, g.z), _roll, _filterCoeff * measQuality);
    _pitch = applyIIR(-(float)atan2(g.x, g.z), _pitch, _filterCoeff * measQuality);
}

}

