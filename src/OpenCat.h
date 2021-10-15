/*
    Skill class holds only the lookup information of joint angles.
    One frame of joint angles defines a static posture, while a series of frames defines a periodic motion, usually a gait.
    Skills are instantiated as either:
      instinct  (trained by Rongzhong Li, saved in external i2c EERPOM) or
      newbility (taught by other users, saved in PROGMEM)
    A well-tuned (finalized) newbility can also be saved in external i2c EEPROM. Remember that EEPROM has very limited (1,000,000) write cycles!

    SkillList (inherit from QList class) holds a mixture of instincts and newbilities.
    It also provides a dict(key) function to return the pointer to the skill.
    Initialization information(individual skill name, address) for SkillList is stored in on-board EEPROM

    Behavior list (inherit from QList class) holds a time dependent sequence of multiple skills, triggered by certain perceptions.
    It defines the order, speed, repetition and interval of skills。
    (Behavior list is yet to be implemented)

    Motion class uses the lookup information of a Skill to construct a Motion object that holds the actual angle array.
    It also implements the reading and writing functions in specific storage locations.
    Considering Arduino's limited SRAM, you should create only one Motion object and update it when loading new skills.

    instinct(external EEPROM) \
                                -- skill that contains only lookup information
    newbility(progmem)        /

    Skill list: skill1, skill2, skill3,...
                              |
                              v
                           motion that holds actual joint angle array in SRAM

    Behavior list: skill3(speed, repetition and interval), skill1(speed, repetition and interval), ...

    **
    Updates: One Skill object in the SkillList takes around 20 bytes in SRAM. It takes 200+ bytes for 15+ skills.
    On a tiny atmega328 chip with only 2KB SRAM, I'm implementing the Skills and SkillList in the on-board EEPROM。
    Now the skill list starts from on-board EEPROM address SKILLS.
    Format:
    1 byte skill_1 nameLength + char string name1 + 1 char skillType1 + 1 int address1,
    1 byte skill_2 nameLength + char string name2 + 1 char skillType2 + 1 int address2,
    ...
    The iterator can traverse the list with the string length of each skill name.

    The Skill and SkillList classes are obsolete in the atmega328 implementation but are still included in this header file.
    **

  Rongzhong Li
  January 2021

  Copyright (c) 2021 Petoi LLC.

  The MIT License

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/
#define I2C_EEPROM //comment this line out if you don't have an I2C EEPROM in your DIY board. 

//postures and movements trained by RongzhongLi
#include<Arduino.h>
#include "Bittle.h" //activate the correct header file according to your model
#include "command/Command.h" //activate the correct header file according to your model
#include "math/trig.h"
#include "math/FixedPoint.h"

#define NyBoard_V1_0


// credit to Adafruit PWM servo driver library
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <EEPROM.h>
//#include <avr/eeprom.h> // doesn't work. <EEPROM.h> works

//abbreviations
#define PT(s) Serial.print(s)  //makes life easier
#define PTL(s) Serial.println(s)
#define PTF(s) Serial.print(F(s))//trade flash memory for dynamic memory with F() function
#define PTLF(s) Serial.println(F(s))

//board configuration
#define INTERRUPT 0
#define IR_RECEIVER 4 // Signal Pin of IR receiver to Arduino Digital Pin 4
#define BUZZER 5
#define GYRO

void beep(int8_t note, float duration = 10, int pause = 0, byte repeat = 1 );
void playMelody(int start);

void meow(int repeat = 0, int pause = 200, int startF = 50,  int endF = 200, int increment = 5);






#define BATT A7
#define DEVICE_ADDRESS 0x54


#ifdef PIXEL_PIN
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 7
#define LIT_ON 30
Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif

#define HEAD
#define LL_LEG
#define P1S
//#define MPU_YAW180

//on-board EEPROM addresses
#define MELODY 1023 //melody will be saved at the end of the 1KB EEPROM, and is read reversely. That allows some flexibility on the melody length. 
#define PIN 0                 // 16 byte array
#define CALIB 16              // 16 byte array
#define MID_SHIFT 32          // 16 byte array
#define ROTATION_DIRECTION 48 // 16 byte array
#define SERVO_ANGLE_RANGE 64  // 16 byte array
#define MPUCALIB 80           // 16 byte array
#define FAST 96               // 16 byte array
#define SLOW 112              // 16 byte array
#define LEFT 128              // 16 byte array
#define RIGHT 144             // 16 byte array

#define ADAPT_PARAM 160          // 16 x NUM_ADAPT_PARAM byte array
#define NUM_ADAPT_PARAM  2    // number of parameters for adaption
#define SKILLS 200         // 1 byte for skill name length, followed by the char array for skill name
// then followed by i(nstinct) on progmem, or n(ewbility) on progmem

#define INITIAL_SKILL_DATA_ADDRESS 0 //the actual data is stored on the I2C EEPROM. 
//the first 1000 bytes are reserved for transferring
//the above constants from onboard EEPROM to I2C EEPROM

//servo constants
#define PWM_FACTOR 4
#define MG92B_MIN 170*PWM_FACTOR
#define MG92B_MAX 550*PWM_FACTOR
#define MG92B_RANGE 150

#define MG90D_MIN 158*PWM_FACTOR //so mg92b and mg90 are not centered at the same signal
#define MG90D_MAX 515*PWM_FACTOR
#define MG90D_RANGE 150

#define P1S_MIN 180*PWM_FACTOR
#define P1S_MAX 620*PWM_FACTOR
#define P1S_RANGE 250

// called this way, it uses the default address 0x40
extern Adafruit_PWMServoDriver pwm;
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#ifdef P1S
#define SERVOMIN  P1S_MIN // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  P1S_MAX // this is the 'maximum' pulse length count (out of 4096)
#define SERVO_ANG_RANGE P1S_RANGE
#else
#define SERVOMIN  MG92B_MIN // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  MG92B_MAX // this is the 'maximum' pulse length count (out of 4096)
#define SERVO_ANG_RANGE MG92B_RANGE
#endif

#define PWM_RANGE (SERVOMAX - SERVOMIN)

typedef FixedPoint<int16_t, 8> AdjustAngle;
typedef FixedPoint<uint8_t, 0> ServoRange;

extern ServoRange servoRange[DOF];
extern int16_t currentAng[DOF];
extern AdjustAngle currentAdjust[DOF];
extern int calibratedDuty0[DOF];

class Motion;
extern Motion motion;

extern float rollDeviation;
extern float pitchDeviation;


//--------------------

float pulsePerDegreeF(int i);

//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value);

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
int EEPROMReadInt(int p_address);

#define WIRE_BUFFER 30 //Arduino wire allows 32 byte buffer, with 2 byte for address.
#define WIRE_LIMIT 16 //That leaves 30 bytes for data. use 16 to balance each writes
#define PAGE_LIMIT 32 //AT24C32D 32-byte Page Write Mode. Partial Page Writes Allowed
#define EEPROM_SIZE (65536/8)

#define NUM_SKILLS 31



void copyDataFromPgmToI2cEeprom(unsigned int &eeAddress, unsigned int pgmAddress);

class Motion {
  public:
    int8_t period;            //the period of a skill. 1 for posture, >1 for gait, <-1 for behavior
    int expectedRollPitch[2]; //expected body orientation (roll, pitch)
    byte angleDataRatio;      //divide large angles by 1 or 2. if the max angle of a skill is >128, all the angls will be divided by 2
    byte loopCycle[3];        //the looping section of a behavior (starting row, ending row, repeating cycles)
    char* dutyAngles;         //the data array for skill angles and parameters
    Motion() {
      period = 0;
      expectedRollPitch[0] = 0;
      expectedRollPitch[1] = 0;
      dutyAngles = NULL;
    }

    int lookupAddressByName(const char* skillName) {
      PTL(skillName);
      int skillAddressShift = 0;
      for (byte s = 0; s < NUM_SKILLS; s++) {//save skill info to on-board EEPROM, load skills to SkillList
        byte nameLen = EEPROM.read(SKILLS + skillAddressShift++);
        char* readName = new char[nameLen + 1];
        for (byte l = 0; l < nameLen; l++) {
          readName[l] = EEPROM.read(SKILLS + skillAddressShift++);
        }
        readName[nameLen] = '\0';
        if (!strcmp(readName, skillName)) {
          delete[]readName;
          return SKILLS + skillAddressShift;
        }
        delete[]readName;
        skillAddressShift += 3;//1 byte type, 1 int address
      }
      PTLF("wrong key!");
      return -1;
    }
    void loadDataFromProgmem(unsigned int pgmAddress) {
      period = pgm_read_byte(pgmAddress);//automatically cast to char*
      for (int i = 0; i < 2; i++)
        expectedRollPitch[i] = (int8_t)pgm_read_byte(pgmAddress + 1 + i);
      angleDataRatio = pgm_read_byte(pgmAddress + 3);
      byte skillHeader = 4;
      byte frameSize;
      if (period < -1) {
        frameSize = 20;
        for (byte i = 0; i < 3; i++)
          loopCycle[i] = pgm_read_byte(pgmAddress + skillHeader + i);
        skillHeader = 7;
      }
      else
        frameSize = period > 1 ? WALKING_DOF : 16;
      int len = abs(period) * frameSize;
      //delete []dutyAngles; //check here
      dutyAngles = new char[len];
      for (int k = 0; k < len; k++) {
        dutyAngles[k] = pgm_read_byte(pgmAddress + skillHeader + k);
      }
    }
    void loadDataFromI2cEeprom(unsigned int &eeAddress) {
      Wire.beginTransmission(DEVICE_ADDRESS);
      Wire.write((int)((eeAddress) >> 8));   // MSB
      Wire.write((int)((eeAddress) & 0xFF)); // LSB
      Wire.endTransmission();
      byte skillHeader = 4;
      Wire.requestFrom((int)DEVICE_ADDRESS, (int)skillHeader);
      period = Wire.read();
      //PTL("read " + String(period) + " frames");
      for (int i = 0; i < 2; i++)
        expectedRollPitch[i] = (int8_t)Wire.read();
      angleDataRatio = Wire.read();

      byte frameSize;
      if (period < -1) {
        skillHeader = 7;
        frameSize = 20;
        Wire.requestFrom(DEVICE_ADDRESS, 3);
        for (byte i = 0; i < 3; i++)
          loopCycle[i] = Wire.read();
      }
      else
        frameSize = period > 1 ? WALKING_DOF : 16;
      int len = abs(period) * frameSize;
      //delete []dutyAngles;//check here

      dutyAngles = new char[len];

      int readFromEE = 0;
      int readToWire = 0;
      while (len > 0) {
        //PTL("request " + String(min(WIRE_BUFFER, len)));
        Wire.requestFrom(DEVICE_ADDRESS, min(WIRE_BUFFER, len));
        readToWire = 0;
        do {
          if (Wire.available()) dutyAngles[readFromEE++] = Wire.read();
          /*PT( (int8_t)dutyAngles[readFromEE - 1]);
            PT('\t')*/
        } while (--len > 0 && ++readToWire < WIRE_BUFFER);
        //PTL();
      }
      //PTLF("finish reading");
    }

    void loadDataByOnboardEepromAddress(int onBoardEepromAddress) {
      char skillType = EEPROM.read(onBoardEepromAddress);
      unsigned int dataArrayAddress = EEPROMReadInt(onBoardEepromAddress + 1);
      delete[] dutyAngles;
#ifdef DEVELOPER
      PTF("free memory: ");
      PTL(freeMemory());
#endif
#ifdef I2C_EEPROM
      if (skillType == 'I') { //copy instinct data array from external i2c eeprom
        loadDataFromI2cEeprom(dataArrayAddress);
      }
      else                    //copy newbility data array from progmem
#endif
      {
        loadDataFromProgmem(dataArrayAddress);
      }
#ifdef DEVELOPER
      PTF("free memory: ");
      PTL(freeMemory());
#endif
    }

    void loadBySkillName(char* skillName) {//get lookup information from on-board EEPROM and read the data array from storage
      int onBoardEepromAddress = lookupAddressByName(skillName);
      if (onBoardEepromAddress == -1)
        return;
      loadDataByOnboardEepromAddress(onBoardEepromAddress);
    }

    void loadByCommand(Command::Command& command) {//get lookup information from on-board EEPROM and read the data array from storage
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
      loadDataByOnboardEepromAddress(onBoardEepromAddress);
    }
};



void assignSkillAddressToOnboardEeprom();

inline byte pin(byte idx) {
  return EEPROM.read(PIN + idx);
}
inline byte remapPin(byte offset, byte idx) {
  return EEPROM.read(offset + idx);
}
inline byte servoAngleRange(byte idx) {
  return EEPROM.read(SERVO_ANGLE_RANGE + idx);
}
inline int8_t middleShift(byte idx) {
  return EEPROM.read( MID_SHIFT + idx);
}

inline int8_t rotationDirection(byte idx) {
  return EEPROM.read(ROTATION_DIRECTION + idx);
}
inline int8_t servoCalib(byte idx) {
  return EEPROM.read( CALIB + idx);
}

// balancing parameters
#define ROLL_LEVEL_TOLERANCE 0.25
#define PITCH_LEVEL_TOLERANCE 0.25

#define LARGE_ROLL 90
#define LARGE_PITCH 75

//the following coefficients will be divided by M_RAD2DEG in the adjust() function. so (float) 0.1 can be saved as (int8_t) 1
//this trick allows using int8_t array insead of float array, saving 96 bytes and allows storage on EEPROM
#define panF 60
#define tiltF 60
#define sRF 50    //shoulder roll factor
#define sPF 12    //shoulder pitch factor
#define uRF 60    //upper leg roll factor
#define uPF 30    //upper leg pitch factor
#define lRF (-1.5*uRF)  //lower leg roll factor 
#define lPF (-1.5*uPF)  //lower leg pitch factor
#define LEFT_RIGHT_FACTOR 1.2
#define FRONT_BACK_FACTOR 1.2
#define POSTURE_WALKING_FACTOR 0.5
extern float postureOrWalkingFactor;


inline int8_t adaptiveCoefficient(byte idx, byte para) {
  return EEPROM.read(ADAPT_PARAM + idx * NUM_ADAPT_PARAM + para);
}

float adjust(byte i);

void saveCalib(int8_t *var);

void calibratedPWM(byte i, float angle, float speedRatio = 0);

void allCalibratedPWM(char * dutyAng, byte offset = 0);

void shutServos();

template <typename T> void moveToPose( T * target, byte angleDataRatio = 1, float degreesPerStep = 1, byte offset = 0) {
  if (degreesPerStep == 0) { // No speed limiting
    allCalibratedPWM(target, 8);
  } else {
    int *diff = new int [DOF - offset], maxDiff = 0;
    for (byte i = offset; i < DOF; i++) {
      diff[i - offset] =   currentAng[i] - target[i - offset] * angleDataRatio;
      maxDiff = max(maxDiff, abs( diff[i - offset]));
    }

    byte steps = byte(round(maxDiff / degreesPerStep));

    for (byte s = 0; s <= steps; s++) {
      for (byte i = offset; i < DOF; i++) {
        float dutyAng = (target[i - offset] * angleDataRatio + (steps == 0 ? 0 : (1 + cos(M_PI * s / steps)) / 2 * diff[i - offset]));
        calibratedPWM(i,  dutyAng);
      }
    }
    delete [] diff;
  }
}

template <typename T> void transform( T * target, byte angleDataRatio = 1, float speedRatio = 1, byte offset = 0) {
  if (speedRatio == 0) { // No speed limiting
    allCalibratedPWM(target, 8);
  } else {
    int *diff = new int [DOF - offset], maxDiff = 0;
    for (byte i = offset; i < DOF; i++) {
      diff[i - offset] =   currentAng[i] - target[i - offset] * angleDataRatio;
      maxDiff = max(maxDiff, abs( diff[i - offset]));
    }

    byte steps = byte(round(maxDiff / 1.0/*degreeStep*/ / speedRatio));//default speed is 1 degree per step

    for (byte s = 0; s <= steps; s++) {
      for (byte i = offset; i < DOF; i++) {
        float dutyAng = (target[i - offset] * angleDataRatio + (steps == 0 ? 0 : (1 + cos(M_PI * s / steps)) / 2 * diff[i - offset]));
        calibratedPWM(i,  dutyAng);
      }
    }
    delete [] diff;
  }
}


void skillByCommand(Command::Command& cmd, byte angleDataRatio = 1, float speedRatio = 1, bool shutServoAfterward = true);



//short tools

template <typename T> void allCalibratedPWM(T * dutyAng, byte offset = 0) {
  for (int8_t i = DOF - 1; i >= offset; i--) {
    calibratedPWM(i, dutyAng[i]);
  }
}

template <typename T> int8_t sign(T val) {
  return (T(0) < val) - (val < T(0));
}

void printRange(int r0 = 0, int r1 = 0);

template <typename T> void printList(T * arr, byte len = DOF) {
  String temp = "";
  for (byte i = 0; i < len; i++) {
    temp += String(int(arr[i]));
    temp += ",\t";
    //PT((T)(arr[i]));
    //PT('\t');
  }
  PTL(temp);
}
template <typename T> void printEEPROMList(int EEaddress, byte len = DOF) {
  for (byte i = 0; i < len; i++) {
    PT((T)(EEPROM.read(EEaddress + i)));
    PT('\t');
  }
  PTL();
}
char getUserInput();


bool sensorConnectedQ(int n);

int SoundLightSensorPattern(char *cmd);
