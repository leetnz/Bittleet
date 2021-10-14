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
#include "Status.h"

namespace Battery {

Status::Battery state(int adcRead);

}

#endif // _BITTLEET_BATTERY_H_
