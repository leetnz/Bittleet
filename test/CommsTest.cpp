#include "catch.hpp"

#include <vector>

#include "Arduino.h"

#include "Comms.h"
#include "Command.h"
#include "Bittle.h"

using namespace Comms;



TEST_CASE("ParseSerial_Simple", "[Infrared]" ) 
{ 
    using Simple = Command::Simple;

    struct TestCase {
        std::string name;
        std::string bytes;
        Command::Command expected;
    };

    const std::vector<TestCase> testCases = {
        { "Rest",           "d", Command::Command(Simple::Rest)},
        // { "Gyro Toggle",    IR_CODE_02, Command::Command(Simple::GyroToggle)},

        // { "Balance",        IR_CODE_11, Command::Command(Simple::Balance)},

        // { "Pause",          IR_CODE_20, Command::Command(Simple::Pause)},
        // { "Calibrate",      IR_CODE_22, Command::Command(WithArgs{ArgType::Calibrate, 0, {}})},

        // { "Step",           IR_CODE_30, Command::Command(Simple::Step)},

        // { "Sit",            IR_CODE_41, Command::Command(Simple::Sit)},
        // { "Stretch",        IR_CODE_42, Command::Command(Simple::Stretch)},

        // { "Greet",          IR_CODE_50, Command::Command(Simple::Greet)},
        // { "Pushup",         IR_CODE_51, Command::Command(Simple::Pushup)},
        // { "Hydrant",        IR_CODE_52, Command::Command(Simple::Hydrant)},

        // { "Check",          IR_CODE_60, Command::Command(Simple::Check)},
        // { "Dead",           IR_CODE_61, Command::Command(Simple::Dead)},
        // { "Zero",           IR_CODE_62, Command::Command(Simple::Zero)},
    };

    Command::Move move = Command::Move{Command::Pace::Medium, Command::Direction::Forward};
    int16_t currentPos[DOF] = {};

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            Stream mock = Stream(tc.bytes);
            REQUIRE(tc.expected == parseSerial(mock, move, currentPos));
        }
    }
}




