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

TEST_CASE("waitUntilNext", "[Scheduler]" ) 
{ 
    SECTION("nothing registered"){
        Scheduler::Scheduler<1> s{};
        REQUIRE(s.waitUntilNextTask() == -1);
    }

    SECTION("single task"){
        TimeMock::reset();
        Scheduler::Scheduler<1> s{};
        int task = s.registerTask(100);
        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 0);
        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 100);
    }

    SECTION("attempt to retain cadence"){
        TimeMock::reset();
        Scheduler::Scheduler<1> s{};
        int task = s.registerTask(100);
        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 0);

        TimeMock::currentUs=199;
        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 0);
        REQUIRE(TimeMock::currentUs == 199);

        TimeMock::currentUs=299;
        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 0);
        REQUIRE(TimeMock::currentUs == 299);

        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 1);
        REQUIRE(TimeMock::currentUs == 300);
    }

    SECTION("reset if we miss cadence by one period"){
        TimeMock::reset();
        Scheduler::Scheduler<1> s{};
        int task = s.registerTask(100);
        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 0);

        TimeMock::currentUs=250;
        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 0);
        REQUIRE(TimeMock::currentUs == 250);

        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 100);
        REQUIRE(TimeMock::currentUs == 350);
    }

    SECTION("never delay more than 10ms (arduino bug)"){
        TimeMock::reset();
        Scheduler::Scheduler<1> s{};
        int task = s.registerTask(50000);
        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 0);

        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 50000);
        REQUIRE(TimeMock::lastDelayUs == 10000);

        TimeMock::currentUs = 59999;
        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 90001);
        REQUIRE(TimeMock::currentUs == 100000);
    }

    SECTION("delay only required wait"){
        TimeMock::reset();
        Scheduler::Scheduler<1> s{};
        int task = s.registerTask(100);
        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 0);

        TimeMock::currentUs=99;
        REQUIRE(task == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 1);
    }


    SECTION("two tasks"){
        TimeMock::reset();
        Scheduler::Scheduler<2> s{};
        int a = s.registerTask(100);
        int b = s.registerTask(101);
        REQUIRE(a == s.waitUntilNextTask());
        REQUIRE(b == s.waitUntilNextTask());
        REQUIRE(TimeMock::totalDelayUs == 0);

        REQUIRE(a == s.waitUntilNextTask());
        REQUIRE(TimeMock::lastDelayUs == 100);
        REQUIRE(TimeMock::totalDelayUs == 100);

        REQUIRE(b == s.waitUntilNextTask());
        REQUIRE(TimeMock::lastDelayUs == 1);
        REQUIRE(TimeMock::totalDelayUs == 101);

        REQUIRE(a == s.waitUntilNextTask());
        REQUIRE(TimeMock::lastDelayUs == 99);
        REQUIRE(TimeMock::totalDelayUs == 200);
    }

    SECTION("three tasks - incomplete list"){
        TimeMock::reset();
        Scheduler::Scheduler<4> s{};
        int a = s.registerTask(100);
        int b = s.registerTask(150);
        int c = s.registerTask(200);
        REQUIRE(a == s.waitUntilNextTask());
        REQUIRE(b == s.waitUntilNextTask());
        REQUIRE(c == s.waitUntilNextTask());
        REQUIRE(TimeMock::currentUs == 0);

        REQUIRE(a == s.waitUntilNextTask());
        REQUIRE(TimeMock::currentUs == 100);

        REQUIRE(b == s.waitUntilNextTask());
        REQUIRE(TimeMock::currentUs == 150);

        REQUIRE(a == s.waitUntilNextTask());
        REQUIRE(c == s.waitUntilNextTask());
        REQUIRE(TimeMock::currentUs == 200);

        REQUIRE(a == s.waitUntilNextTask());
        REQUIRE(b == s.waitUntilNextTask());
        REQUIRE(TimeMock::currentUs == 300);

        REQUIRE(a == s.waitUntilNextTask());
        REQUIRE(c == s.waitUntilNextTask());
        REQUIRE(TimeMock::currentUs == 400);

        REQUIRE(b == s.waitUntilNextTask());
        REQUIRE(TimeMock::currentUs == 450);

        REQUIRE(a == s.waitUntilNextTask());
        REQUIRE(TimeMock::currentUs == 500);

        REQUIRE(a == s.waitUntilNextTask());
        REQUIRE(b == s.waitUntilNextTask());
        REQUIRE(c == s.waitUntilNextTask());
        REQUIRE(TimeMock::currentUs == 600);
    }
}
