//
// Scheduler Tests
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#include "catch.hpp"

#include <vector>

#include "Arduino.h"

#include "scheduler/Scheduler.h"

TEST_CASE("Registration", "[Scheduler]" ) 
{ 
    SECTION("invalid period"){
        Scheduler::Scheduler<1> s{};
        REQUIRE(s.registerTask(0) == -1);
    }

    SECTION("single task"){
        Scheduler::Scheduler<1> s{};
        REQUIRE(s.registerTask(100) >= 0);
        REQUIRE(s.registerTask(100) == -1);
    }

    SECTION("many tasks"){
        Scheduler::Scheduler<5> s{};
        REQUIRE(s.registerTask(100) >= 0);
        REQUIRE(s.registerTask(100) >= 0);
        REQUIRE(s.registerTask(100) >= 0);
        REQUIRE(s.registerTask(100) >= 0);
        REQUIRE(s.registerTask(100) >= 0);
        REQUIRE(s.registerTask(100) == -1);
    }
}
