#ifndef _BITTLEET_INFRARED_H_
#define _BITTLEET_INFRARED_H_

#include <Arduino.h>
#include "Command.h"


namespace Infrared {



Command::Command parseSignal(uint8_t signal, const Command::Move& move);

} // namespace Infrared

#endif // _BITTLEET_INFRARED_H_