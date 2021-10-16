#include "LoaderEeprom.h"
#include "../Bittle.h"

#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>

#define PT(s) Serial.print(s)  //makes life easier
#define PTL(s) Serial.println(s)
#define PTF(s) Serial.print(F(s))//trade flash memory for dynamic memory with F() function
#define PTLF(s) Serial.println(F(s))

#define DEVICE_ADDRESS 0x54
#define WIRE_BUFFER 30 //Arduino wire allows 32 byte buffer, with 2 byte for address.

#define NUM_SKILLS 31

#define LOOKUP_NAME_START_ADDR 200  // On chip skills name start address.

namespace Skill {

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
static int EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

  
int16_t LoaderEeprom::_lookupAddressByName(const char* skillName) {
    PTL(skillName);
    int skillAddressShift = 0;
    int expectedLen = strlen(skillName);
    for (byte s = 0; s < NUM_SKILLS; s++) {
        byte nameLen = EEPROM.read(LOOKUP_NAME_START_ADDR + skillAddressShift++);
        if (nameLen != expectedLen)
        {
            skillAddressShift += nameLen;
        } else {
            char* readName = new char[nameLen + 1];
            for (byte l = 0; l < nameLen; l++) {
                readName[l] = EEPROM.read(LOOKUP_NAME_START_ADDR + skillAddressShift++);
            }
            readName[nameLen] = '\0';
            if (!strcmp(readName, skillName)) {
                delete[]readName;
                char skillType = EEPROM.read(LOOKUP_NAME_START_ADDR + skillAddressShift);
                if (skillType != 'I') { 
                    return -1; // Can't load!
                }
                return EEPROMReadInt(LOOKUP_NAME_START_ADDR + skillAddressShift + 1);
            }
            delete[]readName;
        }
        
        skillAddressShift += 3;//1 byte type, 1 int address
    }
    PTLF("wrong key!");
    return -1;
}

#define BASE_HEADER (4)
#define EXTENDED_HEADER (3)
#define BEHAVIOR_SUFFIX (4)

void LoaderEeprom::_loadFromAddress(uint16_t address, Skill& skill) {
    if (skill.spec != NULL) {
        delete[] skill.spec;
    }
    skill = Skill::Empty();

    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)((address) >> 8));   // MSB
    Wire.write((int)((address) & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom((int)DEVICE_ADDRESS, BASE_HEADER);
    
    int8_t frameSpec = Wire.read();
    int16_t frameSize = DOF;
    if (frameSpec == 1) {
        skill.type = Type::Posture;
        skill.frames = 1;
        frameSize = DOF;
    } else if (frameSpec > 1) {
        skill.type = Type::Gait;
        skill.frames = frameSpec;
        frameSize = WALKING_DOF;
    } else if (frameSpec < 0) {
        skill.type = Type::Behaviour;
        skill.frames = -frameSpec;
        frameSize = DOF + BEHAVIOR_SUFFIX;
    } else {
        PTLF("Invalid skill spec");
        
        return;
    }

    skill.nominalRoll = (int8_t)Wire.read();
    skill.nominalPitch = (int8_t)Wire.read();
    skill.doubleAngles = (Wire.read() == 2) ? true : false;

    if (skill.type == Type::Behaviour) {
        Wire.requestFrom(DEVICE_ADDRESS, EXTENDED_HEADER);
        skill.loopSpec.firstRow = Wire.read();
        skill.loopSpec.finalRow = Wire.read();
        skill.loopSpec.count = Wire.read();
    }

    skill.specLength = (uint16_t)skill.frames * frameSize;
    skill.spec = new char[skill.specLength];

    int index = 0;
    int len = skill.specLength;
    while (len > 0) {
        Wire.requestFrom(DEVICE_ADDRESS, min(WIRE_BUFFER, len));
        while (Wire.available() && (len > 0)) {
            skill.spec[index++] = Wire.read();
            len--;
        }
    }
}



void LoaderEeprom::load(const Command::Command& command, Skill& skill) {
    int address = -1;
    switch (command.type()) {
        case (Command::Type::Move): {
            Command::Move cmd;
            if (command.get(cmd)) {
                if (cmd.direction == Command::Direction::Forward) {
                    if (cmd.pace == Command::Pace::Slow) {
                        address = _lookupAddressByName("crF");
                    } else if (cmd.pace == Command::Pace::Medium) {
                        address = _lookupAddressByName("wkF");
                    } else if (cmd.pace == Command::Pace::Fast) {
                        address = _lookupAddressByName("trF");
                    }else if (cmd.pace == Command::Pace::Reverse) {
                        address = _lookupAddressByName("bk");
                    }
                } else if (cmd.direction == Command::Direction::Left) {
                    if (cmd.pace == Command::Pace::Slow) {
                        address = _lookupAddressByName("crL");
                    } else if (cmd.pace == Command::Pace::Medium) {
                        address = _lookupAddressByName("wkL");
                    } else if (cmd.pace == Command::Pace::Fast) {
                        address = _lookupAddressByName("trL");
                    }else if (cmd.pace == Command::Pace::Reverse) {
                        address = _lookupAddressByName("bkL");
                    }
                } else if (cmd.direction == Command::Direction::Right) {
                    if (cmd.pace == Command::Pace::Slow) {
                        address = _lookupAddressByName("crR");
                    } else if (cmd.pace == Command::Pace::Medium) {
                        address = _lookupAddressByName("wkR");
                    } else if (cmd.pace == Command::Pace::Fast) {
                        address = _lookupAddressByName("trR");
                    } else if (cmd.pace == Command::Pace::Reverse) {
                        address = _lookupAddressByName("bkR");
                    }
                }
            }
            break;
        }
        case (Command::Type::Simple): {
            Command::Simple cmd;
            if (command.get(cmd)) {
            switch (cmd) {
                case (Command::Simple::Rest):                     address = _lookupAddressByName("rest"); break;
                case (Command::Simple::Balance):                  address = _lookupAddressByName("balance"); break;
                case (Command::Simple::Step):                     address = _lookupAddressByName("vt"); break;
                case (Command::Simple::Sit):                      address = _lookupAddressByName("sit"); break;
                case (Command::Simple::Stretch):                  address = _lookupAddressByName("str"); break;
                case (Command::Simple::Greet):                    address = _lookupAddressByName("hi"); break;
                case (Command::Simple::Pushup):                   address = _lookupAddressByName("pu"); break;
                case (Command::Simple::Hydrant):                  address = _lookupAddressByName("pee"); break;
                case (Command::Simple::Check):                    address = _lookupAddressByName("ck"); break;
                case (Command::Simple::Dead):                     address = _lookupAddressByName("pd"); break;
                case (Command::Simple::Zero):                     address = _lookupAddressByName("zero"); break;
                case (Command::Simple::Lifted):                   address = _lookupAddressByName("lifted"); break;
                case (Command::Simple::Dropped):                  address = _lookupAddressByName("dropped"); break;
                case (Command::Simple::Recover):                  address = _lookupAddressByName("rc"); break;
                case (Command::Simple::GyroToggle):               
                case (Command::Simple::SaveServoCalibration):
                case (Command::Simple::AbortServoCalibration):
                case (Command::Simple::ShowJointAngles):
                case (Command::Simple::Pause):
                default:
                break;
            }
            }
            break;
        }
        case (Command::Type::WithArgs): {
            Command::WithArgs cmd;
            if (command.get(cmd)) {
                if (cmd.cmd == Command::ArgType::Calibrate) {
                    address = _lookupAddressByName("calib");
                    break;
                }
            }
            break;
        }
        case (Command::Type::None): {
            return; // TODO: Load default skill
        }
    }
    if (address == -1) {
        return;  // TODO: Load default skill
    }
    _loadFromAddress(address, skill);
}

} // namespace Skill