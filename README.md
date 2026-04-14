# 🌌 SonicBin

### **The Intelligent, Touchless Waste Management System**
*Subtitle: Sound-Controlled Smart Dustbin using Arduino*

---

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: Arduino](https://img.shields.io/badge/Platform-Arduino-00979D?style=flat&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![Language: C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)

## 📖 Overview
**SonicBin** is a touchless, hygiene-focused waste management solution designed to eliminate physical contact with disposal units. While most smart bins rely on expensive proximity modules, SonicBin utilizes **acoustic trigger logic** to provide a low-cost, highly responsive interface.

Beyond simple automation, it features **Fill-Level Intelligence**—constantly monitoring internal capacity to prevent overflows and locking the system when it reaches a maximum threshold, ensuring a sanitary environment at all times.

---

## ⭐ Key Features
* **Acoustic Triggering:** Instant lid activation via clap or snap detection.
* **Intelligent Capacity Monitoring:** Real-time distance sensing to calculate fill percentage.
* **Safety Auto-Lock:** System automatically disables the lid mechanism if the bin is **>80% full**.
* **Precision Actuation:** Smooth 90-degree servo control for seamless mechanical movement.
* **Low-Latency Logic:** Debounced signal processing to prevent accidental triggers.

---

## 🧰 Tech Stack
![Arduino](https://img.shields.io/badge/-Arduino_Uno_R3-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/-Embedded_C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Sensors](https://img.shields.io/badge/-Ultrasonic_%26_Acoustic-FF6F00?style=for-the-badge&logo=sensortower&logoColor=white)

---

## 🏗 Hardware Configuration

| Component | Purpose | Specification |
| :--- | :--- | :--- |
| **Arduino Uno R3** | Central Processing Unit | ATmega328P |
| **Sound Sensor** | Trigger Input | Adjustable Sensitivity |
| **HC-SR04** | Fill-level Detection | Ultrasonic Transceiver |
| **SG90 Servo** | Lid Actuator | 1.2kg/cm Torque |
| **Power Supply** | System Power | 5V / USB |

---

## 🔌 Circuit & Connections
* **Sound Sensor:** VCC → 5V, GND → GND, OUT → Pin 2
* **Ultrasonic (HC-SR04):** VCC → 5V, Trig → Pin 11, Echo → Pin 12, GND → GND
* **Servo Motor (SG90):** PWM → Pin 9, VCC → 5V, GND → GND

---

## ⚙ Working Principle
The system operates on a **Listen-Verify-Act** logic loop:

1. **Detection:** The sound sensor monitors for a specific decibel threshold. A detected clap sends an interrupt signal to the Arduino.
2. **Capacity Check:** The **HC-SR04 ultrasonic sensor** pings the bin interior. If the waste level is within 20% of the lid, the system remains locked to prevent overflow.
3. **Actuation:** If space is available, the **SG90 Micro Servo** rotates 90°, holds for 5 seconds, and then seals the unit.

---

## 💻 Software Implementation

The firmware is designed with **non-blocking logic** using `millis()` instead of `delay()`, ensuring the system remains responsive to sound triggers while simultaneously calculating distance.

### Key Logic Features:
* **State Tracking:** Uses a `lastSoundState` check to prevent multiple triggers from a single sound.
* **Rolling Average:** Implements a 3-sample average for the ultrasonic sensor to eliminate "noise" or outliers in distance measurement.
* **Averaging Interval:** Fill level is recalculated every 3 seconds to optimize power and processing.
* **Safety Thresholds:** Includes logic to serial-print alerts when capacity exceeds 75%.

### Core Logic (`SonicBin.ino`)
```cpp
void loop() {
  static int lastSoundState = -1;
  int soundState = digitalRead(SOUND_PIN);

  // Trigger lid on sound state change
  if (soundState != lastSoundState) {
    lastSoundState = soundState;
    if (!lidOpen && soundState == HIGH) {
      openLid();
    }
  }

  // Automatic timed closing
  if (lidOpen && (millis() - lidOpenTime >= LID_OPEN_TIME)) {
    closeLid();
  }

  // Measure waste level every 3 seconds
  static unsigned long lastCheckTime = 0;
  if (millis() - lastCheckTime > 3000) {
    lastCheckTime = millis();
    measureWasteLevelAverage();
  }
  delay(50);
}

// Function to calculate stable fill percentage
void measureWasteLevelAverage() {
  float totalFillPercent = 0;
  int validReadings = 0;

  for (int i = 0; i < 3; i++) {
    long duration = readUltrasonic();
    if (duration > 0) {
      float distanceCm = duration * 0.0343 / 2;
      if (distanceCm > 0 && distanceCm <= BIN_HEIGHT_CM) {
        float fillPercent = ((BIN_HEIGHT_CM - distanceCm) / BIN_HEIGHT_CM) * 100.0;
        totalFillPercent += constrain(fillPercent, 0, 100);
        validReadings++;
      }
    }
    delay(50);
  }
  
  if (validReadings > 0) {
    float avgFillPercent = totalFillPercent / validReadings;
    Serial.print("Average Waste Level: ");
    Serial.print(avgFillPercent, 1);
    Serial.println("% Filled");
  }
}
```

---

## 🚀 Installation & Setup
* **Hardware Assembly:** Mount the Servo at the bin hinge and the Ultrasonic sensor on the underside of the lid facing downward.
* **Connect Wiring:** Follow the pin configuration listed in the **Circuit** section.
* **Upload Code:** Flash the firmware using the **Arduino IDE**.
* **Calibration:** Adjust the physical potentiometer on the Sound Sensor module to set the desired clap sensitivity.
* **Test:** Perform a clap test and verify the fill-level lock by placing an object close to the ultrasonic sensor.

## 🧠 Engineering Decisions
* **Acoustic vs. IR:** Sound-based triggering provides a wider "activation zone," allowing users to open the bin from various angles without needing to stand directly in front of an IR beam.
* **Ultrasonic Intelligence:** The HC-SR04 adds a layer of preventative maintenance, signaling when the bin needs emptying rather than jamming the motor against overfilled trash.
* **Mechanical Efficiency:** The SG90 servo was selected for its high power-to-weight ratio, providing sufficient torque for a lightweight prototype lid while maintaining low power consumption.

## 🔮 Future Roadmap
- [ ] **IoT Integration:** Add an ESP32 for mobile notifications when the bin is full.
- [ ] **Adaptive Noise Filtering:** Use an Analog input to filter out constant background hums.
- [ ] **OLED Display:** Real-time fill percentage and "Status" face (e.g., 😊/😲).
- [ ] **Dual-Power Mode:** Implement Sleep mode to extend battery life.

## 🎥 Project Demonstration
*(Replace placeholders below with your actual links)*

> 📹 **Demo Video:** [Link to video](#)
> 📷 **Circuitry Photo:** [Link to image](#)

## 📄 License
Distributed under the MIT License.