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

namespace Scheduler {

template <int NTasks>
class Scheduler {
  public:
    Scheduler() {
        for (int i = 0; i<NTasks; i++) {
            _lastUpdateUs[i] = 0;
            _periodUs[i] = 0xFFFFFFFF;
        }
    };

    int registerTask(uint32_t periodUs){
        if ((periodUs == 0) || (registeredTasks >= NTasks)) {
            return -1;
        }
        _periodUs[registeredTasks] = periodUs;
        return registeredTasks++;
    }
    int waitUntilNextTask(){
        return 0;
    }

  protected:
    uint32_t _lastUpdateUs[NTasks];
    uint32_t _periodUs[NTasks];
    int registeredTasks = 0;
};

}

#endif // _BITTLEET_SCHEDULER_H_