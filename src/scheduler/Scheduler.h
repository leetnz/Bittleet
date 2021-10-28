//
// Scheduler
// Schedules time-based actions in bittle... and sleeps alot!
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#ifndef _BITTLEET_SCHEDULER_H_
#define _BITTLEET_SCHEDULER_H_

#include <stdint.h>
#include <stdbool.h>

namespace Scheduler {

template <int NTasks>
class Scheduler {
  public:
    Scheduler() {
        for (int i = 0; i<NTasks; i++) {
            _periodUs[i] = 0xFFFFFFFF;
        }
    };

    int registerTask(uint32_t periodUs){
        if ((periodUs == 0) || (_registeredTasks >= NTasks) || (_firstRun == false)) {
            return -1;
        }
        _periodUs[_registeredTasks] = periodUs;
        return _registeredTasks++;
    }

    int waitUntilNextTask(){
        if (_firstRun){
            _handleFirstRun();
        }
        const int index = _findNextIndex();
        _waitUntilReady(index);
        return index;
    }

  protected:
    uint32_t _nextUpdateUs[NTasks];
    uint32_t _periodUs[NTasks];
    int _registeredTasks = 0;
    bool _firstRun = true;

    void _handleFirstRun() {
        uint32_t currentUs = micros();
        for (int i = 0; i<_registeredTasks; i++) {
            _nextUpdateUs[i] = currentUs;
        }
        _firstRun = false;
    }

    int _findNextIndex() {
        int index = -1;
        int32_t lowestDeltaUs = 2147483647;
        uint32_t currentUs = micros();
        for (int i = 0; i<_registeredTasks; i++){
            int32_t deltaUs = _nextUpdateUs[i] - currentUs;
            if (deltaUs < lowestDeltaUs) {
                index = i;
                lowestDeltaUs = deltaUs;
            }
        }
        return index;
    }

    void _waitUntilReady(int index) {
        int32_t deltaUs = _nextUpdateUs[index] - micros();
        if (deltaUs <= 0){
            if (deltaUs < - (int32_t)_periodUs[index]){
                _nextUpdateUs[index] = micros();
            }
        } else {
            while(deltaUs > 10000){
                delayMicroseconds(10000);
                deltaUs = _nextUpdateUs[index] - micros();
            }
            if (deltaUs > 0){
                delayMicroseconds(deltaUs);
            }
        }
        _nextUpdateUs[index] += _periodUs[index];
    }
};

}

#endif // _BITTLEET_SCHEDULER_H_