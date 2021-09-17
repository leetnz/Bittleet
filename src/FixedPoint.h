#ifndef _BITTLEET_FIXEDPOINT_H_
#define _BITTLEET_FIXEDPOINT_H_

#include <stdint.h>

template <typename T, int BITS>
class FixedPoint {
public:
    FixedPoint() = default;
    FixedPoint(float value) : _value(value * _denominator) {}

    float toF32() { return ((float)_value)/_denominator;}
private:
    T _value = 0;
    static constexpr float _denominator = (float)(1 << BITS);
};



#endif // _BITTLEET_FIXEDPOINT_H_
