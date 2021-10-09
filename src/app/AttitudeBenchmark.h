//
// Attitude Benchmark App
// Monitors and reports time spent doing attitude computations.
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#ifndef _BITTLEET_APP_ATTITUDE_BENCHMARK_H_ 
#define _BITTLEET_APP_ATTITUDE_BENCHMARK_H_

#include "App.h"

class AttitudeBenchmark : public App {
public:
    AttitudeBenchmark() = default;
    
    void setup();
    void loop();
};


#endif // _BITTLEET_APP_ATTITUDE_BENCHMARK_H_
