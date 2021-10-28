#include "Arduino.h"

uint32_t TimeMock::currentUs = 0;
uint16_t TimeMock::lastDelayUs = 0;
uint32_t TimeMock::totalDelayUs = 0;

void TimeMock::reset(){
    currentUs = 0;
    lastDelayUs = 0;
    totalDelayUs = 0; 
}

uint32_t micros(){
    return TimeMock::currentUs;
}

void delayMicroseconds(uint16_t us){
    TimeMock::lastDelayUs = us;
    TimeMock::totalDelayUs += us;
    TimeMock::currentUs += us;
}