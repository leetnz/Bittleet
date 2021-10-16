//
// Arduino Wire Mock
// Allows us to mock I2C
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#ifndef _BITTLEET_MOCK_ARDUINO_WIRE_H_
#define _BITTLEET_MOCK_ARDUINO_WIRE_H_

#include <stdint.h>
#include <vector>

class WireMock {
public:
    WireMock() = default;

    void beginTransmission(int16_t address);
    int16_t write(uint8_t byte);
    int16_t endTransmission();
    int16_t requestFrom(int16_t address, int16_t quantity);
    int16_t read();
    int16_t available();

    int16_t writeAddress = -1;
    std::vector<uint8_t> writeBuffer;

    int16_t requestedAddress = -1;
    int16_t requestedQuantity = -1;
    std::vector<int8_t> readBuffer;
    size_t readIndex = 0;
    size_t availableToRead = 0;
};

extern WireMock Wire;

#endif // _BITTLEET_MOCK_ARDUINO_WIRE_H_