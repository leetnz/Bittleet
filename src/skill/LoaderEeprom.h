#ifndef _BITTLEET_SKILL_LOADER_EEPROM_H_
#define _BITTLEET_SKILL_LOADER_EEPROM_H_

#include "Skill.h"


namespace Skill {

class LoaderEeprom : public Loader {
  public:
    LoaderEeprom() {};

    void load(const Command::Command& command, Skill& skill);  
};

#endif // _BITTLEET_SKILL_LOADER_EEPROM_H_