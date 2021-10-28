//
// Arduino Mock
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//


#ifndef _BITTLEET_MOCK_ARDUINO_H_
#define  _BITTLEET_MOCK_ARDUINO_H_

#include <string>
#include <cstring>
#include <sstream>
#include <math.h>

#include "Stream.h"

#define F(s) (s)

#define min(a,b) std::min(a,b)

typedef uint8_t byte;
typedef std::string String;

class TimeMock{
public:
    static void reset();
    static uint32_t currentUs;
    static uint16_t lastDelayUs;
    static uint32_t totalDelayUs;
};

uint32_t micros();
void delayMicroseconds(uint16_t us);


# endif // _BITTLEET_MOCK_ARDUINO_H_