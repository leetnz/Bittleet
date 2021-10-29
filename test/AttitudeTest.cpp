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

#include "state/Attitude.h"

#include "math/trig.h"

#define NOMINAL_G (16384)
#define NOMINAL_G_2_AXES (11585)
#define NOMINAL_G_3_AXES (9459)

TEST_CASE("Attitude::Update_Simple", "[Attitude]" ) 
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
            .input = Attitude::GravityMeasurement{0, 0, NOMINAL_G}, 
            .expectedRoll = 0.0f,
            .expectedPitch = 0.0f,
        },
        { 
            .name = "Right angles",      
            .input = Attitude::GravityMeasurement{NOMINAL_G_2_AXES, NOMINAL_G_2_AXES, 0}, 
            .expectedRoll = -90.0 * M_DEG2RAD,
            .expectedPitch = -90.0 * M_DEG2RAD,
        },
        { 
            .name = "45 degree",      
            .input = Attitude::GravityMeasurement{NOMINAL_G_3_AXES, NOMINAL_G_3_AXES, NOMINAL_G_3_AXES}, 
            .expectedRoll = -45.0 * M_DEG2RAD,
            .expectedPitch = -45.0 * M_DEG2RAD,
        },
        { 
            .name = "upside down",      
            .input = Attitude::GravityMeasurement{0, 0, -NOMINAL_G}, 
            .expectedRoll = -180.0 * M_DEG2RAD,
            .expectedPitch = -180.0 * M_DEG2RAD,
        },
        { 
            .name = "roll only",      
            .input = Attitude::GravityMeasurement{0, NOMINAL_G_2_AXES, NOMINAL_G_2_AXES}, 
            .expectedRoll = -45.0 * M_DEG2RAD,
            .expectedPitch = 0.0f,
        },
        { 
            .name = "pitch only",      
            .input = Attitude::GravityMeasurement{NOMINAL_G_2_AXES, 0, NOMINAL_G_2_AXES}, 
            .expectedRoll = 0.0f,
            .expectedPitch = -45.0 * M_DEG2RAD,
        },
    };

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            Attitude::Attitude attitude = Attitude::Attitude();
            REQUIRE(attitude.update(tc.input));
            NEAR(tc.expectedRoll, attitude.roll(), 1e-3f);
            NEAR(tc.expectedPitch, attitude.pitch(), 1e-3f);
            NEAR(tc.expectedRoll, attitude.angleFromAxis(Attitude::Axis::Roll), 1e-3f);
            NEAR(tc.expectedPitch, attitude.angleFromAxis(Attitude::Axis::Pitch), 1e-3f);
        }
    }
}

TEST_CASE("Attitude::Update_IIR", "[Attitude]" ) 
{
    struct Step {
       Attitude::GravityMeasurement input;
       float expectedRoll;
       float expectedPitch;
    };
    struct TestCase {
        std::string name;
        float filterCoefficient;
        std::vector<Step> steps;
    };

    const std::vector<TestCase> testCases = {
        { 
            .name = "No filtering",
            .filterCoefficient = 1.0,  
            .steps = {
                {
                    .input = Attitude::GravityMeasurement{0, 0, NOMINAL_G},
                    .expectedRoll = 0.0,
                    .expectedPitch = 0.0,
                },
                {
                    .input = Attitude::GravityMeasurement{0, NOMINAL_G_2_AXES, NOMINAL_G_2_AXES},
                    .expectedRoll = -45.0 * M_DEG2RAD,
                    .expectedPitch = 0.0,
                }
            }, 
        },
        { 
            .name = "0.5 filtering",
            .filterCoefficient = 0.5,  
            .steps = {
                {
                    .input = Attitude::GravityMeasurement{0, 0, NOMINAL_G},
                    .expectedRoll = 0.0,
                    .expectedPitch = 0.0,
                },
                {
                    .input = Attitude::GravityMeasurement{NOMINAL_G_2_AXES, NOMINAL_G_2_AXES, 0},
                    .expectedRoll = -45.0 * M_DEG2RAD,
                    .expectedPitch = -45.0 * M_DEG2RAD,
                },
                {
                    .input = Attitude::GravityMeasurement{NOMINAL_G_2_AXES, NOMINAL_G_2_AXES, 0},
                    .expectedRoll = -90.0 * 0.75 * M_DEG2RAD,
                    .expectedPitch = -90.0 * 0.75 * M_DEG2RAD,
                },
            }, 
        },
        { 
            .name = "0.25 filtering",
            .filterCoefficient = 0.25,  
            .steps = {
                {
                    .input = Attitude::GravityMeasurement{0, 0, NOMINAL_G},
                    .expectedRoll = 0.0,
                    .expectedPitch = 0.0,
                },
                {
                    .input = Attitude::GravityMeasurement{NOMINAL_G_2_AXES, NOMINAL_G_2_AXES, 0},
                    .expectedRoll = -90.0 * 0.25 * M_DEG2RAD,
                    .expectedPitch = -90.0 * 0.25 * M_DEG2RAD,
                },
                {
                    .input = Attitude::GravityMeasurement{NOMINAL_G_2_AXES, NOMINAL_G_2_AXES, 0},
                    .expectedRoll = -90.0 * M_DEG2RAD * (0.25 + 0.75 * 0.25),
                    .expectedPitch = -90.0 * M_DEG2RAD * (0.25 + 0.75 * 0.25),
                },
                {
                    .input = Attitude::GravityMeasurement{0, 0, NOMINAL_G},
                    .expectedRoll = -90.0 * M_DEG2RAD * 0.75 * (0.25 + 0.75 * 0.25),
                    .expectedPitch = -90.0 * M_DEG2RAD * 0.75 * (0.25 + 0.75 * 0.25),
                },
            }, 
        },
        { 
            .name = "No filter on first measurment",
            .filterCoefficient = 0.25,  
            .steps = {
                {
                    .input = Attitude::GravityMeasurement{NOMINAL_G_2_AXES, NOMINAL_G_2_AXES, 0},
                    .expectedRoll = -90.0 * M_DEG2RAD,
                    .expectedPitch = -90.0 * M_DEG2RAD,
                },
                {
                    .input = Attitude::GravityMeasurement{0, 0, NOMINAL_G},
                    .expectedRoll = -90.0 * M_DEG2RAD * 0.75,
                    .expectedPitch = -90.0 * M_DEG2RAD * 0.75,
                },
            }, 
        },

    };

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            Attitude::Attitude attitude = Attitude::Attitude(tc.filterCoefficient);
            for (auto& step: tc.steps) {
                REQUIRE(attitude.update(step.input));
                NEAR(step.expectedRoll, attitude.roll(), 1e-3f);
                NEAR(step.expectedPitch, attitude.pitch(), 1e-3f);
            }
        }
    }
}

TEST_CASE("Attitude::Reset", "[Attitude]" ) 
{
    struct Step {
       Attitude::GravityMeasurement input;
       float expectedRoll;
       float expectedPitch;
    };

    const std::vector<Step> steps = {
        {
            .input = Attitude::GravityMeasurement{NOMINAL_G_2_AXES, NOMINAL_G_2_AXES, 0},
            .expectedRoll = -90.0 * M_DEG2RAD,
            .expectedPitch = -90.0 * M_DEG2RAD,
        },
        {
            .input = Attitude::GravityMeasurement{0, 0, NOMINAL_G},
            .expectedRoll = -90.0 * 0.75 * M_DEG2RAD,
            .expectedPitch = -90.0 * 0.75 * M_DEG2RAD,
        },
    };

    Attitude::Attitude attitude = Attitude::Attitude(0.25);
    for (auto& step: steps) {
        REQUIRE(attitude.update(step.input));
    }
    attitude.reset();
    REQUIRE(0.0 == attitude.pitch());
    REQUIRE(0.0 == attitude.roll());

    for (auto& step: steps) {
        REQUIRE(attitude.update(step.input));
        NEAR(step.expectedRoll, attitude.roll(), 1e-3f);
        NEAR(step.expectedPitch, attitude.pitch(), 1e-3f);
    }
}

TEST_CASE("Attitude::Update_GravityFilter", "[Attitude]" ) 
{
    struct TestCase {
        std::string name;
        Attitude::GravityMeasurement input;
        float expectedRoll;
        float expectedPitch;
    };

    const std::vector<TestCase> testCases = {
        { 
            .name = "Reject Too High",
            .input = Attitude::GravityMeasurement{NOMINAL_G_2_AXES * 2, NOMINAL_G_2_AXES * 2, 0},
            .expectedRoll = 0.0,
            .expectedPitch = 0.0,
        },
        { 
            .name = "Reject Too Low",
            .input = Attitude::GravityMeasurement{NOMINAL_G_2_AXES / 4, NOMINAL_G_2_AXES / 4, 0},
            .expectedRoll = 0.0,
            .expectedPitch = 0.0, 
        },
    };

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            Attitude::Attitude attitude = Attitude::Attitude();
            REQUIRE_FALSE(attitude.update(tc.input));
            NEAR(tc.expectedRoll, attitude.roll(), 1e-3f);
            NEAR(tc.expectedPitch, attitude.pitch(), 1e-3f);
        }
    }
}
