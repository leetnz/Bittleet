//
// Attitude Benchmark App
// Monitors and reports time spent doing attitude computations.
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#include "AttitudeBenchmark.h"
#include "../state/Attitude.h"

#include "../3rdParty/I2Cdev/I2Cdev.h"
#include "../3rdParty/MPU6050/MPU6050.h"


static MPU6050 mpu;

static void initI2C() {
  Wire.begin();
  Wire.setClock(400000);
}

void AttitudeBenchmark::setup() {
  Serial.begin(115200);
  while (!Serial);

  initI2C();
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  mpu.setDLPFMode(2); // Effectively 100Hz bandwidth for gyra and accel
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2); // Don't need anything beyond 2g
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_1000); 
}

void AttitudeBenchmark::loop() {
  static Attitude::Attitude attitude{};
  delay(10);
  uint32_t dt = micros();


  Attitude::Measurement m;
  m.us = micros();
  mpu.getMotion6(&m.accel.x, &m.accel.y, &m.accel.z, &m.gyro.x, &m.gyro.y, &m.gyro.z);
  m.accel.x = -m.accel.x;
  m.accel.y = -m.accel.y;
  m.gyro.x = -m.gyro.x;
  m.gyro.y = -m.gyro.y;

  attitude.update(m);
  dt = micros() - dt;

  Serial.print("dt (us): ");
  Serial.print(dt);
  Serial.print("\t");
  Serial.print("roll: ");
  Serial.print(attitude.roll());
  Serial.print("\t");
  Serial.print("pitch: ");
  Serial.print(attitude.pitch());
  Serial.print("\t");
  Serial.print("ax: ");
  Serial.print(m.accel.x);
  Serial.print("\t");
  Serial.print("ay: ");
  Serial.print(m.accel.y);
  Serial.print("\t");
  Serial.print("az: ");
  Serial.print(m.accel.z);
  Serial.print("\n");

  Serial.print("\n");

}
