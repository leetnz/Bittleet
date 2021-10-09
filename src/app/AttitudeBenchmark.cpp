//
// Attitude Benchmark App
// Monitors and reports time spent doing attitude computations.
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#include "AttitudeBenchmark.h"

#include <I2Cdev.h>
#include <MPU6050.h>


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
}

void AttitudeBenchmark::loop() {
  uint32_t dt = micros();
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  dt = micros() - dt;

  Serial.print("dt (us): ");
  Serial.print(dt);
  Serial.print("\t");
  Serial.print("ax: ");
  Serial.print(ax);
  Serial.print("\t");
  Serial.print("ay: ");
  Serial.print(ay);
  Serial.print("\t");
  Serial.print("az: ");
  Serial.print(az);
  Serial.print("\n");
  delay(1000);
  dt = micros();
  float roll = -atan2f(ay, az);
  float pitch = -atan2f(ax, az);
  dt = micros() - dt;
  Serial.print("dt (us): ");
  Serial.print(dt);
  Serial.print("\t");
  Serial.print("roll: ");
  Serial.print(roll);
  Serial.print("\t");
  Serial.print("pitch: ");
  Serial.print(pitch);
  Serial.print("\n");

}
