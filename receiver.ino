/*
 * Bionic Arm - Receiver Side
 * Receives gesture data over Bluetooth and drives servo motors
 * Wiring:
 *   HC-05 TX    -> D0 (RX)
 *   HC-05 RX    -> D1 (TX) via voltage divider
 *   Servo 1     -> D3  (Thumb)
 *   Servo 2     -> D5  (Index finger)
 *   Servo 3     -> D6  (Middle finger)
 *   Servo 4     -> D9  (Ring finger)
 *   Servo 5     -> D10 (Pinky finger)
 */

#include <Servo.h>
#include <stdio.h>

Servo thumb;
Servo index;
Servo middle;
Servo ring;
Servo pinky;

const uint8_t DEADBAND = 3;
const uint8_t RX_BUFFER_SIZE = 24;

int prevRoll = 90;
int prevPitch = 90;

char rxBuffer[RX_BUFFER_SIZE];
uint8_t rxIndex = 0;

void setAllServos(int angle) {
  thumb.write(angle);
  index.write(angle);
  middle.write(angle);
  ring.write(angle);
  pinky.write(angle);
}

bool readPacket(int &roll, int &pitch) {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      rxBuffer[rxIndex] = '\0';
      rxIndex = 0;

      int parsedRoll = 0;
      int parsedPitch = 0;
      if (sscanf(rxBuffer, "%d,%d", &parsedRoll, &parsedPitch) == 2) {
        roll = parsedRoll;
        pitch = parsedPitch;
        return true;
      }
      return false;
    }

    if (rxIndex < RX_BUFFER_SIZE - 1) {
      rxBuffer[rxIndex++] = c;
    } else {
      // Overflow: drop the partial line and start again.
      rxIndex = 0;
    }
  }

  return false;
}

void setup() {
  Serial.begin(9600);

  thumb.attach(3);
  index.attach(5);
  middle.attach(6);
  ring.attach(9);
  pinky.attach(10);

  // Start all fingers at neutral (half-open).
  setAllServos(90);
  delay(500);
}

void loop() {
  int roll = 0;
  int pitch = 0;

  if (!readPacket(roll, pitch)) {
    return;
  }

  roll = constrain(roll, 0, 180);
  pitch = constrain(pitch, 0, 180);

  // Ignore tiny changes to reduce servo jitter.
  if (abs(roll - prevRoll) < DEADBAND) {
    roll = prevRoll;
  }
  if (abs(pitch - prevPitch) < DEADBAND) {
    pitch = prevPitch;
  }

  prevRoll = roll;
  prevPitch = pitch;

  // Roll controls the four fingers; pitch controls the thumb.
  int fingerAngle = roll;
  int thumbAngle = pitch;

  thumb.write(thumbAngle);
  index.write(fingerAngle);
  middle.write(fingerAngle);
  ring.write(fingerAngle);
  pinky.write(fingerAngle);
}
