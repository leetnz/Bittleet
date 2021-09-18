#include <string>
#include <iostream>

#include "Stream.h"

Stream Serial = Stream(""); // Global serial object used by Arduino Libraries.

Stream::Stream(const std::string& bytes) : buffer(bytes) {}

int Stream::available(){ return (int)buffer.size(); }

int Stream::read() {
    if (buffer.empty()) {
        return -1; 
    }
    int value = buffer.front();
    buffer = buffer.substr(1);
    return value;
}

std::string Stream::readStringUntil(char terminator) { return ""; }
