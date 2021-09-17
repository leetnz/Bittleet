/* Main Arduino sketch for OpenCat, the bionic quadruped walking robot.
   Updates should be posted on GitHub: https://github.com/PetoiCamp/OpenCat

   Rongzhong Li
   Jan.3rd, 2021
   Copyright (c) 2021 Petoi LLC.

   This sketch may also includes others' codes under MIT or other open source liscence.
   Check those liscences in corresponding module test folders.
   Feel free to contact us if you find any missing references.

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
#define MAIN_SKETCH
#include "src/OpenCat.h"
#include "src/Command.h"
#include "src/Comms.h"

#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>

#include "src/battery.h"
#include "src/Infrared.h"


#define PACKET_SIZE 42
#define OVERFLOW_THRESHOLD 128

//#if OVERFLOW_THRESHOLD>1024-1024%PACKET_SIZE-1   // when using (1024-1024%PACKET_SIZE) as the overflow resetThreshold, the packet buffer may be broken
// and the reading will be unpredictable. it should be replaced with previous reading to avoid jumping
#define FIX_OVERFLOW
//#endif
#define HISTORY 2
int8_t lag = 0;


MPU6050 mpu;
#define OUTPUT_READABLE_YAWPITCHROLL
// MPU control/status vars
//bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[PACKET_SIZE]; // FIFO storage buffer



// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

// https://brainy-bits.com/blogs/tutorials/ir-remote-arduino
#include <IRremote.h>
#define SHORT_ENCODING // activating this line will use a shorter encoding of the HEX values
// the original value is formatted as address  code complement
//                                   2Bytes  1Byte   1Byte
// it will save 178 Bytes for the final compiled code
/*-----( Declare objects )-----*/
IRrecv irrecv(IR_RECEIVER);     // create instance of 'irrecv'

// Local variables

//control related variables
static Command::Command lastCmd;
static byte hold = 0;
static int8_t offsetLR = 0;
static bool checkGyro = true;
static int8_t skipGyro = 2;

static uint8_t timer = 0;
static byte firstMotionJoint;
static byte jointIdx = 0;

static int8_t servoCalibs[DOF] = {};

static int8_t tStep = 1; // TODO - this should be a motion flag




void getFIFO() {//get FIFO only without further processing
  while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

  // read a packet from FIFO
  mpu.getFIFOBytes(fifoBuffer, packetSize);

  // track FIFO count here in case there is > 1 packet available
  // (this lets us immediately read more without waiting for an interrupt)
  fifoCount -= packetSize;
}

#define MPU_INT_STATUS_OVERFLOW (0x10)
#define MPU_INT_STATUS_DATARDY  (0x01)

#define YPR_YAW (0)
#define YPR_PITCH (1)
#define YPR_ROLL (2)
static float ypr[3] = {}; // TODO: We never use yaw...

void getYPR() {
  
  static bool processLast = false;
  // orientation/motion vars
  Quaternion q;           // [w, x, y, z]         quaternion container
  VectorFloat gravity;    // [x, y, z]            gravity vector
  //get YPR angles from FIFO data, takes time
  // wait for MPU interrupt or extra packet(s) available
  //while (!mpuInterrupt && fifoCount < packetSize) ;
  if (mpuInterrupt || fifoCount >= packetSize)
  {
    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();
    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & MPU_INT_STATUS_OVERFLOW) || fifoCount > OVERFLOW_THRESHOLD) { //1024) {
      // reset so we can continue cleanly
      mpu.resetFIFO();
      processLast = false; // Use the previous read.

      // --
    } else if (mpuIntStatus & MPU_INT_STATUS_DATARDY) {
      // We only find out of the last read was an error if the next read is good. So we can only rely on previous reads.
      if (processLast) {
        // get Euler angles in degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        ypr[0] = ypr[0] * M_RAD2DEG;
        ypr[1] = ypr[1] * M_RAD2DEG;
        ypr[2] = ypr[2] * M_RAD2DEG;
      }
      // wait for correct available data length, should be a VERY short wait
      getFIFO();
      processLast = true;
    }
  }
}

void checkBodyMotion(bool enableMotion, Command::Command& newCmd)  {
  //if (!dmpReady) return;
  getYPR();
  // --
  //deal with accidents
  if (fabs(ypr[YPR_YAW]) > LARGE_PITCH || fabs(ypr[YPR_ROLL]) > LARGE_ROLL) {//wait until stable
    if (!hold){
      for (byte w = 0; w < 50; w++) {
        delay(10);
      }
    }
    if (fabs(ypr[YPR_PITCH]) > LARGE_PITCH || fabs(ypr[YPR_ROLL]) > LARGE_ROLL) {//check again
      if (!hold) {
        enableMotion = true;
        if (fabs(ypr[YPR_ROLL]) > LARGE_ROLL) {
          newCmd = Command::Command(Command::Simple::Recover); // "rc"
        }
      }
      hold = 10;
    }
  }

  // recover
  else if (hold) {
    if (hold == 1) {
      enableMotion = true;
      newCmd = Command::Command(Command::Simple::Balance);
    }
    hold --;
    if (!hold) {
      Command::Command temp = newCmd;
      newCmd = lastCmd;
      lastCmd = temp;
      meow();
    }
  }
  //calculate deviation
  const float levelTolerance[2] = {ROLL_LEVEL_TOLERANCE, PITCH_LEVEL_TOLERANCE}; //the body is still considered as level, no angle adjustment
  for (byte i = 0; i < 2; i++) {
    RollPitchDeviation[i] = ypr[2 - i] - motion.expectedRollPitch[i]; //all in degrees
    RollPitchDeviation[i] = sign(ypr[2 - i]) * max(fabs(RollPitchDeviation[i]) - levelTolerance[i], 0);//filter out small angles
  }

  //PTL(jointIdx);
}

void initI2C() {
  Wire.begin();
  Wire.setClock(400000);
}

void setup() {
  pinMode(BUZZER, OUTPUT);  

  Serial.begin(BAUD_RATE);
  while (!Serial); // wait for ready
  while (Serial.available() && Serial.read()); // empty buffer

  delay(100);
  PTLF("\n* Start *");
#ifdef BITTLE
  PTLF("Bittle");
#elif defined NYBBLE
  PTLF("Nybble");
#endif
  PTLF("Initialize I2C");
  initI2C();
  PTLF("Connect MPU6050");
  mpu.initialize();
  //do
  {
    delay(500);
    // verify connection
    PTLF("Test connection");
    PTL(mpu.testConnection() ? F("MPU successful") : F("MPU failed"));//sometimes it shows "failed" but is ok to bypass.
  } //while (!mpu.testConnection());

  // load and configure the DMP
  do {
    PTLF("Initialize DMP"); // Digital Motion Processor
    devStatus = mpu.dmpInitialize();
    delay(500);
    // supply your own gyro offsets here, scaled for min sensitivity

    for (byte i = 0; i < 4; i++) {
      PT(EEPROMReadInt(MPUCALIB + 4 + i * 2));
      PTF(" ");
    }
    PTL();
    mpu.setZAccelOffset(EEPROMReadInt(MPUCALIB + 4));
    mpu.setXGyroOffset(EEPROMReadInt(MPUCALIB + 6));
    mpu.setYGyroOffset(EEPROMReadInt(MPUCALIB + 8));
    mpu.setZGyroOffset(EEPROMReadInt(MPUCALIB + 10));
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
      // turn on the DMP, now that it's ready
      PTLF("Enable DMP");
      mpu.setDMPEnabled(true);

      // enable Arduino interrupt detection
      PTLF("Enable interrupt");
      attachInterrupt(INTERRUPT, dmpDataReady, RISING);
      mpuIntStatus = mpu.getIntStatus();

      // set our DMP Ready flag so the main loop() function knows it's okay to use it
      PTLF("DMP ready!");
      //dmpReady = true;

      // get expected DMP packet size for later comparison
      packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      PTLF("DMP failed (code ");
      PT(devStatus);
      PTLF(")");
      PTL();
    }
  } while (devStatus);

  //opening music
  // playMelody(MELODY);

  //IR
  {
    //PTLF("IR Receiver Button Decode");
    irrecv.enableIRIn(); // Start the receiver
  }

  assignSkillAddressToOnboardEeprom();
  PTL();

  // servo
  { pwm.begin();

    pwm.setPWMFreq(60 * PWM_FACTOR); // Analog servos run at ~60 Hz updates
    delay(200);

    //meow();
    lastCmd = Command::Command(Command::Simple::Rest);
    motion.loadByCommand(lastCmd);
    for (int8_t i = DOF - 1; i >= 0; i--) {
      servoRange[i] = servoAngleRange(i);
      servoCalibs[i] = servoCalib(i);
      calibratedDuty0[i] =  SERVOMIN + PWM_RANGE / 2 + float(middleShift(i) + servoCalibs[i]) * pulsePerDegreeF(i)  * rotationDirection(i) ;
      //PTL(SERVOMIN + PWM_RANGE / 2 + float(middleShift(i) + servoCalibs[i]) * pulsePerDegreeF(i) * rotationDirection(i) );
      calibratedPWM(i, motion.dutyAngles[i]);
      delay(20);
    }
    shutServos();
  }
  beep(30);

  pinMode(BATT, INPUT);
  pinMode(BUZZER, OUTPUT);

  meow();
}

void loop() {
  static bool enableMotion = false;
  static Command::Command newCmd;
  static Command::Move move{Command::Pace::Medium, Command::Direction::Forward};
  int battAdcReading = analogRead(BATT);
  Battery::State battState = Battery::state(battAdcReading);
  if (battState == Battery::State::Low) { //if battery voltage < threshold, it needs to be recharged
    //give the robot a break when voltage drops after sprint
    //adjust the thresholds according to your batteries' voltage
    //if set too high, the robot will stop working when the battery still has power.
    //If too low, the robot may not alarm before the battery shuts off
    PTLF("Low power!");
    beep(15, 50, 50, 3);
    delay(1500); // HOANI TODO: Should be disabling all servos here
  }
  // HOANI TODO: Do something when no battery is detected

  else {
    newCmd = Command::Command();

    // input block
    {
      decode_results results;
      if (irrecv.decode(&results)) {
        newCmd = Infrared::parseSignal((results.value >> 8), move);
        irrecv.resume(); // receive the next value
      }
      
      Command::Command serialCmd = Comms::parseSerial(Serial, move, currentAng);
      if (serialCmd.type() != Command::Type::None) {
        newCmd = serialCmd;
      }
    }

    // MPU block
    {
      if (checkGyro) {
        if (!(timer % skipGyro)) {
          checkBodyMotion(enableMotion, newCmd);

        }
        else if (mpuInterrupt || fifoCount >= packetSize)
        {
          // reset interrupt flag and get INT_STATUS byte
          mpuInterrupt = false;
          mpuIntStatus = mpu.getIntStatus();
          getFIFO();
        }
      }
    }

    if (newCmd.type() == Command::Type::Move) {
      if (newCmd.get(move) == false) {
        PTLF("Move Err"); // Unexpected...
        // TODO: Should add an error beep type
      } else {
        enableMotion = true;
        motion.loadByCommand(newCmd);
      }
    } else if (newCmd.type() == Command::Type::Simple) {
      Command::Simple cmd;
      if (newCmd.get(cmd) == false) {
        PTLF("Simple Err"); // Unexpected...
      } else {
        switch(cmd) {
          case Command::Simple::Rest: {
            lastCmd = newCmd;
            skillByCommand(lastCmd);
            enableMotion = false;
            break;
          }
          case Command::Simple::GyroToggle: { // TODO: This may possibly be a toggle - check logic
            if (!checkGyro) {
              checkBodyMotion(enableMotion, newCmd);
            }
            checkGyro = !checkGyro;
            enableMotion = true;
            break;
          }
          case Command::Simple::Pause: {
            tStep = !tStep;
            if (tStep) {
              newCmd = Command::Command(); // resume last command.
              enableMotion = true;
            } else {
              shutServos();
              enableMotion = false;
            }
            break;
          }
          case Command::Simple::SaveServoCalibration: {
            PTLF("save offset");
            saveCalib(servoCalibs);
            break;
          }
          case Command::Simple::AbortServoCalibration: {
            PTLF("aborted");
            for (byte i = 0; i < DOF; i++) {
              servoCalibs[i] = servoCalib( i);
            }
            break;
          }
          case Command::Simple::ShowJointAngles: { //show the list of current joint anles
            printRange(DOF);
            printList(currentAng);
            break;
          }
        }
      }
    } else if (newCmd.type() == Command::Type::WithArgs) {
      enableMotion = false;
      Command::WithArgs cmd;
      if (newCmd.get(cmd) == false) {
        PTLF("WithArgs Err"); // Unexpected...
      } else {
        switch(cmd.cmd) {
          case Command::ArgType::Calibrate: {
            PTL();
            printRange(DOF);
            printList(servoCalibs);
            //yield();
            if (lastCmd != newCmd) { //first time entering the calibration function
              lastCmd = newCmd;
              motion.loadByCommand(newCmd);
              transform(motion.dutyAngles);
              checkGyro = false;
            }
            if (cmd.len == 2) {
              int16_t index = cmd.args[0];
              int16_t angle = cmd.args[1];
              // TODO: This appears to allow both absolute and incremental calibration - kind of wierd logic though; might be able to tidy up later.
              //      - Incremental won't work because we use i8... maybe add incremental calbrate command instead
              if (angle >= 1001) { // Using 1001 for incremental calibration. 1001 is adding 1 degree, 1002 is adding 2 and 1009 is adding 9 degrees
                angle = servoCalibs[index] + angle - 1000;
              } else if (angle <= -1001) { // Using -1001 for incremental calibration. -1001 is removing 1 degree, 1002 is removing 2 and 1009 is removing 9 degrees
                angle = servoCalibs[index] + angle + 1000;
              }
              servoCalibs[index] = angle;
              int duty = SERVOMIN + PWM_RANGE / 2 + float(middleShift(index)  + servoCalibs[index] + motion.dutyAngles[index]) * pulsePerDegreeF(index) * rotationDirection(index);
              pwm.setPWM(pin(index), 0,  duty);
            }
            break;
          }
          case Command::ArgType::MoveSequentially: {
            const float angleInterval = 0.2;
            int angleStep = 0;
            const int16_t joints = cmd.len/2;
            for (int16_t i = 0; i < joints; i++) {
              int16_t index = cmd.args[0];
              int16_t angle = cmd.args[1];
              // TODO: This looks like some incremental step logic
              //      - need to encapsulate duty in a function
              //      - we can probably simplify this a lot.
              angleStep = floor((angle - currentAng[index]) / angleInterval);
              for (int a = 0; a < abs(angleStep); a++) {
                int duty = SERVOMIN + PWM_RANGE / 2 + float(middleShift(index)  + servoCalibs[index] + currentAng[index] + a * angleInterval * angleStep / abs(angleStep)) * pulsePerDegreeF(index) * rotationDirection(index);
                pwm.setPWM(pin(index), 0,  duty);
              }
              currentAng[index] = motion.dutyAngles[index] = angle;
            }
            break;
          } 
          case Command::ArgType::Meow: {
            const int repeat = (cmd.len >= 1) ? cmd.args[0] : 0;
            const int increment = (cmd.len >= 2) ? cmd.args[1] + 1 : 1;
            meow(repeat, 0, 50, 200, increment);
            break;
          }
          case Command::ArgType::Beep: {
            const int8_t note = (cmd.len >= 1) ? cmd.args[0] : 0;
            const uint8_t duration = (cmd.len >= 2) ? cmd.args[1] : 0;
            beep(note, duration);
            break;
          }
          case Command::ArgType::MoveSimultaneously: {
            if (cmd.len != DOF) {
              PTLF("Simultaneous Err"); // Unexpected...
            } else {
              transform(cmd.args, 1, 6);
            }
            break;
          }
        }
      }
    }

    if (newCmd != Command::Command()) {
      beep(8);

      //check above
      if (newCmd != lastCmd) {
          motion.loadByCommand(newCmd);

          offsetLR = 0;
          if (newCmd.type() == Command::Type::Move) {
            if (newCmd.get(move)) {
              if (move.direction == Command::Direction::Left) {
                offsetLR = 15;
              } else if (move.direction == Command::Direction::Right) {
                offsetLR = -15;
              }
            }
          } 

          //motion.info();
          timer = 0;
          if ((newCmd != Command::Simple::Balance) && 
              (newCmd != Command::Simple::Lifted) && 
              (newCmd != Command::Simple::Dropped)) {
            lastCmd = newCmd;
          }
          // Xconfig = strcmp(newCmd, "wkX") ? false : true;

          postureOrWalkingFactor = (motion.period == 1 ? 1 : POSTURE_WALKING_FACTOR);
          // if posture, start jointIdx from 0
          // if gait, walking DOF = 8, start jointIdx from 8
          //          walking DOF = 12, start jointIdx from 4
          firstMotionJoint = (motion.period <= 1) ? 0 : DOF - WALKING_DOF;
          //          if (Xconfig) { //for smooth transition
          //            int *targetAngle;
          //            targetAngle = new int [WALKING_DOF];
          //            for (byte i = 0; i < WALKING_DOF; i++)
          //              targetAngle[i] = motion.dutyAngles[i];
          //            targetAngle[WALKING_DOF - 6] = motion.dutyAngles[WALKING_DOF - 2] -5;
          //            targetAngle[WALKING_DOF - 5] = motion.dutyAngles[WALKING_DOF - 1] -5;
          //            targetAngle[WALKING_DOF - 2] = motion.dutyAngles[WALKING_DOF - 2] + 180;
          //            targetAngle[WALKING_DOF - 1] = motion.dutyAngles[WALKING_DOF - 1] + 180;
          //            transform( targetAngle,  1,2, firstMotionJoint);
          //            delete [] targetAngle;
          //          }
          //          else

          if (motion.period < 1) {
            int8_t repeat = motion.loopCycle[2] - 1;
            byte frameSize = 20;
            for (byte c = 0; c < abs(motion.period); c++) { //the last two in the row are transition speed and delay
              transform(motion.dutyAngles + c * frameSize, motion.angleDataRatio, motion.dutyAngles[16 + c * frameSize] / 4.0);

              if (motion.dutyAngles[18 + c * frameSize]) {
                int triggerAxis = motion.dutyAngles[18 + c * frameSize];
                int triggerAngle = motion.dutyAngles[19 + c * frameSize];

                float currentYpr = ypr[abs(triggerAxis)];
                float previousYpr = currentYpr;
                while (1) {
                  getYPR();
                  currentYpr = ypr[abs(triggerAxis)];
                  PT(currentYpr);
                  PTF("\t");
                  PTL(triggerAngle);
                  if ((180 - fabs(currentYpr) > 2)  //skip the angle when the reading jumps from 180 to -180
                      && (triggerAxis * currentYpr < triggerAxis * triggerAngle && triggerAxis * previousYpr > triggerAxis * triggerAngle )
                     ) //the sign of triggerAxis will deterine whether the current angle should be larger or smaller than the trigger angle
                    break;
                  previousYpr = currentYpr;
                }
              }
              else
              {
                delay(motion.dutyAngles[17 + c * frameSize] * 50);
              }
              if (c == motion.loopCycle[1] && repeat > 0) {
                c = motion.loopCycle[0] - 1;
                repeat--;
              }
            }
            lastCmd = Command::Command(Command::Simple::Balance);
            skillByCommand(lastCmd, 1, 2, false);
            for (byte a = 0; a < DOF; a++)
              currentAdjust[a] = 0.0f;
            hold = 0;
          } else {
            transform( motion.dutyAngles, motion.angleDataRatio, 1, firstMotionJoint);
          }
          jointIdx = 3;//DOF; to skip the large adjustment caused by MPU overflow. joint 3 is not used.
          if (newCmd == Command::Simple::Rest) {
            shutServos();
            enableMotion = false;
            lastCmd = newCmd;
          }
      }
    }

    //motion block
    {
      if (enableMotion) {
        if (jointIdx == DOF) {
            // timer = (timer + 1) % abs(motion.period);
            timer += tStep;
            if (timer == abs(motion.period)) {
              timer = 0;
            }
            else if (timer == 255)
              timer = abs(motion.period) - 1;

          jointIdx = 0;

        }
        if (jointIdx == 1) {
          jointIdx = DOF - WALKING_DOF;
        }
        if (jointIdx < firstMotionJoint && abs(motion.period) > 1) {
          calibratedPWM(jointIdx, (jointIdx != 1 ? offsetLR : 0) //look left or right
                        + 10 * sin (timer * (jointIdx + 2) * M_PI / abs(motion.period)) //look around
                        + (checkGyro ? adjust(jointIdx) : 0)
                       );
        }
        else if (jointIdx >= firstMotionJoint) {
          int dutyIdx = timer * WALKING_DOF + jointIdx - firstMotionJoint;
          calibratedPWM(jointIdx, motion.dutyAngles[dutyIdx]*motion.angleDataRatio//+ ((Xconfig && (jointIdx == 14 || jointIdx == 15)) ? 180 : 0)
                        + (checkGyro ?
                           (!(timer % skipGyro)  ?
                            adjust(jointIdx)
                            : currentAdjust[jointIdx].toF32())
                           : 0)
                       );
  
        }
        jointIdx++;
      }
      else
        timer = 0;
    }
  }
}
