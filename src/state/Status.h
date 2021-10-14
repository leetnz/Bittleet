//
// Status Interface for classes which express a status.
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//


#ifndef _BITTLEET_STATUS_H_
#define _BITTLEET_STATUS_H_

#include <stdint.h>

namespace Status {

enum class BatteryLevel : uint8_t {
    None = 0,
    Low,
    Ok
};

struct Battery {
    BatteryLevel level;
    uint8_t percent;
};

// Interface for status reporting classes.
class StatusReporter {
public:
    virtual void statusBattery(Battery& status) = 0;
};

} // namespace Status

#endif // _BITTLEET_STATUS_H_