//
// Bittleet Comms
// Convert Serial Data into Bittle Commands
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//


#include "Comms.h"
#include "Bittle.h"

//token list
#define T_ABORT     'a'
#define T_BEEP      'b'
#define T_CALIBRATE 'c'
#define T_REST      'd'
#define T_GYRO      'g'
#define T_HELP      'h'
#define T_INDEXED   'i'
#define T_JOINTS    'j'
#define T_LISTED    'l'
#define T_MOVE      'm'
#define T_SIMULTANEOUS_MOVE 'M'
#define T_MELODY    'o'
#define T_PAUSE     'p'
#define T_RAMP      'r'
#define T_SAVE      's'
#define T_SKILL     'k'
#define T_MEOW      'u'
#define T_UNDEFINED 'w'
#define T_XLEG      'x'

#define S_FORWARD     'F'       //forward
#define S_LEFT        'L'       //left
#define S_RIGHT       'R'       //right
#define S_BACKWARD    'B'       //backward
#define S_BALANCE     'b'       //neutral stand up posture
#define S_STEP        'v'       //stepping
#define S_CRAWL       'c'       //crawl
#define S_WALK        'w'       //walk
#define S_TROT        't'       //trot
#define S_SIT         's'       //sit
#define S_STRETCH     'T'       //stretch
#define S_GREET       'h'       //greeting
#define S_PUSHUP      'p'       //push up
#define S_HYDRANT     'e'       //standng with three legs
#define S_CHECK       'k'       //check around
#define S_DEAD        'd'       //play dead
#define S_ZERO        'z'       //zero position


namespace Comms {

Command::Command SerialComms::parse(const Command::Move& lastMove, const int16_t* currentAngles) {
    Command::Command result;
    while (Serial.available() > 0) {
        uint8_t byte = Serial.read();
        switch (_state) {
            case (State::None): {
                if (_parseSingle(byte, result)) {
                    return result;
                }
                break;
            }
            case (State::Skill): {
                if (_parseSkill(byte, lastMove, result)) {
                    return result;
                }
                break;
            }
            case (State::Args): {
                if (_parseWithArgs(byte, currentAngles, result)) {
                    return result;
                }
                break;
            }
        }
    }
    return Command::Command();
}

// Private Helpers

bool SerialComms::_parseSingle(uint8_t byte, Command::Command& result) {
    switch (byte) {
        case T_PAUSE:       result = Command::Command(Command::Simple::Pause); return true;
        case T_GYRO:        result = Command::Command(Command::Simple::GyroToggle); return true;
        case T_REST:        result = Command::Command(Command::Simple::Rest); return true;
        // Calibration Commands
        case T_SAVE:        result = Command::Command(Command::Simple::SaveServoCalibration); return true;
        case T_ABORT:       result = Command::Command(Command::Simple::AbortServoCalibration); return true;
        // Diagnostic Commands
        case T_JOINTS:      result = Command::Command(Command::Simple::ShowJointAngles); return true;
        case T_HELP:        result = Command::Command(Command::Simple::ShowHelp); return true;
        // Commands with arguments
        case T_CALIBRATE:           _toArgs(Command::ArgType::Calibrate); break;
        case T_MOVE:                _toArgs(Command::ArgType::MoveSequentially); break;
        case T_MEOW:                _toArgs(Command::ArgType::Meow); break;
        case T_BEEP:                _toArgs(Command::ArgType::Beep); break;
        case T_SIMULTANEOUS_MOVE:   _toArgs(Command::ArgType::MoveSimultaneously); break;
        // Skill - the next byte will determine which skill
        case T_SKILL:               _state = State::Skill; break;
        default: { break; } // Try again.
    }
    return false;
}

void SerialComms::_toArgs(Command::ArgType argType) {
    _argType = argType;
    _state = State::Args;
    _argStrLen = 0;
}


bool SerialComms::_parseSkill(uint8_t byte, const Command::Move& lastMove, Command::Command& result) {
    _state = State::None; // Will return to None regardless of result.
    switch (byte) {  
        case S_FORWARD:     result = Command::Command(Command::Direction::Forward, lastMove); return true;
        case S_LEFT:        result = Command::Command(Command::Direction::Left, lastMove); return true;
        case S_RIGHT:       result = Command::Command(Command::Direction::Right, lastMove); return true;
        case S_BACKWARD:    result = Command::Command(Command::Pace::Reverse, lastMove); return true;
        case S_BALANCE:     result = Command::Command(Command::Simple::Balance); return true;
        case S_STEP:        result = Command::Command(Command::Simple::Step); return true;
        case S_CRAWL:       result = Command::Command(Command::Pace::Slow, lastMove); return true;
        case S_WALK:        result = Command::Command(Command::Pace::Medium, lastMove); return true;
        case S_TROT:        result = Command::Command(Command::Pace::Fast, lastMove); return true;
        case S_SIT:         result = Command::Command(Command::Simple::Sit); return true;
        case S_STRETCH:     result = Command::Command(Command::Simple::Stretch); return true;
        case S_GREET:       result = Command::Command(Command::Simple::Greet); return true;
        case S_PUSHUP:      result = Command::Command(Command::Simple::Pushup); return true;
        case S_HYDRANT:     result = Command::Command(Command::Simple::Hydrant); return true;
        case S_CHECK:       result = Command::Command(Command::Simple::Check); return true;
        case S_DEAD:        result = Command::Command(Command::Simple::Dead); return true;
        case S_ZERO:        result = Command::Command(Command::Simple::Zero); return true;
        default:            return false;
    }
}

bool SerialComms::_parseWithArgs(uint8_t byte, const int16_t* currentAngles, Command::Command& result) {
    if (byte != '\n') {
        if (_argStrLen < MAX_STRING_LENGTH) {
            _argStr[_argStrLen++] = byte;
        } else {
            _state = State::None; // Too many bytes!
        }
        return false;
    } else {
        Command::WithArgs cmd = {};
        cmd.len = 0;
        if (_argType == Command::ArgType::MoveSimultaneously) {
            for (int i = 0; i < DOF; i += 1) {
                cmd.args[i] = currentAngles[i];
            }
        }

        bool validArgs = _extractArgsFromString(cmd);

        _state = State::None; // Reset
        _argStrLen = 0; 
        
        if (validArgs) {
            if (_argType == Command::ArgType::MoveSimultaneously) {
                cmd.len = DOF;
            }
            cmd.cmd = _argType;
            result = Command::Command(cmd);
        } else {
            result = Command::Command(); // Something went wrong!
        }
        return true;
    }
}

bool SerialComms::_extractArgsFromString(Command::WithArgs& cmd) {
    char *pch;
    pch = strtok(_argStr, " ,");
    while (pch != NULL) {
        if (cmd.len >= COMMAND_MAX_ARGS) {
            return false; // Too many arguments!
        }
        int16_t argPair[2] = {};
        for (int i = 0; i<2; i++) {
            argPair[i] = atoi(pch);
            pch = strtok(NULL, " ,\t");
            if ((i == 0) && (pch == NULL)) {
                return true; // Args are expected to arrive in pairs. Still assume we have a valid arg set.
            }
        }
        if (_argType == Command::ArgType::MoveSimultaneously) {
            const int8_t index = argPair[0];
            const int8_t value = argPair[1];
            if (index < 0 || index >= DOF) {
                return false; // Invalid index
            }
            cmd.args[index] = value;
        } else {
            cmd.args[cmd.len++] = (int8_t)argPair[0]; 
            cmd.args[cmd.len++] = (int8_t)argPair[1]; 
        }
    };
    return true;
}



} // namespace Comms