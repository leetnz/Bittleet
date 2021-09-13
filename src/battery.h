#ifndef _BITTLEET_BATTERY_H_
#define  _BITTLEET_BATTERY_H_

enum class BatteryState_t{
    None = 0,
    Low,
    Ok
};

BatteryState_t batteryState(int adcRead);

#endif // _BITTLEET_BATTERY_H_
