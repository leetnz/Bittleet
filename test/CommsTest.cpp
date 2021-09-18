//
// Comms Tests
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#include "catch.hpp"

#include <vector>

#include "Arduino.h"

#include "Comms.h"
#include "Command.h"
#include "Bittle.h"

using namespace Comms;

using Simple = Command::Simple;
using Move = Command::Move;
using Pace = Command::Pace;
using Direction = Command::Direction;
using WithArgs = Command::WithArgs;
using ArgType = Command::ArgType;

TEST_CASE("ParseSerial_Simple", "[Comms]" ) 
{ 
    struct TestCase {
        std::string name;
        std::string bytes;
        Command::Command expected;
    };

    const std::vector<TestCase> testCases = {
        { "Rest",               "d", Command::Command(Simple::Rest)},
        { "Gyro",               "g", Command::Command(Simple::GyroToggle)},
        { "Pause",              "p", Command::Command(Simple::Pause)},

        { "Abort Calibration",  "a", Command::Command(Simple::AbortServoCalibration)},
        { "Save Calibration",   "s", Command::Command(Simple::SaveServoCalibration)},

        { "Show Joint Angles",  "j", Command::Command(Command::Simple::ShowJointAngles)},
        { "Show Help",          "h", Command::Command(Command::Simple::ShowHelp)},
    };

    Move move = Move{Pace::Medium, Direction::Forward};
    int16_t currentPos[DOF] = {};

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            SerialComms comms{};
            Serial = Stream(tc.bytes);
            REQUIRE(tc.expected == comms.parse(move, currentPos));
        }
    }
}



TEST_CASE("ParseSerial_Simple_Skills", "[Comms]" ) 
{ 
    struct TestCase {
        std::string name;
        std::string bytes;
        Command::Command expected;
    };

    const std::vector<TestCase> testCases = {     
        { "BALANCE",    "kb", Command::Command(Simple::Balance)},      
        { "STEP",       "kv", Command::Command(Simple::Step)},             
        { "SIT",        "ks", Command::Command(Simple::Sit)},       
        { "STRETCH",    "kT", Command::Command(Simple::Stretch)},       
        { "GREET",      "kh", Command::Command(Simple::Greet)},       
        { "PUSHUP",     "kp", Command::Command(Simple::Pushup)},
        { "HYDRANT",    "ke", Command::Command(Simple::Hydrant)},
        { "CHECK",      "kk", Command::Command(Simple::Check)},
        { "DEAD",       "kd", Command::Command(Simple::Dead)},
        { "ZERO",       "kz", Command::Command(Simple::Zero)},
    };

    Move move = Move{Pace::Medium, Direction::Forward};
    int16_t currentPos[DOF] = {};

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            SerialComms comms{};
            Serial = Stream(tc.bytes);
            REQUIRE(tc.expected == comms.parse(move, currentPos));
        }
        SECTION("Split: " + tc.name) {
            SerialComms comms{};
            Serial = Stream(tc.bytes.substr(0, 1));
            REQUIRE(Command::Command() == comms.parse(move, currentPos));

            Serial = Stream(tc.bytes.substr(1));
            REQUIRE(tc.expected == comms.parse(move, currentPos));
        }
    }
}

TEST_CASE("ParseSerial_Move", "[Comms]" ) 
{ 
    struct TestCase {
        std::string name;
        Move move;
    };

    const std::vector<TestCase> testCases = {
        { "FF", Move{Pace::Fast, Direction::Forward} },
        { "RL", Move{Pace::Reverse, Direction::Left} },
        { "SR", Move{Pace::Slow, Direction::Right} },
    };

    int16_t currentPos[DOF] = {};

    // Direction
    {
        struct Setup {
            std::string name;
            std::string bytes;
            Direction dir;
        }; 

        const std::vector<Setup> setups = {
            { "Forward", "kF", Direction::Forward },
            { "Left",    "kL", Direction::Left    },
            { "Right",   "kR", Direction::Right   },
        };

        for (auto& setup : setups) {
            for (auto& tc : testCases) {
                SECTION("Dir: " + setup.name + tc.name) {
                    const Command::Command expected = Command::Command(setup.dir, tc.move);

                    SerialComms comms{};
                    Serial = Stream(setup.bytes);
                    REQUIRE(expected == comms.parse(tc.move, currentPos));
                }
                SECTION("Dir Split: " + setup.name + tc.name) {
                    const Command::Command expected = Command::Command(setup.dir, tc.move);

                    SerialComms comms{};
                    Serial = Stream(setup.bytes.substr(0, 1));
                    REQUIRE(Command::Command() == comms.parse(tc.move, currentPos));

                    Serial = Stream(setup.bytes.substr(1));
                    REQUIRE(expected == comms.parse(tc.move, currentPos));
                }
            }
        }
    }

    // Pace
    {
        struct Setup {
            std::string name;
            std::string bytes;
            Pace pace;
        };      

        const std::vector<Setup> setups = {
            { "Reverse",    "kB", Pace::Reverse },
            { "Slow",       "kc", Pace::Slow },
            { "Medium",     "kw", Pace::Medium },
            { "Fast",       "kt", Pace::Fast },
        };

        for (auto& setup : setups) {
            for (auto& tc : testCases) {
                SECTION("Pace: " + setup.name + tc.name) {
                    const Command::Command expected = Command::Command(setup.pace, tc.move);

                    SerialComms comms{};
                    Serial = Stream(setup.bytes);
                    REQUIRE(expected == comms.parse(tc.move, currentPos)); 
                }
                SECTION("Pace Split: " + setup.name + tc.name) {
                    const Command::Command expected = Command::Command(setup.pace, tc.move);

                    SerialComms comms{};
                    Serial = Stream(setup.bytes.substr(0, 1));
                    REQUIRE(Command::Command() == comms.parse(tc.move, currentPos));

                    Serial = Stream(setup.bytes.substr(1));
                    REQUIRE(expected == comms.parse(tc.move, currentPos));
                }
            }
        }
    } 
}

TEST_CASE("ParseSerial_WithArgsSimple", "[Comms]" ) 
{
    struct Setup {
        std::string name;
        std::string token;
        ArgType argType;
    };      

    const std::vector<Setup> setups = {
        { "Calibrate",  "c", ArgType::Calibrate },
        { "Move",       "m", ArgType::MoveSequentially },
        { "Meow",       "u", ArgType::Meow },
        { "Beep",       "b", ArgType::Beep },
    };

    for (auto& setup : setups) {
        struct TestCase {
            std::string name;
            std::string bytes;
            Command::Command expected;
        };

        const std::vector<TestCase> testCases = {
            { "{}",                 setup.token + "\n",                 Command::Command(WithArgs{setup.argType, 0, {}})},
            { "{1,5}",              setup.token + "1 5\n",              Command::Command(WithArgs{setup.argType, 2, {1, 5}})},
            { "{1,-5}",             setup.token + "1 -5\n",             Command::Command(WithArgs{setup.argType, 2, {1, -5}})},
            { "{1,2,3,4,5,6,7,8}",  setup.token + "1,2,3,4,5,6,7,8\n",  Command::Command(WithArgs{setup.argType, 8, {1,2,3,4,5,6,7,8}})},
            { "Expected in pairs",  setup.token + "1,2,3,4,5,6,7\n",    Command::Command(WithArgs{setup.argType, 6, {1,2,3,4,5,6}})},
            { 
                "Exactly 16 Arguments",  
                setup.token + "1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16\n",  
                Command::Command(WithArgs{setup.argType, 16, {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}})
            },
            { 
                "Exactly 63 bytes",   
                setup.token + "03   ,09   ,15   ,21   ,27   ,33   ,39   ,45,48,51,54,57,61,63\n", 
                Command::Command(WithArgs{setup.argType, 14, {3, 9, 15, 21, 27, 33, 39, 45, 48, 51, 54, 57, 61, 63}})
            },
            { 
                "More than 16 arguments", 
                setup.token + "1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17\n",  
                Command::Command()
            },
            { 
                "More than 63 bytes", 
                setup.token + "03   ,09   ,15   ,21   ,27   ,33,36,39,42,45,48,51,54,57, 61, 64\n",  
                Command::Command()
            },
        };

        const Move move = Move{Pace::Medium, Direction::Forward};
        const int16_t currentPos[DOF] = {};

        for (auto& tc : testCases) {
            SECTION(setup.name + " " + tc.name) {
                SerialComms comms{};
                Serial = Stream(tc.bytes);
                REQUIRE(tc.expected == comms.parse(move, currentPos));
            }
            SECTION(setup.name + " incremental " + tc.name) {
                SerialComms comms{};
                for (int i = 0; i < (int)tc.bytes.length() - 1; i++) {
                    Serial = Stream(tc.bytes.substr(i, 1));
                    REQUIRE(Command::Command() == comms.parse(move, currentPos));
                }
                Serial = Stream(tc.bytes.substr(tc.bytes.length() - 1));
                REQUIRE(tc.expected == comms.parse(move, currentPos));
            }
        }
    }
}

TEST_CASE("ParseSerial_WithArgs_MoveSimultaneously", "[Comms]" ) 
{
    struct TestCase {
        std::string name;
        std::string bytes;
        Command::Command expected;
    };

    const int16_t currentPos[DOF] = {-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15,-16};
    const ArgType type = ArgType::MoveSimultaneously;
    const std::vector<TestCase> testCases = {
        { "{}",                 "M\n",                 Command::Command(WithArgs{type, DOF, {-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15,-16}})},
        { "{1,5}",              "M1 5\n",              Command::Command(WithArgs{type, DOF, {-1, 5,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15,-16}})},
        { "{1,-5}",             "M1 -5\n",             Command::Command(WithArgs{type, DOF, {-1,-5,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15,-16}})},
        { "{0,1,1,2,2,3,3,4}",  "M0,1,1,2,2,3,3,4\n",  Command::Command(WithArgs{type, DOF, { 1, 2, 3, 4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15,-16}})},
        { "Expected in pairs",  "M0,1,1,2,2,3,3\n",    Command::Command(WithArgs{type, DOF, { 1, 2, 3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15,-16}})},
        { 
            "Exactly 16 Arguments",  
            "M4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4\n",  
            Command::Command(WithArgs{type, DOF, {1,2,3,4,5,6,7,8,-9,-10,-11,-12,-13,-14,-15,-16}})
        },
        { 
            "Exactly 63 bytes",   
            "M 0    , 1   , 1   , 2   ,2    , 3   ,3    , 4,4 ,5 ,5 , 6,6 ,7 \n", 
            Command::Command(WithArgs{type, DOF, { 1, 2, 3, 4, 5, 6, 7,-8,-9,-10,-11,-12,-13,-14,-15,-16}})
        },
        { 
            "Negative Index", 
            "M -1, 0\n",  
            Command::Command()
        },
        { 
            "Invalid Index", 
            "M 17, 0\n",  
            Command::Command()
        },
        { 
            "More than 63 bytes", 
            "M 0    , 1   , 1   , 2   ,2    , 3   ,3    , 4,4 ,5 ,5 , 6,6,7,7,8\n",  
            Command::Command()
        },
    };

    const Move move = Move{Pace::Medium, Direction::Forward};
    

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            SerialComms comms{};
            Serial = Stream(tc.bytes);
            REQUIRE(tc.expected == comms.parse(move, currentPos));
        }
        SECTION("incremental " + tc.name) {
            SerialComms comms{};
            for (int i = 0; i < (int)tc.bytes.length() - 1; i++) {
                Serial = Stream(tc.bytes.substr(i, 1));
                REQUIRE(Command::Command() == comms.parse(move, currentPos));
            }
            Serial = Stream(tc.bytes.substr(tc.bytes.length() - 1));
            REQUIRE(tc.expected == comms.parse(move, currentPos));
        }
    }
}


