//
// Bittleet App
// Operational firmware for Petoi's Bittle quadraped robot.
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#ifndef _BITTLEET_APP_BITTLEET_H_ 
#define _BITTLEET_APP_BITTLEET_H_

#include "App.h"

class Bittleet : public App {
public:
    Bittleet() = default;
    
    void setup();
    void loop();
};


#endif // _BITTLEET_APP_BITTLEET_H_
