 Bionic Arm — Gesture-Controlled Prosthetic

A gesture-controlled bionic arm built using an **ATmega328P microcontroller** and **MPU6050 IMU sensor**, communicating wirelessly via **HC-05 Bluetooth**. The arm mirrors real-time hand gestures of the user, with applications in assistive robotics and prosthetics.

---



---

 How It Works

```
User's Hand (Transmitter Side)
    └── MPU6050 (Gyro/Accel) → ATmega328P → HC-05 (TX)
                                                    ↓ Bluetooth
                                              HC-05 (RX)
                                                    ↓
                                          ATmega328P (Receiver)
                                                    ↓
                                    Servo Motors → Bionic Arm Fingers
```

1. The MPU6050 reads hand tilt/orientation (roll, pitch, yaw) at the transmitter end.
2. The ATmega328P processes the raw IMU data and applies a moving-average filter to reduce noise.
3. Processed values are sent wirelessly over Bluetooth (HC-05) to the receiver module.
4. The receiver maps sensor values to servo motor angles — each finger servo corresponds to a gesture axis.

---
 Components

| Component | Purpose |
|-----------|---------|
| ATmega328P (x2) | Main microcontroller (transmitter + receiver) |
| MPU6050 | 6-axis Gyroscope + Accelerometer (I2C) |
| HC-05 Bluetooth Module (x2) | Wireless communication |
| SG90 Servo Motors (x5) | Finger actuation |
| 9V Battery + LM7805 | Power supply |
| 3D Printed / Cardboard Frame | Arm structure |

---

 Project Structure

```
bionic-arm/
├── transmitter/
│   └── transmitter.ino       
├── receiver/
│   └── receiver.ino                    
└── README.md
```

---

 Getting Started

Prerequisites
- Arduino IDE (1.8.x or 2.x)
- Libraries: `Wire.h` (built-in), `MPU6050` by Electronic Cats, `Servo.h` (built-in)

 Upload Steps
1. Clone this repo
2. Open `transmitter/transmitter.ino` in Arduino IDE
3. Install the MPU6050 library via Library Manager
4. Upload to the **transmitter** Arduino (with MPU6050 + HC-05)
5. Open `receiver/receiver.ino` and upload to the **receiver** Arduino (with servos + HC-05)
6. Power both boards — gestures on the transmitter side will mirror on the arm

---

 Key Features
- Real-time gesture detection via IMU
- Wireless transmission over Bluetooth (~10m range)
- Debouncing + threshold calibration for smooth servo response
- Modular code — easy to extend to more fingers or axes

---

 Future Improvements
- Add EMG (muscle) sensors for more natural control
- Implement finger-level individual control
- Port to ESP32 for WiFi + cloud logging of gesture data
- 3D print a proper prosthetic casing

---

 Author

**Aryan Bhasakhetre**
B.E. EXTC — Atharva College of Engineering, Mumbai
[LinkedIn](https://linkedin.com/in/aryanbhasakhetre) | [GitHub](https://github.com/aryanbhasakhetre)

---

## 📄 License
MIT License — free to use, modify, and distribute with attribution.
