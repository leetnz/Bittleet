//
// Battery
// Battery State Monitor
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#include "Battery.h"

#define MAX_MV (10000)
#define MV_PER_COUNT (10) // Actually 10.24, but close enough.

#define NO_BATT_MV (200)
#define NO_BATT_COUNT (NO_BATT_MV/MV_PER_COUNT)

#define LOW_BATT_MV (6400)
#define LOW_BATT_COUNT (LOW_BATT_MV/MV_PER_COUNT)

namespace Battery {
    
State state(int adcRead) {
    if (adcRead <= NO_BATT_MV) {
        return State::None;
    }
    if (adcRead <= LOW_BATT_COUNT) {
        return State::Low;  
    }
    return State::Ok;
}

}
