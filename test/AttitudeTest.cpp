//
// Attitude Tests
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#include "catch.hpp"

#include <vector>

#include "Arduino.h"

#include "Attitude.h"

TEST_CASE("Update", "[Attitude]" ) 
{ 
    struct TestCase {
        std::string name;
        Attitude::GravityMeasurement input;
        Attitude::Angles expected;
    };

    const std::vector<TestCase> testCases = {
        { "No Angles",      Attitude::GravityMeasurement{0, 0, 1000}, Attitude::Angles{0.0, 0.0}},
    };

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            Attitude::Attitude attitude = Attitude::Attitude();
            Attitude::Angles result = attitude.update(tc.input);
            REQUIRE(tc.expected.roll == result.roll);
            REQUIRE(tc.expected.pitch == result.pitch);
        }
    }
}
