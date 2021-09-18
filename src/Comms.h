#ifndef _BITTLEET_COMMS_H_
#define _BITTLEET_COMMS_H_

#include <Arduino.h>
#include <stdint.h>
#include "Command.h"

namespace Comms {

class SerialComms {
    public:
        SerialComms() = default;

        Command::Command parse(const Command::Move& lastMove, const int16_t* currentAngles);

    private:
        enum class State : uint8_t {
            None,
            Skill
        };
        State _state = State::None;
        char _argStr[64]  = {'\0'};
};

}

#endif //_BITTLEET_COMMS_H_