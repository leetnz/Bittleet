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
#define T_SKILL     'k'
#define T_LISTED    'l'
#define T_MOVE      'm'
#define T_SIMULTANEOUS_MOVE 'M'
#define T_MELODY    'o'
#define T_PAUSE     'p'
#define T_RAMP      'r'
#define T_SAVE      's'
#define T_MEOW      'u'
#define T_UNDEFINED 'w'
#define T_XLEG      'x'

namespace Comms {

Command::Command parseSerial(Stream& serial, const Command::Move& move,const int16_t* currentAngles ) {
    while (Serial.available() > 0) {
        uint8_t token = Serial.read();
        // this block handles argumentless tokens
        switch (token) {
            case T_REST: {
                return Command::Command(Command::Simple::Rest);
            }
            case T_GYRO: {
                return Command::Command(Command::Simple::GyroToggle);
            }
            case T_PAUSE: {
                return Command::Command(Command::Simple::Pause);
            }
            case T_SAVE: {
                return Command::Command(Command::Simple::SaveServoCalibration);
            }
            case T_ABORT: {
                return Command::Command(Command::Simple::AbortServoCalibration);
            }
            case T_JOINTS: { //show the list of current joint anles
                return Command::Command(Command::Simple::ShowJointAngles);
            }
            case T_CALIBRATE: //calibration
            case T_MOVE: //move multiple indexed joints to angles once at a time (ASCII format entered in the serial monitor)
            case T_SIMULTANEOUS_MOVE: //move multiple indexed joints to angles simultaneously (ASCII format entered in the serial monitor)
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
                    cmd.len = DOF;
                    for (int i = 0; i < DOF; i += 1) {
                        cmd.args[i] = currentAngles[i];
                    }
                }
                String inBuffer = Serial.readStringUntil('\n');
                char temp[64]  = {'\0'};
                strcpy(temp, inBuffer.c_str());
                char *pch;
                pch = strtok(temp, " ,");
                do {//it supports combining multiple commands at one time
                    //for example: "m8 40 m8 -35 m 0 50" can be written as "m8 40 8 -35 0 50"
                    //the combined commands should be less than four. string len <=30 to be exact.
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
                } while (pch != NULL);
                return Command::Command(cmd);
            }


            default: { break; } // Try again.
        }
    }
}
} // namespace Comms