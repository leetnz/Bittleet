//
// Bittleet Comms
// Convert Serial Data into Bittle Commands
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//


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

        bool _parseSingle(uint8_t byte, Command::Command& result);
        bool _parseSkill(uint8_t byte, const Command::Move& lastMove, Command::Command& result);
        bool _parseWithArgs(uint8_t byte, const int16_t* currentAngles, Command::Command& result);

        void _toArgs(Command::ArgType argType);
        bool _extractArgsFromString(Command::WithArgs& cmd);
};

}

#endif //_BITTLEET_COMMS_H_