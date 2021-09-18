#include "catch.hpp"

#include <vector>
#include <sstream>

#include "Arduino.h"

#include "Comms.h"
#include "Command.h"
#include "Bittle.h"

using namespace Comms;

using Simple = Command::Simple;
using Move = Command::Move;
using Pace = Command::Pace;
using Direction = Command::Direction;

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

    Command::Move move = Command::Move{Command::Pace::Medium, Command::Direction::Forward};
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

    Command::Move move = Command::Move{Command::Pace::Medium, Command::Direction::Forward};
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


