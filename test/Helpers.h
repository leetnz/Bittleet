//
// Test Helpers
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#ifndef _BITTLEET_TEST_HELPERS_H_
#define _BITTLEET_TEST_HELPERS_H_

#include <iostream>

#include "catch.hpp"


void NEAR(double expected, double result, double tol){
    if (fabs(expected - result) > tol) {
        std::ostringstream str;
        str << "difference between " << expected << " and "<< result <<" is greater than " << tol;
        FAIL(str.str());
    }
}

# endif // _BITTLEET_TEST_HELPERS_H_