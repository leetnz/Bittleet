#ifndef _BITTLEET_MOCK_ARDUINO_STREAM_H_
#define _BITTLEET_MOCK_ARDUINO_STREAM_H_

#include <string>
#include <iostream>

class Stream {
public:
    Stream(const std::string& bytes);

    int available();
    int read();
    std::string readStringUntil(char terminator);

    std::string buffer;
};

extern Stream Serial;

# endif // _BITTLEET_MOCK_ARDUINO_STREAM_H_