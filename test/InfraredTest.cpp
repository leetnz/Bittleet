#include "catch.hpp"

#include <map>

#include "Arduino.h"

#include "Infrared.h"

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

const std::map<uint8_t, String> validCases = {
    {IR_CODE_00, K00},  {IR_CODE_01, K01},  {IR_CODE_02, K02}, 
    {IR_CODE_10, K10},  {IR_CODE_11, K11},  {IR_CODE_12, K12}, 
    {IR_CODE_20, K20},  {IR_CODE_21, K21},  {IR_CODE_22, K22}, 
    {IR_CODE_30, K30},  {IR_CODE_31, K31},  {IR_CODE_32, K32}, 
    {IR_CODE_40, K40},  {IR_CODE_41, K41},  {IR_CODE_42, K42}, 
    {IR_CODE_50, K50},  {IR_CODE_51, K51},  {IR_CODE_52, K52}, 
    {IR_CODE_60, K60},  {IR_CODE_61, K61},  {IR_CODE_62, K62}, 
};

TEST_CASE("InfraredTest", "[infrared]" ) 
{ 
    SECTION("Valid Cases") {
        for (std::map<uint8_t, String>::const_reference tc : validCases) {
            REQUIRE(tc.second == Infrared::translate(tc.first));
        }
    }
    
    SECTION("Invalid returns empty") {
        REQUIRE("" == Infrared::translate(0x00));
    }
}