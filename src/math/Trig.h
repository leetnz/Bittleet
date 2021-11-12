//
// Trigonometry Defintions
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//


#ifndef _BITTLEET_TRIG_H_
#define _BITTLEET_TRIG_H_

#define M_DEG2RAD (M_PI / 180.0)
#define M_RAD2DEG (180.0 / M_PI)

inline float wrapPiToNegPi(float v){
    while (v > M_PI) {
        v -= (float)(2.0 * M_PI);
    }
    while (v < -M_PI) {
        v += (float)(2.0 * M_PI);
    }
    return v;
}

// Return the shortest path from a to b
inline float shortestRadianPath(float a, float b) {
    const float out = b - a;
    if (out > M_PI) {
        return b - (a + (float)(2.0 * M_PI));
    } else if (out < -M_PI) {
        return b - (a - (float)(2.0 * M_PI));
    }
    return out;
}

#endif // _BITTLEET_TRIG_H_


