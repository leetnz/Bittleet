#include "Infrared.h"

namespace Infrared {

#define IR_CODE_00 (0xA2)
#define IR_CODE_01 (0x62)
#define IR_CODE_02 (0xE2)
#define IR_CODE_10 (0x22)
#define IR_CODE_11 (0x02)
#define IR_CODE_12 (0xC2)
#define IR_CODE_20 (0xE0)
#define IR_CODE_21 (0xA8)
#define IR_CODE_22 (0x90)
#define IR_CODE_30 (0x68)
#define IR_CODE_31 (0x98)
#define IR_CODE_32 (0xB0)
#define IR_CODE_40 (0x30)
#define IR_CODE_41 (0x18)
#define IR_CODE_42 (0x7A)
#define IR_CODE_50 (0x10)
#define IR_CODE_51 (0x38)
#define IR_CODE_52 (0x5A)
#define IR_CODE_60 (0x42)
#define IR_CODE_61 (0x4A)
#define IR_CODE_62 (0x52)

#define K00 Input::Rest       //rest and shutdown all servos 
#define K01 Input::Forward    //forward
#define K02 Input::GyroToggle    //turn off gyro feedback to boost speed

#define K10 Input::Left       //left
#define K11 Input::Balance    //neutral stand up posture
#define K12 Input::Right      //right

#define K20 Input::Pause      //pause motion and shut off all servos 
#define K21 Input::Backward   //backward
#define K22 Input::Calibrate  //calibration mode with IMU turned off

#define K30 Input::Step       //stepping
#define K31 Input::Crawl      //crawl
#define K32 Input::Walk       //walk

#define K40 Input::Trot       //trot
#define K41 Input::Sit        //sit
#define K42 Input::Stretch    //stretch

#define K50 Input::Greet      //greeting
#define K51 Input::Pushup     //push up
#define K52 Input::Hydrant    //standng with three legs

#define K60 Input::Check      //check around
#define K61 Input::Dead       //play dead
#define K62 Input::Zero       //zero position

Input translate(uint8_t signal)
{
    switch (signal) {
        case IR_CODE_00: return K00;
        case IR_CODE_01: return K01;
        case IR_CODE_02: return K02;
        case IR_CODE_10: return K10;
        case IR_CODE_11: return K11;
        case IR_CODE_12: return K12;
        case IR_CODE_20: return K20;
        case IR_CODE_21: return K21;
        case IR_CODE_22: return K22;
        case IR_CODE_30: return K30;
        case IR_CODE_31: return K31;
        case IR_CODE_32: return K32;
        case IR_CODE_40: return K40;
        case IR_CODE_41: return K41;
        case IR_CODE_42: return K42;
        case IR_CODE_50: return K50;
        case IR_CODE_51: return K51;
        case IR_CODE_52: return K52;
        case IR_CODE_60: return K60;
        case IR_CODE_61: return K61;
        case IR_CODE_62: return K62;
        default: return Input::None;
    }
}

Command::Command parser(Input in, const Command::Move& current) {
    using namespace Command;
    switch (in) {
        case Input::Backward:   return Command::Command(Move{Pace::Reverse, current.direction});
        case Input::Forward:    return Command::Command(Move{Pace::Medium, Direction::Forward});
        case Input::Left:       return Command::Command(Move{current.pace, Direction::Left});
        case Input::Right:      return Command::Command(Move{current.pace, Direction::Right});
        case Input::Crawl:      return Command::Command(Move{Pace::Slow, current.direction});
        case Input::Walk:       return Command::Command(Move{Pace::Medium, current.direction});
        case Input::Trot:       return Command::Command(Move{Pace::Fast, current.direction});
        case Input::Rest:       return Command::Command(Simple::Rest);
        case Input::GyroToggle:    return Command::Command(Simple::GyroToggle);  
        case Input::Balance:    return Command::Command(Simple::Balance);
        case Input::Pause:      return Command::Command(Simple::Pause);
        case Input::Calibrate:  return Command::Command(WithArgs{ArgType::Calibrate, 0, {}});
        case Input::Step:       return Command::Command(Simple::Step);
        case Input::Sit:        return Command::Command(Simple::Sit);
        case Input::Stretch:    return Command::Command(Simple::Stretch);
        case Input::Greet:      return Command::Command(Simple::Greet);
        case Input::Pushup:     return Command::Command(Simple::Pushup);
        case Input::Hydrant:    return Command::Command(Simple::Hydrant);
        case Input::Check:      return Command::Command(Simple::Check);
        case Input::Dead:       return Command::Command(Simple::Dead);
        case Input::Zero:       return Command::Command(Simple::Zero);
        default:                return Command::Command();
    }
}

} // namespace Infrared
