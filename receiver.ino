/*
 * Bionic Arm — Receiver Side
 * Receives gesture data over Bluetooth and drives servo motors
 *
 * Author: Aryan Bhasakhetre
 * Hardware: ATmega328P + HC-05 (Serial) + 5x SG90 Servo Motors
 *
 * Wiring:
 *   HC-05 TX    → D0 (RX)
 *   HC-05 RX    → D1 (TX) via voltage divider
 *   Servo 1     → D3  (Thumb)
 *   Servo 2     → D5  (Index finger)
 *   Servo 3     → D6  (Middle finger)
 *   Servo 4     → D9  (Ring finger)
 *   Servo 5     → D10 (Pinky finger)
 */

#include <Servo.h>

Servo thumb, index, middle, ring, pinky;

// Threshold: ignore changes smaller than this (reduces jitter)
#define DEADBAND 3

int prevRoll  = 90;
int prevPitch = 90;

void setup() {
  Serial.begin(9600);

  thumb.attach(3);
  index.attach(5);
  middle.attach(6);
  ring.attach(9);
  pinky.attach(10);

  // Start all fingers at neutral (half-open)
  setAllServos(90);
  delay(500);
}

void setAllServos(int angle) {
  thumb.write(angle);
  index.write(angle);
  middle.write(angle);
  ring.write(angle);
  pinky.write(angle);
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    data.trim();

    int commaIndex = data.indexOf(',');
    if (commaIndex == -1) return; // malformed packet

    int roll  = data.substring(0, commaIndex).toInt();
    int pitch = data.substring(commaIndex + 1).toInt();

    // Apply deadband filter
    if (abs(roll  - prevRoll)  < DEADBAND) roll  = prevRoll;
    if (abs(pitch - prevPitch) < DEADBAND) pitch = prevPitch;

    prevRoll  = roll;
    prevPitch = pitch;

    // Map roll to finger curl (all fingers open/close together based on wrist tilt)
    // Map pitch to thumb opposition
    int fingerAngle = constrain(roll,  0, 180);
    int thumbAngle  = constrain(pitch, 0, 180);

    thumb.write(thumbAngle);
    index.write(fingerAngle);
    middle.write(fingerAngle);
    ring.write(fingerAngle);
    pinky.write(fingerAngle);
  }
}
