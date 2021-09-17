#include "Infrared.h"

namespace Infrared {

#define IR_CODE_00 (0xA2)   //rest and shutdown all servos 
#define IR_CODE_01 (0x62)   //forward
#define IR_CODE_02 (0xE2)   //turn off gyro feedback to boost speed
#define IR_CODE_10 (0x22)   //left
#define IR_CODE_11 (0x02)   //neutral stand up posture
#define IR_CODE_12 (0xC2)   //right
#define IR_CODE_20 (0xE0)   //pause motion and shut off all servos 
#define IR_CODE_21 (0xA8)   //backward
#define IR_CODE_22 (0x90)   //calibration mode with IMU turned off
#define IR_CODE_30 (0x68)   //stepping
#define IR_CODE_31 (0x98)   //crawl
#define IR_CODE_32 (0xB0)   //walk
#define IR_CODE_40 (0x30)   //trot
#define IR_CODE_41 (0x18)   //sit
#define IR_CODE_42 (0x7A)   //stretch
#define IR_CODE_50 (0x10)   //greeting
#define IR_CODE_51 (0x38)   //push up
#define IR_CODE_52 (0x5A)   //standng with three legs
#define IR_CODE_60 (0x42)   //check around
#define IR_CODE_61 (0x4A)   //play dead
#define IR_CODE_62 (0x52)   //zero position

Command::Command parseSignal(uint8_t signal, const Command::Move& move) {
    using namespace Command;

    switch (signal) {
        case IR_CODE_00:    return Command::Command(Simple::Rest);    
        case IR_CODE_01:    return Command::Command(Move{Pace::Medium, Direction::Forward});
        case IR_CODE_02:    return Command::Command(Simple::GyroToggle);  
        case IR_CODE_10:    return Command::Command(Move{move.pace, Direction::Left});
        case IR_CODE_11:    return Command::Command(Simple::Balance);
        case IR_CODE_12:    return Command::Command(Move{move.pace, Direction::Right});
        case IR_CODE_20:    return Command::Command(Simple::Pause);
        case IR_CODE_21:    return Command::Command(Move{Pace::Reverse, move.direction});
        case IR_CODE_22:    return Command::Command(WithArgs{ArgType::Calibrate, 0, {}});
        case IR_CODE_30:    return Command::Command(Simple::Step);
        case IR_CODE_31:    return Command::Command(Move{Pace::Slow, move.direction});
        case IR_CODE_32:    return Command::Command(Move{Pace::Medium, move.direction});
        case IR_CODE_40:    return Command::Command(Move{Pace::Fast, move.direction});
        case IR_CODE_41:    return Command::Command(Simple::Sit);
        case IR_CODE_42:    return Command::Command(Simple::Stretch);
        case IR_CODE_50:    return Command::Command(Simple::Greet);
        case IR_CODE_51:    return Command::Command(Simple::Pushup);
        case IR_CODE_52:    return Command::Command(Simple::Hydrant);
        case IR_CODE_60:    return Command::Command(Simple::Check);
        case IR_CODE_61:    return Command::Command(Simple::Dead);
        case IR_CODE_62:    return Command::Command(Simple::Zero);
        default:            return Command::Command();
    }
}


} // namespace Infrared
