#include "LoaderEeprom.h"
#include "../Bittle.h"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <EEPROM.h>

#define DEVICE_ADDRESS 0x54
#define WIRE_BUFFER 30 //Arduino wire allows 32 byte buffer, with 2 byte for address.

#define NUM_SKILLS 31

#define LOOKUP_NAME_START_ADDR 200  // Onchip skills name start address.

  
static int lookupAddressByName(const char* skillName) {
    PTL(skillName);
    int skillAddressShift = 0;
    for (byte s = 0; s < NUM_SKILLS; s++) {
        byte nameLen = EEPROM.read(LOOKUP_NAME_START_ADDR + skillAddressShift++);
        char* readName = new char[nameLen + 1];
        for (byte l = 0; l < nameLen; l++) {
            readName[l] = EEPROM.read(LOOKUP_NAME_START_ADDR + skillAddressShift++);
        }
        readName[nameLen] = '\0';
        if (!strcmp(readName, skillName)) {
            delete[]readName;
            return LOOKUP_NAME_START_ADDR + skillAddressShift;
        }
        delete[]readName;
        skillAddressShift += 3;//1 byte type, 1 int address
    }
    PTLF("wrong key!");
    return -1;
}

#define BASE_HEADER (4)
#define EXTENDED_HEADER (3)
#define BEHAVIOR_SUFFIX (4)

static void loadFromEeprom(unsigned int &eeAddress, Skill& skill) {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)((eeAddress) >> 8));   // MSB
    Wire.write((int)((eeAddress) & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom((int)DEVICE_ADDRESS, BASE_HEADER);
    
    int8_t frameSpec = Wire.read();
    if (frameSpec == 1) {
        skill.type = Type::Posture;
        skill.frames = 1;
    } else if (frameSpec > 1) {
        skill.type = Type::Gait;
        skill.frames = frameSpec;
    } else if (frameSpec < 0) {
        skill.type = Type::Behaviour;
        skill.frames = -frameSpec;
    } else {
        PTLF("Invalid skill spec");
        // TODO: Load a default skill here...
        return;
    }

    skill.nominalRoll = (int8_t)Wire.read();
    skill.nominalPitch = (int8_t)Wire.read();
    skill.doubleAngles = (Wire.read() == 2) ? true : false;

    byte frameSize = DOF;
    switch (skill.type) {
        case Type::Behaviour:
            frameSize = DOF + BEHAVIOR_SUFFIX;
            break;
        case Type::Gait:
            frameSize = WALKING_DOF;
            break;
        case Type::Posture:
            frameSize = DOF;
            break;
    }
    if (skill.type == Type::Behaviour) {
        Wire.requestFrom(DEVICE_ADDRESS, EXTENDED_HEADER);
        skill.loopSpec.firstRow = Wire.read();
        skill.loopSpec.finalRow = Wire.read();
        skill.loopSpec.count = Wire.read();
    }

    int len = skill.frames * frameSize;

    skill.spec = new char[len];

    int readFromEE = 0;
    int readToWire = 0;
    while (len > 0) {
       Wire.requestFrom(DEVICE_ADDRESS, min(WIRE_BUFFER, len));
        readToWire = 0;
        do {
            if (Wire.available()) dutyAngles[readFromEE++] = Wire.read();
        } while (--len > 0 && ++readToWire < WIRE_BUFFER);
    }
}

static void loadDataByOnboardEepromAddress(int onBoardEepromAddress, Skill& skill) {
    char skillType = EEPROM.read(onBoardEepromAddress);
    unsigned int dataArrayAddress = EEPROMReadInt(onBoardEepromAddress + 1);
    delete[] dutyAngles;
    if (skillType == 'I') { //copy instinct data array from external i2c eeprom
        loadFromEeprom(dataArrayAddress);
    }
    else                    //copy newbility data array from progmem
    {
        // Too bad! TODO - add default skill here
    }
}


void LoaderEeprom::load(const Command::Command& command, Skill& skill) {
    int onBoardEepromAddress = -1;
    switch (command.type()) {
        case (Command::Type::Move): {
            Command::Move cmd;
            if (command.get(cmd)) {
                if (cmd.direction == Command::Direction::Forward) {
                    if (cmd.pace == Command::Pace::Slow) {
                    onBoardEepromAddress = lookupAddressByName("crF");
                    } else if (cmd.pace == Command::Pace::Medium) {
                    onBoardEepromAddress = lookupAddressByName("wkF");
                    } else if (cmd.pace == Command::Pace::Fast) {
                    onBoardEepromAddress = lookupAddressByName("trF");
                    }else if (cmd.pace == Command::Pace::Reverse) {
                    onBoardEepromAddress = lookupAddressByName("bk");
                    }
                } else if (cmd.direction == Command::Direction::Left) {
                    if (cmd.pace == Command::Pace::Slow) {
                    onBoardEepromAddress = lookupAddressByName("crL");
                    } else if (cmd.pace == Command::Pace::Medium) {
                    onBoardEepromAddress = lookupAddressByName("wkL");
                    } else if (cmd.pace == Command::Pace::Fast) {
                    onBoardEepromAddress = lookupAddressByName("trL");
                    }else if (cmd.pace == Command::Pace::Reverse) {
                    onBoardEepromAddress = lookupAddressByName("bkL");
                    }
                } else if (cmd.direction == Command::Direction::Right) {
                    if (cmd.pace == Command::Pace::Slow) {
                    onBoardEepromAddress = lookupAddressByName("crR");
                    } else if (cmd.pace == Command::Pace::Medium) {
                    onBoardEepromAddress = lookupAddressByName("wkR");
                    } else if (cmd.pace == Command::Pace::Fast) {
                    onBoardEepromAddress = lookupAddressByName("trR");
                    } else if (cmd.pace == Command::Pace::Reverse) {
                    onBoardEepromAddress = lookupAddressByName("bkR");
                    }
                }
            }
            break;
        }
        case (Command::Type::Simple): {
            Command::Simple cmd;
            if (command.get(cmd)) {
            switch (cmd) {
                case (Command::Simple::Rest):                     onBoardEepromAddress = lookupAddressByName("rest"); break;
                case (Command::Simple::Balance):                  onBoardEepromAddress = lookupAddressByName("balance"); break;
                case (Command::Simple::Step):                     onBoardEepromAddress = lookupAddressByName("vt"); break;
                case (Command::Simple::Sit):                      onBoardEepromAddress = lookupAddressByName("sit"); break;
                case (Command::Simple::Stretch):                  onBoardEepromAddress = lookupAddressByName("str"); break;
                case (Command::Simple::Greet):                    onBoardEepromAddress = lookupAddressByName("hi"); break;
                case (Command::Simple::Pushup):                   onBoardEepromAddress = lookupAddressByName("pu"); break;
                case (Command::Simple::Hydrant):                  onBoardEepromAddress = lookupAddressByName("pee"); break;
                case (Command::Simple::Check):                    onBoardEepromAddress = lookupAddressByName("ck"); break;
                case (Command::Simple::Dead):                     onBoardEepromAddress = lookupAddressByName("pd"); break;
                case (Command::Simple::Zero):                     onBoardEepromAddress = lookupAddressByName("zero"); break;
                case (Command::Simple::Lifted):                   onBoardEepromAddress = lookupAddressByName("lifted"); break;
                case (Command::Simple::Dropped):                  onBoardEepromAddress = lookupAddressByName("dropped"); break;
                case (Command::Simple::Recover):                  onBoardEepromAddress = lookupAddressByName("rc"); break;
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
                    onBoardEepromAddress = lookupAddressByName("calib");
                    break;
                }
            }
            break;
        }
    }
    if (onBoardEepromAddress == -1) {
        return;
    }
    loadDataByOnboardEepromAddress(onBoardEepromAddress, skill);
}

} // namespace Skill