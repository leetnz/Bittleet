//
// Arduino Stream Mock
// Allows us to mock Serial
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//


#include <string>
#include <iostream>

#include "Stream.h"

Stream Serial = Stream(""); // Global serial object used by Arduino Libraries.

Stream::Stream(const std::string& bytes) : buffer(bytes) {}

int16_t Stream::available(){ return (int)buffer.size(); }

int16_t Stream::read() {
    if (buffer.empty()) {
        return -1; 
    }
    int value = buffer.front();
    buffer = buffer.substr(1);
    return value;
}

std::string Stream::readStringUntil(char terminator) {
    return buffer; 
}

int16_t Stream::print(const char* s) { 
    std::cout << s;
    return 0; 
}
int16_t Stream::println(const char* s) { 
    std::cout << s << std::endl;
    return 0; 
}
