//
// Arduino EEPROM Mock
// Allows us to mock onchip EEPROM
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//


#include "EEPROM.h"

EEPROMMock EEPROM = EEPROMMock();

int16_t EEPROMMock::read(int16_t address) const {
    return (address < 1024) ? data[address] : -1;
}

