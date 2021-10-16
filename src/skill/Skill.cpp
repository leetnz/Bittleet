//
// Bittle Skill
// Represents skills that Bittle can execute
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//


#include "Skill.h"

namespace Skill {

Skill Skill::Empty() {
    return Skill {
        .type = Type::Invalid,
        .frames = 0,
        .nominalRoll = 0,
        .nominalPitch = 0,
        .doubleAngles = false,
        .loopSpec = LoopSpec{
            .firstRow = 0,
            .finalRow = 0,
            .count = 0,
        },
        .spec = NULL,
        .specLength = 0, 
    };
}


} // namespace Skill