#ifndef _BITTLEET_MOCK_ARDUINO_STREAM_H_
#define _BITTLEET_MOCK_ARDUINO_STREAM_H_

#include <string>
#include <iostream>

class Stream {
public:
    Stream(const std::string& bytes) : buffer(bytes) {}

    int available(){ return (int)buffer.size(); };
    int read() {
        if (buffer.empty()) {
            return -1; 
        }
        int value = buffer.front();
        buffer = buffer.substr(1);
        return value;
    }
    std::string readStringUntil(char terminator) { return ""; };

    std::string buffer;
};

# endif // _BITTLEET_MOCK_ARDUINO_STREAM_H_