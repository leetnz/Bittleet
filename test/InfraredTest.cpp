//
// Infrared Tests
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#include "catch.hpp"

#include <vector>

#include "Arduino.h"

#include "Infrared.h"
#include "Command.h"

using namespace Infrared;

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

TEST_CASE("ParseSignal_Simple", "[Infrared]" ) 
{ 
    using Simple = Command::Simple;
    using WithArgs = Command::WithArgs;
    using ArgType = Command::ArgType;

    struct TestCase {
        std::string name;
        uint8_t signal;
        // Command::Move move;
        Command::Command expected;
    };

    const std::vector<TestCase> testCases = {
        { "Rest",           IR_CODE_00, Command::Command(Simple::Rest)},
        { "Gyro Toggle",    IR_CODE_02, Command::Command(Simple::GyroToggle)},

        { "Balance",        IR_CODE_11, Command::Command(Simple::Balance)},

        { "Pause",          IR_CODE_20, Command::Command(Simple::Pause)},
        { "Calibrate",      IR_CODE_22, Command::Command(WithArgs{ArgType::Calibrate, 0, {}})},

        { "Step",           IR_CODE_30, Command::Command(Simple::Step)},

        { "Sit",            IR_CODE_41, Command::Command(Simple::Sit)},
        { "Stretch",        IR_CODE_42, Command::Command(Simple::Stretch)},

        { "Greet",          IR_CODE_50, Command::Command(Simple::Greet)},
        { "Pushup",         IR_CODE_51, Command::Command(Simple::Pushup)},
        { "Hydrant",        IR_CODE_52, Command::Command(Simple::Hydrant)},

        { "Check",          IR_CODE_60, Command::Command(Simple::Check)},
        { "Dead",           IR_CODE_61, Command::Command(Simple::Dead)},
        { "Zero",           IR_CODE_62, Command::Command(Simple::Zero)},
    };

    Command::Move move = Command::Move{Command::Pace::Medium, Command::Direction::Forward};

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            REQUIRE(tc.expected == parseSignal(tc.signal, move));
        }
    }
}

TEST_CASE("ParseSignal_Move", "[Infrared]" ) 
{ 
    using Move = Command::Move;
    using Pace = Command::Pace;
    using Direction = Command::Direction;

    struct TestCase {
        std::string name;
        Move move;
    };

    const std::vector<TestCase> testCases = {
        { "FF", Move{Pace::Fast, Direction::Forward} },
        { "RL", Move{Pace::Reverse, Direction::Left} },
        { "SR", Move{Pace::Slow, Direction::Right} },
    };

    // Direction
    {
        struct Setup {
            std::string name;
            uint8_t signal;
            Direction dir;
        };

        const std::vector<Setup> setups = {
            { "Forward", IR_CODE_01, Direction::Forward },
            { "Left",    IR_CODE_10, Direction::Left    },
            { "Right",   IR_CODE_12, Direction::Right   },
        };

        for (auto& setup : setups) {
            for (auto& tc : testCases) {
                SECTION("Dir: " + setup.name + tc.name) {
                    const Command::Command expected = Command::Command(setup.dir, tc.move);

                    REQUIRE(expected == parseSignal(setup.signal, tc.move));
                }
            }
        }
    }

    // Pace
    {
        struct Setup {
            std::string name;
            uint8_t signal;
            Pace pace;
        };

        const std::vector<Setup> setups = {
            { "Reverse",    IR_CODE_21, Pace::Reverse },
            { "Slow",       IR_CODE_31, Pace::Slow },
            { "Medium",     IR_CODE_32, Pace::Medium },
            { "Fast",       IR_CODE_40, Pace::Fast },
        };

        for (auto& setup : setups) {
            for (auto& tc : testCases) {
                SECTION("Pace: " + setup.name + tc.name) {
                    const Command::Command expected = Command::Command(setup.pace, tc.move);

                    REQUIRE(expected == parseSignal(setup.signal, tc.move));  
                }
            }
        }
    } 
}
