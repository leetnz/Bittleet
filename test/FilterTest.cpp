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

#include "math/Filters.h"

TEST_CASE("Update_IIR", "[Filters]" ) 
{
    struct Step {
       float input;
       float expected;
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
                    .input = 1.0,
                    .expected = 1.0,
                },
                {
                    .input = -1.0,
                    .expected = -1.0,
                }
            }, 
        },
        { 
            .name = "0.5 filtering",
            .filterCoefficient = 0.5,  
            .steps = {
                {
                    .input = -1.0,
                    .expected = -0.5,
                },
                {
                    .input = -1.0,
                    .expected = -0.75,
                },
            }, 
        },
        { 
            .name = "0.25 filtering",
            .filterCoefficient = 0.25,  
            .steps = {
                {
                    .input = 1.0,
                    .expected = 0.25,
                },
                {
                    .input = 1.0,
                    .expected = (0.25 + 0.75 * 0.25),
                },
                {
                    .input = 0.0,
                    .expected = 0.75 * (0.25 + 0.75 * 0.25),
                },
            }, 
        },
    };

    for (auto& tc : testCases) {
        SECTION(tc.name) {
            float y = 0.0;
            for (auto& step: tc.steps) {
                y = applyIIR(step.input, y, tc.filterCoefficient);
                NEAR(step.expected, y, 1e-3f);
            }
        }
    }
}

