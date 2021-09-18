//
// Battery
// Battery State Monitor
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#ifndef _BITTLEET_BATTERY_H_ 
#define _BITTLEET_BATTERY_H_

#include <stdint.h>

namespace Battery {

    enum class Direction : uint8_t {
    Forward = 0,
    Left,
    Right,
    TOTAL
};

enum class State : uint8_t {
    None = 0,
    Low,
    Ok
};

State state(int adcRead);

}

#endif // _BITTLEET_BATTERY_H_
