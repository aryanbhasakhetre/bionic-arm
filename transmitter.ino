/*
 * Bionic Arm - Transmitter Side
 */

#include <Wire.h>
#include <MPU6050.h>
#include <math.h>

MPU6050 mpu;

const uint8_t FILTER_SIZE = 5;
const unsigned long SAMPLE_PERIOD_MS = 20; // ~50 Hz update rate

struct MovingAverageFilter {
  int values[FILTER_SIZE];
  uint8_t index;
  uint8_t count;
  long sum;
};

MovingAverageFilter rollFilter  = {{0}, 0, 0, 0};
MovingAverageFilter pitchFilter = {{0}, 0, 0, 0};

unsigned long lastSampleMs = 0;

static int updateMovingAverage(MovingAverageFilter &filter, int newVal) {
  filter.sum -= filter.values[filter.index];
  filter.values[filter.index] = newVal;
  filter.sum += newVal;

  filter.index = (filter.index + 1) % FILTER_SIZE;
  if (filter.count < FILTER_SIZE) {
    filter.count++;
  }

  return (int)(filter.sum / filter.count);
}

void setup() {
  Serial.begin(9600); // HC-05 default baud rate
  Wire.begin();
  Wire.setClock(400000);
  mpu.initialize();

  if (!mpu.testConnection()) {
    // MPU6050 not found - blink LED on pin 13 rapidly
    pinMode(13, OUTPUT);
    while (1) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
  }
}

void loop() {
  unsigned long now = millis();
  if (now - lastSampleMs < SAMPLE_PERIOD_MS) {
    return;
  }
  lastSampleMs = now;

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float rollDeg = atan2((float)ay, sqrt((float)ax * ax + (float)az * az)) * 180.0f / PI;
  float pitchDeg = atan2(-(float)ax, sqrt((float)ay * ay + (float)az * az)) * 180.0f / PI;

  int rawRoll = constrain((int)roundf(rollDeg + 90.0f), 0, 180);
  int rawPitch = constrain((int)roundf(pitchDeg + 90.0f), 0, 180);

  int smoothRoll = updateMovingAverage(rollFilter, rawRoll);
  int smoothPitch = updateMovingAverage(pitchFilter, rawPitch);

  Serial.print(smoothRoll);
  Serial.print(',');
  Serial.println(smoothPitch);
}
