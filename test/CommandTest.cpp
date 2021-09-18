//
// Command Tests
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//


#include "catch.hpp"
#include <vector>

#include "Arduino.h"
#include "Command.h"

using namespace Command;

TEST_CASE("Command Equality", "[Command]" ) 
{
    struct Setup {
        std::string type;
        Command::Command cmd;
    };

    struct TestCase {
        std::string name;
        Command::Command a;
        Command::Command b;
        bool equal;
    };

    const std::vector<Setup> setups = {
        {"Empty",   Command::Command()},
        {"Simple",  Command::Command(Simple::Balance)},
        {"Move FL", Command::Command(Move{Pace::Fast, Direction::Left})},
        {"Move SR", Command::Command(Move{Pace::Slow, Direction::Right})},
        {"Move RF", Command::Command(Move{Pace::Reverse, Direction::Forward})},
        {"Args M0", Command::Command(WithArgs{ArgType::Meow, 0, {}})},
        {"Args C4", Command::Command(WithArgs{ArgType::Calibrate, 4, {0x11, 0x12, 0x13, 0x14, 0x15}})},
        {"Args C5", Command::Command(WithArgs{ArgType::Calibrate, 5, {0x21, 0x12, 0x13, 0x14, 0x15}})},
    };

    for (auto& su : setups) {
        const std::vector<TestCase> testCases = {
            {su.type + " == " + su.type, su.cmd, su.cmd, true},
            {su.type + " != Simple",     su.cmd, Command::Command(Simple::Rest), false},
            {su.type + " != Move",       su.cmd, Command::Command(Move{Pace::Slow, Direction::Forward}), false},
            {su.type + " != WithArgs",   su.cmd, Command::Command(WithArgs{ArgType::Calibrate, 5, {0x11, 0x12, 0x13, 0x14, 0x15}}), false},
        };

        for (auto& tc : testCases) {
            SECTION(tc.name) {
                const bool result = (tc.a == tc.b);
                const bool invResult = (tc.a != tc.b);
                REQUIRE(tc.equal == result);
                REQUIRE(tc.equal != invResult);
            }
        }
    }
}




