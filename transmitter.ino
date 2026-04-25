/*
 * Bionic Arm — Transmitter Side
 * Reads MPU6050 gesture data and sends over Bluetooth (HC-05)
 *
 * Author: Aryan Bhasakhetre
 * Hardware: ATmega328P + MPU6050 (I2C) + HC-05 (Serial)
 * 
 * Wiring:
 *   MPU6050 SDA → A4
 *   MPU6050 SCL → A5
 *   MPU6050 VCC → 3.3V
 *   MPU6050 GND → GND
 *   HC-05 TX    → D0 (RX)
 *   HC-05 RX    → D1 (TX) via voltage divider
 */

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Moving average filter window size
#define FILTER_SIZE 5

int rollBuffer[FILTER_SIZE]  = {0};
int pitchBuffer[FILTER_SIZE] = {0};
int bufIndex = 0;

void setup() {
  Serial.begin(9600);   // HC-05 default baud rate
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    // MPU6050 not found — blink LED on pin 13 rapidly
    pinMode(13, OUTPUT);
    while (1) {
      digitalWrite(13, HIGH); delay(100);
      digitalWrite(13, LOW);  delay(100);
    }
  }
}

int movingAverage(int* buf, int newVal) {
  buf[bufIndex % FILTER_SIZE] = newVal;
  long sum = 0;
  for (int i = 0; i < FILTER_SIZE; i++) sum += buf[i];
  return (int)(sum / FILTER_SIZE);
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Convert raw accelerometer to roll/pitch (-90 to 90 degrees, mapped 0–180)
  int rawRoll  = map(ay, -17000, 17000, 0, 180);
  int rawPitch = map(ax, -17000, 17000, 0, 180);

  // Clamp values
  rawRoll  = constrain(rawRoll,  0, 180);
  rawPitch = constrain(rawPitch, 0, 180);

  // Apply moving average filter
  int smoothRoll  = movingAverage(rollBuffer,  rawRoll);
  int smoothPitch = movingAverage(pitchBuffer, rawPitch);

  bufIndex++;

  // Send as "ROLL,PITCH\n" over Bluetooth
  Serial.print(smoothRoll);
  Serial.print(",");
  Serial.println(smoothPitch);

  delay(20); // ~50Hz update rate
}
