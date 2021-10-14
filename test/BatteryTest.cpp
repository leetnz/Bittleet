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

#include "state/Battery.h"
#include "state/Status.h"

using namespace Battery;


TEST_CASE("State", "[Battery]" ) 
{ 
    struct TestCase {
        std::string name;
        int measurement;
        Status::Battery expected;
    };

    const std::vector<TestCase> testCases = {
        { "No Battery",      0, Status::Battery{Status::BatteryLevel::None, 0}},
        { "Low Battery",   300, Status::Battery{Status::BatteryLevel::Low, 0}},
        { "Good Battery",  730, Status::Battery{Status::BatteryLevel::Ok, 50}},
        { "Full Battery",  820, Status::Battery{Status::BatteryLevel::Ok, 100}},
        { "Overcharged",  1000, Status::Battery{Status::BatteryLevel::Ok, 100}},
    };

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            REQUIRE(tc.expected.level == state(tc.measurement).level);
            REQUIRE(tc.expected.percent == state(tc.measurement).percent);
        }
    }
}
