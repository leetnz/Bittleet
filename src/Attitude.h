//
// Attitude
// Attitude Computation
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#ifndef _BITTLEET_ATTITUDE_H_ 
#define _BITTLEET_ATTITUDE_H_

#include <stdint.h>
#include "Status.h"

namespace Attitude {

struct GravityMeasurement {
    int16_t x, y, z;
};

struct Angles {
    float roll;
    float pitch;
};

class Attitude {
public:
    Attitude() = default;

    Angles update(const GravityMeasurement& gravity);
};

}

#endif // _BITTLEET_ATTITUDE_H_
