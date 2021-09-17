#include "catch.hpp"

#include <map>

#include "Arduino.h"

#include "Infrared.h"

using namespace Infrared;

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
#define K00 Input::Rest         //rest and shutdown all servos 
#define K01 Input::Forward      //forward
#define K02 Input::GyroToggle   //turn off gyro feedback to boost speed
#define K10 Input::Left         //left
#define K11 Input::Balance      //neutral stand up posture
#define K12 Input::Right        //right
#define K20 Input::Pause        //pause motion and shut off all servos 
#define K21 Input::Backward     //backward
#define K22 Input::Calibrate    //calibration mode with IMU turned off
#define K30 Input::Step         //stepping
#define K31 Input::Crawl        //crawl
#define K32 Input::Walk         //walk
#define K40 Input::Trot         //trot
#define K41 Input::Sit          //sit
#define K42 Input::Stretch      //stretch
#define K50 Input::Greet        //greeting
#define K51 Input::Pushup       //push up
#define K52 Input::Hydrant      //standng with three legs
#define K60 Input::Check        //check around
#define K61 Input::Dead         //play dead
#define K62 Input::Zero         //zero position

const std::map<uint8_t, Input> validCases = {
    {IR_CODE_00, K00},  {IR_CODE_01, K01},  {IR_CODE_02, K02}, 
    {IR_CODE_10, K10},  {IR_CODE_11, K11},  {IR_CODE_12, K12}, 
    {IR_CODE_20, K20},  {IR_CODE_21, K21},  {IR_CODE_22, K22}, 
    {IR_CODE_30, K30},  {IR_CODE_31, K31},  {IR_CODE_32, K32}, 
    {IR_CODE_40, K40},  {IR_CODE_41, K41},  {IR_CODE_42, K42}, 
    {IR_CODE_50, K50},  {IR_CODE_51, K51},  {IR_CODE_52, K52}, 
    {IR_CODE_60, K60},  {IR_CODE_61, K61},  {IR_CODE_62, K62}, 
};

TEST_CASE("InfraredTest", "[translate]" ) 
{ 
    SECTION("Valid Cases") {
        for (std::map<uint8_t, Input>::const_reference tc : validCases) {
            REQUIRE(tc.second == translate(tc.first));
        }
    }
    
    SECTION("Invalid returns None") {
        REQUIRE(Input::None == translate(0x00));
    }
}