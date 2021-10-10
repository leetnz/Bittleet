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
    Attitude(float filterCoeff) : _filterCoeff(filterCoeff){}

    void update(const GravityMeasurement& gravity);
    float roll() { return _roll; }
    float pitch() { return _pitch; }
private:
    float _roll = 0.0;
    float _pitch = 0.0;
    float _filterCoeff = 1.0; // 1.0 = no filtering, value should be between [0.0, 1.0]
};

}

#endif // _BITTLEET_ATTITUDE_H_
