//
// Attitude Tests
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#include "catch.hpp"
#include "Helpers.h"

#include <vector>

#include "Arduino.h"
#include "Wire.h"

#include "skill/LoaderEeprom.h"

#include "math/trig.h"

#define NOMINAL_G (16384)
#define NOMINAL_G_2_AXES (11585)
#define NOMINAL_G_3_AXES (9459)

using LoaderEeprom = Skill::LoaderEeprom;
using Type = Skill::Type;
using LoopSpec = Skill::LoopSpec;

class LoaderWhitebox : public LoaderEeprom {
    public:
        LoaderWhitebox() = default;
        void loadFromAddress(uint16_t address, Skill::Skill& skill) {
            _loadFromAddress(address, skill);
        }
        int16_t lookupAddressByName(const char* name) {
            return _lookupAddressByName(name);
        }
};

TEST_CASE("LoaderEeprom::_loadFromAddress", "[LoaderEeprom]" ) 
{ 
    struct TestCase {
        std::string name;
        uint16_t address;
        std::vector<int8_t> readBuffer;
        Skill::Skill expected;
        std::vector<int8_t> expectedSpec;
    };

    const std::vector<TestCase> testCases = {
        TestCase{ 
            .name = "posture",      
            .address = 0x1234, 
            .readBuffer = std::vector<int8_t>{
                1, 0, 0, 1,
                0,   0,   0,   0,   0,   0,   0,   0,  30,  30,  30,  30,  30,  30,  30,  30,
            },
            .expected = Skill::Skill{
                .type = Type::Posture,
                .frames = 1,
                .nominalRoll = 0,
                .nominalPitch = 0,
                .doubleAngles = false,
                .loopSpec = LoopSpec{},
            },
            .expectedSpec = std::vector<int8_t>{
                0,   0,   0,   0,   0,   0,   0,   0,  30,  30,  30,  30,  30,  30,  30,  30,
            },
        },
        TestCase{ 
            .name = "gait",      
            .address = 0xabcd, 
            .readBuffer = std::vector<int8_t>{
                3, 5, -5, 2,
                20,  21,  22,  23,  24,  25,  26,  27,
                30,  31,  32,  33,  34,  35,  36,  37,
                40,  41,  42,  43,  44,  45,  46,  47,
            },
            .expected = Skill::Skill{
                .type = Type::Gait,
                .frames = 3,
                .nominalRoll = 5,
                .nominalPitch = -5,
                .doubleAngles = true,
                .loopSpec = LoopSpec{},
            },
            .expectedSpec = std::vector<int8_t>{
                20,  21,  22,  23,  24,  25,  26,  27,
                30,  31,  32,  33,  34,  35,  36,  37,
                40,  41,  42,  43,  44,  45,  46,  47,
            },
        },
        TestCase{ 
            .name = "behavior",      
            .address = 0x5678, 
            .readBuffer = std::vector<int8_t>{
                -5, -2, 7, 1,
                1, 3, 10,
                0, 0, 0, 0, 0, 0, 0, 0, 10,  11,  12,  13,  14,  15,  16,  17,      0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 20,  21,  22,  23,  24,  25,  26,  27,      0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 30,  31,  32,  33,  34,  35,  36,  37,      0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 40,  41,  42,  43,  44,  45,  46,  47,      0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 50,  51,  52,  53,  54,  55,  56,  57,      0, 0, 0, 0,
            },
            .expected = Skill::Skill{
                .type = Type::Behaviour,
                .frames = 5,
                .nominalRoll = -2,
                .nominalPitch = 7,
                .doubleAngles = false,
                .loopSpec = LoopSpec{
                    .firstRow = 1,
                    .finalRow = 3,
                    .count = 10,
                },
            },
            .expectedSpec = std::vector<int8_t>{
                0, 0, 0, 0, 0, 0, 0, 0, 10,  11,  12,  13,  14,  15,  16,  17,      0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 20,  21,  22,  23,  24,  25,  26,  27,      0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 30,  31,  32,  33,  34,  35,  36,  37,      0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 40,  41,  42,  43,  44,  45,  46,  47,      0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 50,  51,  52,  53,  54,  55,  56,  57,      0, 0, 0, 0,
            },
        },
        TestCase{ 
            .name = "invalid",      
            .address = 0x1234, 
            .readBuffer = std::vector<int8_t>{
                0, 0, 0, 1,
            },
            .expected = Skill::Skill::Empty(),
            .expectedSpec = std::vector<int8_t>{},
        },
    };

    for (auto& tc : testCases) {
        SECTION(tc.name) {            
            Wire = WireMock();
            Wire.readBuffer = tc.readBuffer;

            LoaderWhitebox loader = LoaderWhitebox();
            Skill::Skill skill = Skill::Skill::Empty();
            loader.loadFromAddress(tc.address, skill);

            REQUIRE(Wire.writeBuffer.size() == 2);
            REQUIRE(Wire.writeBuffer[0] == ((tc.address >> 8) & 0xFF));
            REQUIRE(Wire.writeBuffer[1] == (tc.address & 0xFF));

            REQUIRE(tc.expected.type == skill.type);
            REQUIRE(tc.expected.frames == skill.frames);
            REQUIRE(tc.expected.nominalRoll == skill.nominalRoll);
            REQUIRE(tc.expected.nominalPitch == skill.nominalPitch);
            REQUIRE(tc.expected.doubleAngles == skill.doubleAngles);
            REQUIRE(tc.expectedSpec.size() == skill.specLength);
            for (size_t i = 0; i< tc.expectedSpec.size(); i++) {
                REQUIRE(tc.expectedSpec[i] == skill.spec[i]);
            }
            if (tc.expected.type == Type::Behaviour) {
                REQUIRE(tc.expected.loopSpec.firstRow == skill.loopSpec.firstRow);
                REQUIRE(tc.expected.loopSpec.finalRow == skill.loopSpec.finalRow);
                REQUIRE(tc.expected.loopSpec.count == skill.loopSpec.count);
            }
        }
    }
}

