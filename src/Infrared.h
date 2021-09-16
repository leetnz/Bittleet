#ifndef _BITTLEET_INFRARED_H_
#define  _BITTLEET_INFRARED_H_

#include<Arduino.h>
#include "Command.h"


namespace Infrared {

enum Input : uint8_t {
    None = 0,
    Rest,
    Forward,
    GyroOff,
    Left,
    Balance,
    Right,
    Pause,
    Backward,
    Calibrate,
    Step,
    Crawl,
    Walk,
    Trot,
    Sit,
    Stretch,
    Greet,
    Pushup,
    Hydrant,
    Check,
    Dead,
    Zero,
    TOTAL
};

Input translate(uint8_t signal);

Command::Command parser(Input command, const Command::Move& move);

} // namespace Infrared

#endif // _BITTLEET_INFRARED_H_