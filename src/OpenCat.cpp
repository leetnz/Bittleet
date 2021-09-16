/*
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

#include "OpenCat.h"

// credit to Adafruit PWM servo driver library
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <EEPROM.h>

// Local variables


static bool restQ = false;



// Dirty globals 

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

float pulsePerDegree[DOF] = {};
int currentAng[DOF] = {};
float currentAdjust[DOF] = {};
int calibratedDuty0[DOF] = {};

char token;

float postureOrWalkingFactor;

float RollPitchDeviation[2];
int8_t ramp = 1;

Motion motion;

int lightLag = 0;





void beep(int8_t note, float duration, int pause, byte repeat) {
  if (note == 0) {//rest note
    analogWrite(BUZZER, 0);
    delay(duration);
    return;
  }
  int freq = 220 * pow(1.059463, note - 1); // 1.059463 comes from https://en.wikipedia.org/wiki/Twelfth_root_of_two
  float period = 1000000.0 / freq;
  for (byte r = 0; r < repeat; r++) {
    for (float t = 0; t < duration * 1000; t += period) {
      analogWrite(BUZZER, 150);      // Almost any value can be used except 0 and 255
      // experiment to get the best tone
      delayMicroseconds(period / 2);        // rise for half period
      analogWrite(BUZZER, 0);       // 0 turns it off
      delayMicroseconds(period / 2);        // down for half period
    }
    delay(pause);
  }
}
void playMelody(int start) {
  byte len = (byte)EEPROM.read(start) / 2;
  for (int i = 0; i < len; i++)
    beep(EEPROM.read(start - 1 - i), 1000 / EEPROM.read(start - 1 - len - i), 100);
}

void meow(int repeat, int pause, int startF, int endF, int increment) {
  for (int r = 0; r < repeat + 1; r++) {
    for (int amp = startF; amp <= endF; amp += increment) {
      analogWrite(BUZZER, amp);
      delay(15); // wait for 15 milliseconds to allow the buzzer to vibrate
    }
    delay(100 + 500 / increment);
    analogWrite(BUZZER, 0);
    if (repeat)delay(pause);
  }
}

//--------------------

//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);
  EEPROM.update(p_address, lowByte);
  EEPROM.update(p_address + 1, highByte);
}

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
int EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

void copyDataFromPgmToI2cEeprom(unsigned int &eeAddress, unsigned int pgmAddress) {
  int8_t period = pgm_read_byte(pgmAddress);//automatically cast to char*
  byte skillHeader = 4;
  byte frameSize;
  if (period < -1) {
    skillHeader = 7; //rows, roll, tilt, loopStart, loopEnd, loopNumber, angle ratio <1,2>
    //(if the angles are larger than 128, they will be divided by angle ratio)
    frameSize = 20;
  }
  else
    frameSize = period > 1 ? WALKING_DOF : 16;
  int len = abs(period) * frameSize + skillHeader;
  int writtenToEE = INITIAL_SKILL_DATA_ADDRESS;
  while (len > 0) {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)((eeAddress) >> 8));   // MSB
    Wire.write((int)((eeAddress) & 0xFF)); // LSB
    /*PTF("\n* current address: ");
      PTL((unsigned int)eeAddress);
      PTLF("0\t1\t2\t3\t4\t5\t6\t7\t8\t9\ta\tb\tc\td\te\tf\t\n\t\t\t");*/
    byte writtenToWire = 0;
    do {
      if (eeAddress == EEPROM_SIZE) {
        PTL();
        PTL("I2C EEPROM overflow! You must reduce the size of your instincts file!\n");
#ifdef BUZZER
        meow(3);
#endif
        return;
      }
      /*PT((int8_t)pgm_read_byte(pgmAddress + writtenToEE));
        PT("\t");*/
      Wire.write((byte)pgm_read_byte(pgmAddress + writtenToEE++));
      writtenToWire++;
      eeAddress++;
    } while ((--len > 0 ) && (eeAddress  % PAGE_LIMIT ) && (writtenToWire < WIRE_LIMIT));//be careful with the chained conditions
    //self-increment may not work as expected
    Wire.endTransmission();
    delay(6);  // needs 5ms for page write
    //PTL("\nwrote " + String(writtenToWire) + " bytes.");
  }
  //PTLF("finish copying to I2C EEPROM");
}


// class Motion {
//   public:
//     byte pins[DOF];           //the mapping between PCB pins and the joint indexes
//     int8_t period;            //the period of a skill. 1 for posture, >1 for gait, <-1 for behavior
//     int expectedRollPitch[2]; //expected body orientation (roll, pitch)
//     byte angleDataRatio;      //divide large angles by 1 or 2. if the max angle of a skill is >128, all the angls will be divided by 2
//     byte loopCycle[3];        //the looping section of a behavior (starting row, ending row, repeating cycles)
//     char* dutyAngles;         //the data array for skill angles and parameters
//     Motion() {
//       period = 0;
//       expectedRollPitch[0] = 0;
//       expectedRollPitch[1] = 0;
//       dutyAngles = NULL;
//     }

//     int lookupAddressByName(char* skillName) {
//       int skillAddressShift = 0;
//       for (byte s = 0; s < NUM_SKILLS; s++) {//save skill info to on-board EEPROM, load skills to SkillList
//         byte nameLen = EEPROM.read(SKILLS + skillAddressShift++);
//         char* readName = new char[nameLen + 1];
//         for (byte l = 0; l < nameLen; l++) {
//           readName[l] = EEPROM.read(SKILLS + skillAddressShift++);
//         }
//         readName[nameLen] = '\0';
//         if (!strcmp(readName, skillName)) {
//           delete[]readName;
//           return SKILLS + skillAddressShift;
//         }
//         delete[]readName;
//         skillAddressShift += 3;//1 byte type, 1 int address
//       }
//       PTLF("wrong key!");
//       return -1;
//     }
//     void loadDataFromProgmem(unsigned int pgmAddress) {
//       period = pgm_read_byte(pgmAddress);//automatically cast to char*
//       for (int i = 0; i < 2; i++)
//         expectedRollPitch[i] = (int8_t)pgm_read_byte(pgmAddress + 1 + i);
//       angleDataRatio = pgm_read_byte(pgmAddress + 3);
//       byte skillHeader = 4;
//       byte frameSize;
//       if (period < -1) {
//         frameSize = 20;
//         for (byte i = 0; i < 3; i++)
//           loopCycle[i] = pgm_read_byte(pgmAddress + skillHeader + i);
//         skillHeader = 7;
//       }
//       else
//         frameSize = period > 1 ? WALKING_DOF : 16;
//       int len = abs(period) * frameSize;
//       //delete []dutyAngles; //check here
//       dutyAngles = new char[len];
//       for (int k = 0; k < len; k++) {
//         dutyAngles[k] = pgm_read_byte(pgmAddress + skillHeader + k);
//       }
//     }
//     void loadDataFromI2cEeprom(unsigned int &eeAddress) {
//       Wire.beginTransmission(DEVICE_ADDRESS);
//       Wire.write((int)((eeAddress) >> 8));   // MSB
//       Wire.write((int)((eeAddress) & 0xFF)); // LSB
//       Wire.endTransmission();
//       byte skillHeader = 4;
//       Wire.requestFrom(DEVICE_ADDRESS, skillHeader);
//       period = Wire.read();
//       //PTL("read " + String(period) + " frames");
//       for (int i = 0; i < 2; i++)
//         expectedRollPitch[i] = (int8_t)Wire.read();
//       angleDataRatio = Wire.read();

//       byte frameSize;
//       if (period < -1) {
//         skillHeader = 7;
//         frameSize = 20;
//         Wire.requestFrom(DEVICE_ADDRESS, 3);
//         for (byte i = 0; i < 3; i++)
//           loopCycle[i] = Wire.read();
//       }
//       else
//         frameSize = period > 1 ? WALKING_DOF : 16;
//       int len = abs(period) * frameSize;
//       //delete []dutyAngles;//check here

//       dutyAngles = new char[len];

//       int readFromEE = 0;
//       int readToWire = 0;
//       while (len > 0) {
//         //PTL("request " + String(min(WIRE_BUFFER, len)));
//         Wire.requestFrom(DEVICE_ADDRESS, min(WIRE_BUFFER, len));
//         readToWire = 0;
//         do {
//           if (Wire.available()) dutyAngles[readFromEE++] = Wire.read();
//           /*PT( (int8_t)dutyAngles[readFromEE - 1]);
//             PT('\t')*/
//         } while (--len > 0 && ++readToWire < WIRE_BUFFER);
//         //PTL();
//       }
//       //PTLF("finish reading");
//     }

//     void loadDataByOnboardEepromAddress(int onBoardEepromAddress) {
//       char skillType = EEPROM.read(onBoardEepromAddress);
//       unsigned int dataArrayAddress = EEPROMReadInt(onBoardEepromAddress + 1);
//       delete[] dutyAngles;

//       if (skillType == 'I') { //copy instinct data array from external i2c eeprom
//         loadDataFromI2cEeprom(dataArrayAddress);
//       }
//       else                    //copy newbility data array from progmem
//       {
//         loadDataFromProgmem(dataArrayAddress);
//       }
//     }

//     void loadBySkillName(char* skillName) {//get lookup information from on-board EEPROM and read the data array from storage
//       int onBoardEepromAddress = lookupAddressByName(skillName);
//       if (onBoardEepromAddress == -1)
//         return;
//       loadDataByOnboardEepromAddress(onBoardEepromAddress);
//     }

//     void info() {
//       PTL("period: " + String(period) + ",\tdelayBetweenFrames: " + ",\texpected (pitch,roll): (" + expectedRollPitch[0]*M_RAD2DEG + "," + expectedRollPitch[1]*M_RAD2DEG + ")");
//       for (int k = 0; k < period * (period > 1 ? WALKING_DOF : 16); k++) {
//         PT(String((int8_t)dutyAngles[k]) + ", ");
//       }
//       PTL();
//     }
// };


void assignSkillAddressToOnboardEeprom() {
  const char zero[] PROGMEM = { 
    1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};
  const char* progmemPointer[] = {zero, };

  int skillAddressShift = 0;
  PT("\n* Assigning ");
  PT(sizeof(progmemPointer) / 2);
  PTL(" skill addresses...");
  for (byte s = 0; s < sizeof(progmemPointer) / 2; s++) { //save skill info to on-board EEPROM, load skills to SkillList
    if (s)
      PTL(s);
    byte nameLen = EEPROM.read(SKILLS + skillAddressShift++); //without last type character
    skillAddressShift += nameLen;
    char skillType = EEPROM.read(SKILLS + skillAddressShift++);
    if (skillType == 'N') // the address of I(nstinct) has been written in previous operation: saveSkillNameFromProgmemToOnboardEEPROM() in instinct.ino
      // if skillType == N(ewbility), save pointer address of progmem data array to onboard eeprom.
      // it has to be done for different sketches because the addresses are dynamically assigned
      EEPROMWriteInt(SKILLS + skillAddressShift, (int)progmemPointer[s]);
    skillAddressShift += 2;
  }
  PTLF("Finished!");
}

float adjust(byte i) {
  float rollAdj, pitchAdj, adj;
  if (i == 1 || i > 3)  {//check idx = 1
    bool leftQ = (i - 1 ) % 4 > 1 ? true : false;
    float leftRightFactor = 1;
    if ((leftQ && ramp * RollPitchDeviation[0] > 0 )// operator * is higher than &&
        || ( !leftQ && ramp * RollPitchDeviation[0] < 0))
      leftRightFactor = LEFT_RIGHT_FACTOR * abs(ramp);
    rollAdj = fabs(RollPitchDeviation[0]) * adaptiveCoefficient(i, 0) * leftRightFactor;

  }
  else
    rollAdj = RollPitchDeviation[0] * adaptiveCoefficient(i, 0) ;
  currentAdjust[i] = M_DEG2RAD * (
                       (i > 3 ? postureOrWalkingFactor : 1) *
                       rollAdj - ramp * adaptiveCoefficient(i, 1) * ((i % 4 < 2) ? ( RollPitchDeviation[1]) : RollPitchDeviation[1]));
  return currentAdjust[i];
}

void saveCalib(int8_t *var) {
  for (byte i = 0; i < DOF; i++) {
    EEPROM.update(CALIB + i, var[i]);
    calibratedDuty0[i] = SERVOMIN + PWM_RANGE / 2 + float(middleShift(i) + var[i]) * pulsePerDegree[i] * rotationDirection(i);
  }
}

void calibratedPWM(byte i, float angle, float speedRatio) {
  int duty0 = calibratedDuty0[i] + currentAng[i] * pulsePerDegree[i] * rotationDirection(i);
  currentAng[i] = angle;
  int duty = calibratedDuty0[i] + angle * pulsePerDegree[i] * rotationDirection(i);
  duty = max(SERVOMIN , min(SERVOMAX , duty));
  byte steps = byte(round(abs(duty - duty0) / 1.0/*degreeStep*/ / speedRatio)); //default speed is 1 degree per step
  for (byte s = 0; s <= steps; s++) {
    pwm.setPWM(pin(i), 0, duty + (steps == 0 ? 0 : (1 + cos(M_PI * s / steps)) / 2 * (duty0 - duty)));
  }
}

void allCalibratedPWM(char * dutyAng, byte offset) {
  for (int8_t i = DOF - 1; i >= offset; i--) {
    calibratedPWM(i, dutyAng[i]);
  }
}

void shutServos() {
  delay(100);
  for (int8_t i = DOF - 1; i >= 0; i--) {
    pwm.setPWM(i, 0, 4096);
  }
}


void skillByName(char* skillName, byte angleDataRatio, float speedRatio, bool shutServoAfterward) {
  motion.loadBySkillName(skillName);
  transform(motion.dutyAngles, angleDataRatio, speedRatio);
  if (shutServoAfterward) {
    shutServos();
    token = 'd';
  }
}

void skillByCommand(Command::Command& cmd, byte angleDataRatio, float speedRatio, bool shutServoAfterward) {
  motion.loadByCommand(cmd);
  transform(motion.dutyAngles, angleDataRatio, speedRatio);
  if (shutServoAfterward) {
    shutServos();
    cmd = Command::Command(Command::Simple::Rest);
  }
}


//short tools

void printRange(int r0, int r1) {
  if (r1 == 0)
    for (byte i = 0; i < r0; i++) {
      PT(i);
      PT('\t');
    }
  else
    for (byte i = r0; i < r1; i++) {
      PT(i);
      PT('\t');
    }
  PTL();
}


char getUserInput() {//limited to one character
  while (!Serial.available());
  return Serial.read();
}


bool sensorConnectedQ(int n) {
  float mean = 0;
  float bLag = analogRead(A3);
  for (int i = 0; i < READING_COUNT; i++) {
    int a, b;
    a = analogRead(A2);
    b = analogRead(A3);
    mean = mean + ((a - b) * (a - b) - mean) / (i + 1);

    if (abs(a - b) > 50 && abs(b - bLag) < 5) {
      return true;
    }

    bLag = b;
    delay(1);
  }
  return sqrt(mean) > 20 ? true : false;
}


// TODO: See if we have this - implement if we do
// int SoundLightSensorPattern(char *cmd) { //under construction, and will only be active with our new sound and light sensor connected.
//   int sound = analogRead(SOUND); //larger sound has larger readings
//   int light = analogRead(LIGHT); //lower light has larger readings
//   Serial.print(1024);
//   Serial.print('\t');
//   Serial.print(0);
//   Serial.print('\t');
//   Serial.print(sound);
//   Serial.print('\t');
//   Serial.println(light);
//   if (light > 800 && sound < 500) { //dark condition
//     if (!restQ) {
//       skillByName("rest", 1, 1, 1);
//       delay(500);
//       restQ = true;
//     }
//   }
//   else {
//     restQ = false;

//     if (sound > 700) {

//       for (byte l = 0; l < 10; l++) {
//         lightLag = light;
//         light = analogRead(LIGHT); //lower light has larger readings
//         if (light - lightLag  < - 40) {
//           beep(15);
//           PTL("Bang!");
//           strcpy(cmd, "pd");
//           token = 'k';
//           return 4;
//         }
//       }

//       strcpy(cmd, "balance");

//     }
//     else
//       strcpy(cmd, "sit");


//     token = 'k';
//     return 4;
//     //delay(10);
//   }
//   return 0;
// }
