#ifndef _BITTLEET_COMMS_H_
#define _BITTLEET_COMMS_H_

#include <Arduino.h>
#include <stdint.h>
#include "Command.h"

#define MAX_STRING_LENGTH (63)

namespace Comms {

class SerialComms {
    public:
        SerialComms() = default;

        Command::Command parse(const Command::Move& lastMove, const int16_t* currentAngles);

    private:
        enum class State : uint8_t {
            None,
            Skill,
            Args,
        };
        State _state = State::None;
        Command::ArgType _argType;
        char _argStr[MAX_STRING_LENGTH + 1]  = {'\0'}; // Note, +1 so we always have a terminating character '\0'
        uint8_t _argStrLen = 0;
};

}

#endif //_BITTLEET_COMMS_H_