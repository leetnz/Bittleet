//
// Bittle Skill
// Represents skills that Bittle can execute
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#ifndef _BITTLEET_SKILL_H_
#define _BITTLEET_SKILL_H_

#include <stdint.h>
#include "../Bittle.h"
#include "../command/Command.h"


namespace Skill {

enum class Type : uint8_t {
    Invalid = 0,
    Posture,
    Gait,
    Behaviour,
};

struct LoopSpec {
    uint8_t firstRow;
    uint8_t finalRow;
    uint8_t count;
};

struct Skill {
    Type type;
    uint8_t frames;
    int8_t nominalRoll;
    int8_t nominalPitch;
    bool doubleAngles;
    LoopSpec loopSpec;
    char * spec; // Interpretation depends on type
    uint16_t specLength;

    void clear();

    static Skill Empty(); // This is the safe method of creating a skill.
};

class Loader {
  public:
    virtual void load(const Command::Command& command, Skill& skill) = 0;
};

}

#endif // _BITTLEET_SKILL_H_