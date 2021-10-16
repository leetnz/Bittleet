//
// Main sketch for Bittleet
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//
// Bittleet was forked from OpenCat, the bionic quadruped walking robot.
// https://github.com/PetoiCamp/OpenCat
//
// OpenCat is Copyright (c) 2021 Petoi LLC.
//

// #include "src/app/AttitudeBenchmark.h"
// App* app = new AttitudeBenchmark();

#include "src/app/Bittleet.h"
App* app = new Bittleet();

void setup() {
  app->setup();
}

void loop() {
  app->loop();
}
