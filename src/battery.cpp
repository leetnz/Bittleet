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

#define HIGH_BATT_MV (8200)
#define LOW_BATT_MV (6400)
#define LOW_BATT_COUNT (LOW_BATT_MV/MV_PER_COUNT)

#define BATT_RANGE_MV (HIGH_BATT_MV - LOW_BATT_MV)

namespace Battery {
    
Status::Battery state(int adcRead) {
    if (adcRead <= NO_BATT_MV) {
        return Status::Battery{
            .level = Status::BatteryLevel::None, 
            .percent = 0,
        };
    }
    if (adcRead <= LOW_BATT_COUNT) {
        return Status::Battery{
            .level = Status::BatteryLevel::Low, 
            .percent = 0,
        };
    }
    const int32_t percent = (100 * (adcRead * MV_PER_COUNT - LOW_BATT_MV)) / BATT_RANGE_MV;
    const uint8_t clampedPercent =  (percent > 100) ? 100u : (percent < 0) ? 0u : (uint8_t)percent;
    return Status::Battery{
        .level = Status::BatteryLevel::Ok,
        .percent = clampedPercent,
    };
}

}
