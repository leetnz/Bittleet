//
// Arduino EEPROM Mock
// Allows us to mock onchip EEPROM
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#ifndef _BITTLEET_MOCK_ARDUINO_EEPROM_H_
#define _BITTLEET_MOCK_ARDUINO_EEPROM_H_

#include <stdint.h>
#include <vector>

class EEPROMMock {
public:
    EEPROMMock() = default;

    int16_t read(int16_t address) const;

    std::vector<int8_t> data = std::vector<int8_t>(1024, 0x00);
};

extern EEPROMMock EEPROM;

#endif // _BITTLEET_MOCK_ARDUINO_EEPROM_H_