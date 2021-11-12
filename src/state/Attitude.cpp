//
// Attitude
// Attitude Computation
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#include "Attitude.h"
#include "math/Filters.h"
#include <Arduino.h>

#define US_PER_SEC (1000000)
#define RAD_PER_S_PER_LSB (5.3263221801584764920766930190693e-4f)

#define NOMINAL_G (16384)

#define NOMINAL_G2 (((int32_t)NOMINAL_G)*((int32_t)NOMINAL_G))

#define ACCEL_COEFF (0.01f)

namespace Attitude {

void Attitude::update(const Measurement& m) {

    const float dt = (float)(m.us - _usUpdate)/(float)US_PER_SEC;

    float rollPredict = _roll + (0.5 * RAD_PER_S_PER_LSB * m.gyro.x) * dt;
    float pitchPredict = _pitch + (0.5 * RAD_PER_S_PER_LSB * m.gyro.y) * dt;

    const float trust = _computeTrust(m);
    if (trust == 0.0) {
        if (_reset == false) {
            _roll = rollPredict;
            _pitch = pitchPredict;
        }
    } else {
        const float rollMeasurement = (float)atan2(m.accel.y, m.accel.z);
        const float pitchMeasurement = (float)atan2(m.accel.x, m.accel.z);
        if (_reset) {
            _roll = rollMeasurement;
            _pitch = pitchMeasurement;
            _reset = false;
        } else {
            _roll = applyIIR(rollMeasurement, _roll, trust * ACCEL_COEFF);
            _pitch = applyIIR(pitchMeasurement, _pitch, trust * ACCEL_COEFF);
        }
    }
    _usUpdate = m.us;
}

float Attitude::_computeTrust(const Measurement& m) const {
    const int32_t accel2 = ((int32_t)m.accel.x * (int32_t)m.accel.x) +
                           ((int32_t)m.accel.y * (int32_t)m.accel.y) + 
                           ((int32_t)m.accel.z * (int32_t)m.accel.z);
    int32_t diff2 = accel2 - NOMINAL_G2;
    diff2 = (diff2 < 0) ? -diff2 : diff2;
    float trust = 1.0 - 10.0 * ((float)diff2 / (float)NOMINAL_G2);
    if (trust < 0.0) {
        return 0.0;
    }
    return trust;
}

void Attitude::reset() {
    _roll = 0.0;
    _pitch = 0.0;
    _reset = true;
}

float Attitude::angleFromAxis(Axis axis) const {
    switch (axis) {
        case Axis::Roll: {
            return _roll;
        }
        case Axis::Pitch: {
            return _pitch;
        }
        default: {
            return 0.0f;
        }
    }
}

float Attitude::angleFromAxis(int8_t axis) const {
    Axis a = static_cast<Axis>(abs(axis));
    return angleFromAxis(a); 
}



} // namespace Attitude

