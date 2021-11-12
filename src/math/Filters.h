//
// Filters
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

// We are solving the equation:
//      y[k] = A*x[k] + B*y[k-1]
inline float applyIIR(float xCurrent, float yLast, float coeff) {
    return coeff * xCurrent + (1.0f - coeff) * yLast;
}
