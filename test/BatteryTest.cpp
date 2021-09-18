//
// Battery Tests
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#include "catch.hpp"

#include <vector>

#include "Arduino.h"

#include "Battery.h"

using namespace Battery;


TEST_CASE("State", "[Battery]" ) 
{ 
    struct TestCase {
        std::string name;
        int measurement;
        State expected;
    };

    const std::vector<TestCase> testCases = {
        { "No Battery",      0, State::None},
        { "Low Battery",   300, State::Low},
        { "Good Battery",  700, State::Ok},
    };

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            REQUIRE(tc.expected == state(tc.measurement));
        }
    }
}
