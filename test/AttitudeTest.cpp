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

#include "Attitude.h"

#include "trig.h"

TEST_CASE("Attitude::Update", "[Attitude]" ) 
{ 
    struct TestCase {
        std::string name;
        Attitude::GravityMeasurement input;
        Attitude::Angles expected;
    };

    const std::vector<TestCase> testCases = {
        { 
            .name = "No Angles",      
            .input = Attitude::GravityMeasurement{0, 0, 1}, 
            .expected = Attitude::Angles{0.0, 0.0},
        },
        { 
            .name = "Right angles",      
            .input = Attitude::GravityMeasurement{1, 1, 0}, 
            .expected = Attitude::Angles{-90.0 * M_DEG2RAD, -90.0 * M_DEG2RAD},
        },
        { 
            .name = "45 degree",      
            .input = Attitude::GravityMeasurement{1, 1, 1}, 
            .expected = Attitude::Angles{-45.0 * M_DEG2RAD, -45.0 * M_DEG2RAD},
        },
        { 
            .name = "upside down",      
            .input = Attitude::GravityMeasurement{0, 0, -1}, 
            .expected = Attitude::Angles{-180.0 * M_DEG2RAD, -180.0 * M_DEG2RAD},
        },
        { 
            .name = "roll only",      
            .input = Attitude::GravityMeasurement{0, 1, 1}, 
            .expected = Attitude::Angles{-45.0 * M_DEG2RAD, 0.0},
        },
        { 
            .name = "pitch only",      
            .input = Attitude::GravityMeasurement{1, 0, 1}, 
            .expected = Attitude::Angles{0.0, -45.0 * M_DEG2RAD},
        },
    };

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            Attitude::Attitude attitude = Attitude::Attitude();
            Attitude::Angles result = attitude.update(tc.input);
            NEAR(tc.expected.roll, result.roll, 1e-7f);
            NEAR(tc.expected.pitch, result.pitch, 1e-7f);
        }
    }
}
