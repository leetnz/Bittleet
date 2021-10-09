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

Angles Attitude::update(const GravityMeasurement& g) {
    return Angles {
        .roll = -(float)atan2(g.y, g.z),
        .pitch = -(float)atan2(g.x, g.z)
    };
}

}

