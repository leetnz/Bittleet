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

struct Vec3 {
    int16_t x, y, z;
};

struct Measurement {
    uint32_t us;
    Vec3 accel;
    Vec3 gyro;
};

class Attitude {
public:
    Attitude() = default;

    void update(const Measurement& m);
    void reset();

    float angleFromAxis(Axis axis) const;
    float angleFromAxis(int8_t axis) const;
    float roll() const { return _roll; }
    float pitch() const { return _pitch; }
protected:
    float _computeTrust(const Measurement& m) const;

    float _roll = 0.0;
    float _pitch = 0.0;
    uint32_t _usUpdate = 0;
    bool _reset = true;
};

}

#endif // _BITTLEET_ATTITUDE_H_
