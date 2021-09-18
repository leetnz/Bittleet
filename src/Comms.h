#ifndef _BITTLEET_COMMS_H_
#define _BITTLEET_COMMS_H_

#include <Arduino.h>
#include "Command.h"

namespace Comms {

Command::Command parseSerial(Stream& serial, const Command::Move& lastMove, const int16_t* currentAngles);

}

#endif //_BITTLEET_COMMS_H_