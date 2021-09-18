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
#define T_SIT       't'
#define T_STRETCH   'T'
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
    while (Serial.available() > 0) {
        switch (_state) {
            case (State::None): {
                uint8_t token = Serial.read();
                switch (token) {
                    case T_PAUSE:       return Command::Command(Command::Simple::Pause);
                    case T_GYRO:        return Command::Command(Command::Simple::GyroToggle);
                    case T_REST:        return Command::Command(Command::Simple::Rest);
                    case T_SIT:         return Command::Command(Command::Simple::Sit);
                    case T_STRETCH:     return Command::Command(Command::Simple::Stretch);
                    // Calibration Commands
                    case T_SAVE:        return Command::Command(Command::Simple::SaveServoCalibration);
                    case T_ABORT:       return Command::Command(Command::Simple::AbortServoCalibration);
                    // Diagnostic Commands
                    case T_JOINTS:      return Command::Command(Command::Simple::ShowJointAngles);
                    case T_HELP:        return Command::Command(Command::Simple::ShowHelp);
                    // Commands with arguments
                    case T_CALIBRATE: //calibration
                    case T_MOVE: //move multiple indexed joints to angles once at a time
                    case T_SIMULTANEOUS_MOVE: //move multiple indexed joints to angles simultaneously
                    case T_MEOW: //meow (repeat, increament)
                    case T_BEEP: //beep(tone, duration): tone 0 is pause, duration range is 0~255
                    {
                        Command::WithArgs cmd = {};
                        cmd.len = 0;
                        switch (token) {
                        case (T_CALIBRATE):         cmd.cmd = Command::ArgType::Calibrate; break;
                        case (T_MOVE):              cmd.cmd = Command::ArgType::MoveSequentially; break;
                        case (T_SIMULTANEOUS_MOVE): cmd.cmd = Command::ArgType::MoveSimultaneously; break;
                        case (T_MEOW):              cmd.cmd = Command::ArgType::Meow; break;
                        case (T_BEEP):              cmd.cmd = Command::ArgType::Beep; break;
                        }

                        if (token == T_SIMULTANEOUS_MOVE) {
                            for (int i = 0; i < DOF; i += 1) {
                                cmd.args[i] = currentAngles[i];
                            }
                        }
                        String inBuffer = Serial.readStringUntil('\n');
                        if (inBuffer.length() > 64) {
                            return Command::Command(); // Too many bytes!
                        }
                        char temp[64]  = {'\0'};
                        strcpy(temp, inBuffer.c_str());
                        char *pch;
                        pch = strtok(temp, " ,");
                        while (pch != NULL) {
                            if (cmd.len >= COMMAND_MAX_ARGS) {
                                return Command::Command(); // Too many arguments!
                            }
                            int target[2] = {};
                            byte inLen = 0;
                            for (byte b = 0; b < 2 && pch != NULL; b++) {
                                target[b] = atoi(pch);
                                pch = strtok(NULL, " ,\t");
                                inLen++;
                            }
                            if (inLen != 2) {
                                break; // Args are expected to arrive in pairs.
                            }
                            if (token == T_SIMULTANEOUS_MOVE) {
                                cmd.args[target[0]] = (int8_t)target[1];
                            } else {
                                cmd.args[cmd.len++] = (int8_t)target[0]; 
                                cmd.args[cmd.len++] = (int8_t)target[1]; 
                            }
                        };
                        if (token == T_SIMULTANEOUS_MOVE) {
                            cmd.len = DOF;
                        }
                        return Command::Command(cmd);
                    }
                    case T_SKILL: {
                        _state = State::Skill;
                        break;
                    }

                    default: { break; } // Try again.
                }
                break;
            }
            case (State::Skill): {
                _state = State::None; // Will return to None regardless of result.
                uint8_t skill = Serial.read();
                switch (skill) {  
                    case S_FORWARD:     return Command::Command(Command::Direction::Forward, lastMove);
                    case S_LEFT:        return Command::Command(Command::Direction::Left, lastMove);
                    case S_RIGHT:       return Command::Command(Command::Direction::Right, lastMove);
                    case S_BACKWARD:    return Command::Command(Command::Pace::Reverse, lastMove);
                    case S_BALANCE:     return Command::Command(Command::Simple::Balance);
                    case S_STEP:        return Command::Command(Command::Simple::Step);
                    case S_CRAWL:       return Command::Command(Command::Pace::Slow, lastMove);
                    case S_WALK:        return Command::Command(Command::Pace::Medium, lastMove);
                    case S_TROT:        return Command::Command(Command::Pace::Fast, lastMove);
                    case S_SIT:         return Command::Command(Command::Simple::Sit);
                    case S_STRETCH:     return Command::Command(Command::Simple::Stretch);
                    case S_GREET:       return Command::Command(Command::Simple::Greet);
                    case S_PUSHUP:      return Command::Command(Command::Simple::Pushup);
                    case S_HYDRANT:     return Command::Command(Command::Simple::Hydrant);
                    case S_CHECK:       return Command::Command(Command::Simple::Check);
                    case S_DEAD:        return Command::Command(Command::Simple::Dead);
                    case S_ZERO:        return Command::Command(Command::Simple::Zero);
                    default:            break;
                };
                break;
            }
        }
    }
    return Command::Command();
}

} // namespace Comms