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


//abbreviation //gait/posture/function names
#define K00 "d"       //rest and shutdown all servos 
#define K01 "F"       //forward
#define K02 "g"       //turn off gyro feedback to boost speed

#define K10 "L"       //left
#define K11 "balance" //neutral stand up posture
#define K12 "R"       //right

#define K20 "p"       //pause motion and shut off all servos 
#define K21 "B"       //backward
#define K22 "c"       //calibration mode with IMU turned off

#define K30 "vt"      //stepping
#define K31 "cr"      //crawl
#define K32 "wk"      //walk

#define K40 "tr"      //trot
#define K41 "sit"     //sit
#define K42 "str"     //stretch

#define K50 "hi"      //greeting
#define K51 "pu"      //push up
#define K52 "pee"     //standng with three legs

#define K60 "ck"      //check around
#define K61 "pd"      //play dead
#define K62 "zero"    //zero position

String translate(uint8_t signal)
{
    switch (signal) {
        case IR_CODE_00: return (F(K00));
        case IR_CODE_01: return (F(K01));
        case IR_CODE_02: return (F(K02));
        case IR_CODE_10: return (F(K10));
        case IR_CODE_11: return (F(K11));
        case IR_CODE_12: return (F(K12));
        case IR_CODE_20: return (F(K20));
        case IR_CODE_21: return (F(K21));
        case IR_CODE_22: return (F(K22));
        case IR_CODE_30: return (F(K30));
        case IR_CODE_31: return (F(K31));
        case IR_CODE_32: return (F(K32));
        case IR_CODE_40: return (F(K40));
        case IR_CODE_41: return (F(K41));
        case IR_CODE_42: return (F(K42));
        case IR_CODE_50: return (F(K50));
        case IR_CODE_51: return (F(K51));
        case IR_CODE_52: return (F(K52));
        case IR_CODE_60: return (F(K60));
        case IR_CODE_61: return (F(K61));
        case IR_CODE_62: return (F(K62));
        default: return         ("");
    }
}

} // namespace Infrared
