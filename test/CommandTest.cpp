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
#include "command/Command.h"

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

TEST_CASE("Move Commands", "[Command]" ) 
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

TEST_CASE("Command Move Forward", "[Command]" ) 
{ 
    struct TestCase {
        std::string name;
        Move move;
    };

    const Direction dir = Direction::Forward;
    Command::Command expected = Command::Command(Move{Pace::Medium, Direction::Forward});

    const std::vector<TestCase> testCases = {
        { "MF", Move{Pace::Medium, Direction::Forward} },
        { "FF", Move{Pace::Fast, Direction::Forward} },
        { "SF", Move{Pace::Slow, Direction::Forward} },
        { "RF", Move{Pace::Reverse, Direction::Forward} },
        { "ML", Move{Pace::Medium, Direction::Left} },
        { "MR", Move{Pace::Medium, Direction::Right} },
        { "RL", Move{Pace::Reverse, Direction::Left} },
        { "SR", Move{Pace::Slow, Direction::Right} },
    };

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            Command::Command result = Command::Command(dir, tc.move);
            REQUIRE(expected == result);
        }
    }
}

TEST_CASE("Command Move Left/Right", "[Command]" ) 
{ 
    struct Setup {
        std::string name;
        Direction dir;
    };

    struct TestCase {
        std::string name;
        Move move;
        Command::Command expected;
    };

    const std::vector<Setup> setups = {
        { "Left",    Direction::Left  },
        { "Right",   Direction::Right },
    };

    for (auto& setup : setups) {

        const std::vector<TestCase> testCases = {
            { setup.name + " MF", Move{Pace::Medium, Direction::Forward},     Command::Command(Move{Pace::Medium,  setup.dir}) },
            { setup.name + " FF", Move{Pace::Fast, Direction::Forward},       Command::Command(Move{Pace::Fast,    setup.dir}) },
            { setup.name + " SF", Move{Pace::Slow, Direction::Forward},       Command::Command(Move{Pace::Slow,    setup.dir}) },
            { setup.name + " RF", Move{Pace::Reverse, Direction::Forward},    Command::Command(Move{Pace::Reverse, setup.dir}) },
            { setup.name + " ML", Move{Pace::Medium, Direction::Left},        Command::Command(Move{Pace::Medium,  setup.dir}) },
            { setup.name + " MR", Move{Pace::Medium, Direction::Right},       Command::Command(Move{Pace::Medium,  setup.dir}) },
            { setup.name + " RL", Move{Pace::Reverse, Direction::Left},       Command::Command(Move{Pace::Reverse, setup.dir}) },
            { setup.name + " SR", Move{Pace::Slow, Direction::Right},         Command::Command(Move{Pace::Slow,    setup.dir}) },
        };

        for (auto& tc : testCases) {
            SECTION(tc.name) {
                Command::Command result = Command::Command(setup.dir, tc.move);
                REQUIRE(tc.expected == result);
            }
        }
    }
}


TEST_CASE("Command Move Pace", "[Command]" ) 
{ 
    struct Setup {
        std::string name;
        Pace pace;
    };

    struct TestCase {
        std::string name;
        Move move;
        Command::Command expected;
    };

    const std::vector<Setup> setups = {
        { "Reverse", Pace::Reverse },
        { "Slow",    Pace::Slow },
        { "Medium",  Pace::Medium },
        { "Fast",    Pace::Fast },
    };

    for (auto& setup : setups) {

        const std::vector<TestCase> testCases = {
            { setup.name + "MF", Move{Pace::Medium, Direction::Forward},     Command::Command(Move{setup.pace,  Direction::Forward}) },
            { setup.name + "FF", Move{Pace::Fast, Direction::Forward},       Command::Command(Move{setup.pace,  Direction::Forward}) },
            { setup.name + "SF", Move{Pace::Slow, Direction::Forward},       Command::Command(Move{setup.pace,  Direction::Forward}) },
            { setup.name + "RF", Move{Pace::Reverse, Direction::Forward},    Command::Command(Move{setup.pace,  Direction::Forward}) },
            { setup.name + "ML", Move{Pace::Medium, Direction::Left},        Command::Command(Move{setup.pace,  Direction::Left}) },
            { setup.name + "MR", Move{Pace::Medium, Direction::Right},       Command::Command(Move{setup.pace,  Direction::Right}) },
            { setup.name + "RL", Move{Pace::Reverse, Direction::Left},       Command::Command(Move{setup.pace,  Direction::Left}) },
            { setup.name + "SR", Move{Pace::Slow, Direction::Right},         Command::Command(Move{setup.pace,  Direction::Right}) },
        };

        for (auto& tc : testCases) {
            SECTION(tc.name) {
                Command::Command result = Command::Command(setup.pace, tc.move);
                REQUIRE(tc.expected == result);
            }
        }
    }   
}
