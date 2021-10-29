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

#include <Arduino.h>
#include <stdint.h>
#include "Status.h"

namespace Attitude {

enum class Axis : uint8_t {
    Yaw = 0,
    Pitch,
    Roll
};

struct GravityMeasurement {
    int16_t x, y, z;
};

class Attitude {
public:
    Attitude() = default;
    Attitude(float filterCoeff) : _filterCoeff(filterCoeff){}

    bool update(const GravityMeasurement& gravity);

    float angleFromAxis(Axis axis);
    float angleFromAxis(int8_t axis);
    float roll() { return _roll; }
    float pitch() { return _pitch; }
    void reset();
private:
    float _roll = 0.0;
    float _pitch = 0.0;
    float _filterCoeff = 1.0; // 1.0 = no filtering, value should be between [0.0, 1.0]
    bool _reset = true;
};

}

#endif // _BITTLEET_ATTITUDE_H_
