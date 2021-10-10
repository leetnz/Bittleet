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
        float expectedRoll;
        float expectedPitch;
    };

    const std::vector<TestCase> testCases = {
        { 
            .name = "No Angles",      
            .input = Attitude::GravityMeasurement{0, 0, 1}, 
            .expectedRoll = 0.0f,
            .expectedPitch = 0.0f,
        },
        { 
            .name = "Right angles",      
            .input = Attitude::GravityMeasurement{1, 1, 0}, 
            .expectedRoll = -90.0 * M_DEG2RAD,
            .expectedPitch = -90.0 * M_DEG2RAD,
        },
        { 
            .name = "45 degree",      
            .input = Attitude::GravityMeasurement{1, 1, 1}, 
            .expectedRoll = -45.0 * M_DEG2RAD,
            .expectedPitch = -45.0 * M_DEG2RAD,
        },
        { 
            .name = "upside down",      
            .input = Attitude::GravityMeasurement{0, 0, -1}, 
            .expectedRoll = -180.0 * M_DEG2RAD,
            .expectedPitch = -180.0 * M_DEG2RAD,
        },
        { 
            .name = "roll only",      
            .input = Attitude::GravityMeasurement{0, 1, 1}, 
            .expectedRoll = -45.0 * M_DEG2RAD,
            .expectedPitch = 0.0f,
        },
        { 
            .name = "pitch only",      
            .input = Attitude::GravityMeasurement{1, 0, 1}, 
            .expectedRoll = 0.0f,
            .expectedPitch = -45.0 * M_DEG2RAD,
        },
    };

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            Attitude::Attitude attitude = Attitude::Attitude();
            attitude.update(tc.input);
            NEAR(tc.expectedRoll, attitude.roll(), 1e-7f);
            NEAR(tc.expectedPitch, attitude.pitch(), 1e-7f);
        }
    }
}
