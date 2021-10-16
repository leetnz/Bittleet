//
// Arduino Wire Mock
// Allows us to mock I2C
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//


#include "Wire.h"
#include <algorithm>


WireMock Wire = WireMock();

void WireMock::beginTransmission(int16_t address) {
    writeAddress = address;
}

int16_t WireMock::write(uint8_t byte) {
    writeBuffer.push_back(byte);
    return 1;
}

int16_t WireMock::endTransmission() {
    return 0;
}

int16_t WireMock::requestFrom(int16_t address, int16_t quantity) {
    requestedAddress = address;
    requestedQuantity = quantity;
    availableToRead = quantity; //std::min(quantity, (int16_t)((int32_t)readBuffer.size() - (int32_t)readIndex));
    return availableToRead;
}

int16_t WireMock::read() {
    if (availableToRead != 0) {
        availableToRead--;
        return readBuffer[readIndex++];
    }
    return -1;
}

int16_t WireMock::available() {
    if (availableToRead > 0) {
        return availableToRead;
    } else {
        return 0;
    }
}

