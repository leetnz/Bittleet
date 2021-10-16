//
// Arduino Stream Mock
// Allows us to mock Serial
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#ifndef _BITTLEET_MOCK_ARDUINO_STREAM_H_
#define _BITTLEET_MOCK_ARDUINO_STREAM_H_

#include <string>
#include <iostream>

class Stream {
public:
    Stream(const std::string& bytes);

    int16_t available();
    int16_t read();
    std::string readStringUntil(char terminator);
    int16_t print(const char* s);
    int16_t println(const char* s);

    std::string buffer;
};

extern Stream Serial;

#endif // _BITTLEET_MOCK_ARDUINO_STREAM_H_