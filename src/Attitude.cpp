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

namespace Attitude{

void Attitude::update(const GravityMeasurement& g) {
    _roll = -(float)atan2(g.y, g.z);
    _pitch = -(float)atan2(g.x, g.z);
}

}

