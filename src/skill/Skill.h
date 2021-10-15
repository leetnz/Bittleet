#ifndef _BITTLEET_SKILL_H_
#define _BITTLEET_SKILL_H_

#include <stdint.h>
#include "../Bittle.h"
#include "../Command/Command.h"


namespace Skill {

enum class Type : uint8_t {
    Posture = 0,
    Gait,
    Behaviour,
};

struct LoopSpec {
    uint8_t firstRow;
    uint8_t finalRow;
    uint8_t count;
};

struct Skill {
    Type type = Type::Posture;
    uint8_t frames = 0;
    int8_t nominalRoll = 0;
    int8_t nominalPitch = 0;
    bool doubleAngles = false;
    LoopSpec loopSpec;
    char * spec; // Interpretation depends on type
};

class Loader {
  public:
    virtual void load(const Command::Command& command, Skill& skill) = 0;
};

}

#endif // _BITTLEET_SKILL_H_