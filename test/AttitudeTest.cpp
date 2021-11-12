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

#define RAD_PER_S (1877)
#define NOMINAL_G (16384)
#define NOMINAL_G_2_AXES (11585)
#define NOMINAL_G_3_AXES (9459)

using Measurement = Attitude::Measurement;
using Vec3 = Attitude::Vec3;

TEST_CASE("Attitude::Update_Accel", "[Attitude]" ) 
{ 
    struct TestCase {
        std::string name;
        Measurement input;
        float expectedRoll;
        float expectedPitch;
    };

    const std::vector<TestCase> testCases = {
        { 
            .name = "No Angles",      
            .input = Measurement{.us=0, .accel = Vec3{0, 0, NOMINAL_G}, .gyro=Vec3{0,0,0}},
            .expectedRoll = 0.0f,
            .expectedPitch = 0.0f,
        },
        { 
            .name = "Right angles",      
            .input = Measurement{.accel = Vec3{NOMINAL_G_2_AXES, NOMINAL_G_2_AXES, 0}}, 
            .expectedRoll = 90.0 * M_DEG2RAD,
            .expectedPitch = 90.0 * M_DEG2RAD,
        },
        { 
            .name = "45 degree",      
            .input = Measurement{.accel = Vec3{NOMINAL_G_3_AXES, NOMINAL_G_3_AXES, NOMINAL_G_3_AXES}}, 
            .expectedRoll = 45.0 * M_DEG2RAD,
            .expectedPitch = 45.0 * M_DEG2RAD,
        },
        { 
            .name = "upside down",      
            .input = Measurement{.accel = Vec3{0, 0, -NOMINAL_G}}, 
            .expectedRoll = 180.0 * M_DEG2RAD,
            .expectedPitch = 180.0 * M_DEG2RAD,
        },
        { 
            .name = "roll only",      
            .input = Measurement{.accel = Vec3{0, NOMINAL_G_2_AXES, NOMINAL_G_2_AXES}}, 
            .expectedRoll = 45.0 * M_DEG2RAD,
            .expectedPitch = 0.0f,
        },
        { 
            .name = "pitch only",      
            .input = Measurement{.accel = Vec3{NOMINAL_G_2_AXES, 0, NOMINAL_G_2_AXES}}, 
            .expectedRoll = 0.0f,
            .expectedPitch = 45.0 * M_DEG2RAD,
        },
    };

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            Attitude::Attitude attitude = Attitude::Attitude();
            attitude.update(tc.input);
            NEAR(tc.expectedRoll, attitude.roll(), 1e-3f);
            NEAR(tc.expectedPitch, attitude.pitch(), 1e-3f);
            NEAR(tc.expectedRoll, attitude.angleFromAxis(Attitude::Axis::Roll), 1e-3f);
            NEAR(tc.expectedPitch, attitude.angleFromAxis(Attitude::Axis::Pitch), 1e-3f);
        }
    }
}

TEST_CASE("Attitude::Update_Gyro", "[Attitude]" ) 
{ 
    struct TestCase {
        std::string name;
        Attitude::Measurement input;
        float expectedRoll;
        float expectedPitch;
    };

    const std::vector<TestCase> testCases = {
        { 
            .name = "No Change",      
            .input = Measurement{.us = 1000000, .gyro = Vec3{0, 0, 0}}, 
            .expectedRoll = 0.0f,
            .expectedPitch = 0.0f,
        },
        { 
            .name = "Trapezoidal Integration roll",      
            .input = Measurement{.us = 1000000, .gyro = Vec3{RAD_PER_S, 0, 0}}, 
            .expectedRoll = 0.5f,
            .expectedPitch = 0.0f,
        },
        { 
            .name = "Trapezoidal Integration pitch",      
            .input = Measurement{.us = 1000000, .gyro = Vec3{0, -RAD_PER_S, 0}}, 
            .expectedRoll = 0.0f,
            .expectedPitch = -0.5f,
        },
        { 
            .name = "Trapezoidal Integration at 100Hz",      
            .input = Measurement{.us = 10000, .gyro = Vec3{10 *RAD_PER_S, 4 *RAD_PER_S, 0}}, 
            .expectedRoll = 0.05f,
            .expectedPitch = 0.02f,
        },
    };

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            Attitude::Attitude attitude = Attitude::Attitude();
            attitude.update(
                Measurement{
                    .us = 0, 
                    .accel = Vec3{0, 0, NOMINAL_G},
                    .gyro = Vec3{0, 0, 0}, 
                }
            );

            float tol = std::max((abs(tc.expectedPitch) + abs(tc.expectedRoll)) * 1e-3, 1e-6);

            attitude.update(tc.input);
            NEAR(tc.expectedRoll, attitude.roll(), tol);
            NEAR(tc.expectedPitch, attitude.pitch(), tol);
            NEAR(tc.expectedRoll, attitude.angleFromAxis(Attitude::Axis::Roll), tol);
            NEAR(tc.expectedPitch, attitude.angleFromAxis(Attitude::Axis::Pitch), tol);
        }
    }
}


TEST_CASE("Attitude::Reset", "[Attitude]" ) 
{
    struct Step {
       Attitude::Measurement input;
       float expectedRoll;
       float expectedPitch;
    };

    const std::vector<Step> steps = {
        {
            .input = Measurement{.us=0, .accel = Vec3{NOMINAL_G_2_AXES, NOMINAL_G_2_AXES, 0}},
            .expectedRoll = 90.0 * M_DEG2RAD,
            .expectedPitch = 90.0 * M_DEG2RAD,
        },
        {
            .input = Measurement{.us=10000, .accel = Vec3{0, 0, NOMINAL_G}},
            .expectedRoll = 0.0,
            .expectedPitch = 0.0,
        },
    };

    Attitude::Attitude attitude{};
    
    for (auto& step: steps) {
        attitude.reset();
        REQUIRE(0.0 == attitude.pitch());
        REQUIRE(0.0 == attitude.roll());

        attitude.update(step.input);
        NEAR(step.expectedRoll, attitude.roll(), 1e-3f);
        NEAR(step.expectedPitch, attitude.pitch(), 1e-3f);
    }
}

TEST_CASE("Attitude::Update_GravityFilter", "[Attitude]" ) 
{
    class Whitebox : Attitude::Attitude {
    public:
        Whitebox() = default;

        float computeTrust(const Measurement& m) const {
            return _computeTrust(m);
        }
    };

    struct TestCase {
        std::string name;
        Attitude::Measurement input;
        float expectedTrust;
    };

    const std::vector<TestCase> testCases = {
        { 
            .name = "Max trust basic",
            .input = Measurement{
                .us = 0,
                .accel = Vec3{0, 0, NOMINAL_G},
                .gyro= Vec3{},
            },
            .expectedTrust = 1.0,
        },
        { 
            .name = "Max trust x and y",
            .input = Measurement{
                .us = 0,
                .accel = Vec3{NOMINAL_G_2_AXES, NOMINAL_G_2_AXES, 0},
                .gyro= Vec3{},
            },
            .expectedTrust = 1.0,
        },
        { 
            .name = "Max trust x, y and z",
            .input = Measurement{
                .us = 0,
                .accel = Vec3{NOMINAL_G_3_AXES, NOMINAL_G_3_AXES, NOMINAL_G_3_AXES},
                .gyro= Vec3{},
            },
            .expectedTrust = 1.0,
        },
        { 
            .name = "No trust too low",
            .input = Measurement{
                .us = 0,
                .accel = Vec3{0, 0, (int16_t)(0.95 * (float)NOMINAL_G)},
                .gyro= Vec3{},
            },
            .expectedTrust = 0.0,
        },
        { 
            .name = "No trust too high",
            .input = Measurement{
                .us = 0,
                .accel = Vec3{0, 0, (int16_t)(1.05 * (float)NOMINAL_G)},
                .gyro= Vec3{},
            },
            .expectedTrust = 0.0,
        },
    };

    const float tol = 0.05; // 5% error caused by taylor approx of ideal trust fn.

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            Whitebox attitude{};
            NEAR(tc.expectedTrust, attitude.computeTrust(tc.input), tol);
        }
    }
}
