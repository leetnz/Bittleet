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

class Attitude {
public:
    Attitude() = default;

    void update(const GravityMeasurement& gravity);
    float roll() { return _roll; }
    float pitch() { return _pitch; }
private:
    float _roll = 0.0;
    float _pitch = 0.0;
};

}

#endif // _BITTLEET_ATTITUDE_H_
