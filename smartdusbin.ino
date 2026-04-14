#include <Servo.h>

// Pin definitions
#define SOUND_PIN 2
#define SERVO_PIN 6
#define TRIG_PIN 8
#define ECHO_PIN 9

// Constants
#define BIN_HEIGHT_CM 17.0
#define LID_OPEN_TIME 5000

// Globals
Servo lidServo;
unsigned long lidOpenTime = 0;
bool lidOpen = false;

void setup() {
  Serial.begin(9600);

  pinMode(SOUND_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  lidServo.attach(SERVO_PIN);
  lidServo.write(0);  // Start with lid closed

  Serial.println("Smart Dustbin System with Averaged Ultrasonic Ready");
}

void loop() {
  static int lastSoundState = -1;

  int soundState = digitalRead(SOUND_PIN);

  if (soundState != lastSoundState) {
    lastSoundState = soundState;

    if (!lidOpen && soundState == HIGH) {
      openLid();
    }
  }

  if (lidOpen && (millis() - lidOpenTime >= LID_OPEN_TIME)) {
    closeLid();
  }

  // Measure waste level every 3 seconds with average
  static unsigned long lastCheckTime = 0;
  if (millis() - lastCheckTime > 3000) {
    lastCheckTime = millis();
    measureWasteLevelAverage();
  }

  delay(50);
}

void openLid() {
  Serial.println("Sound detected! Opening lid");
  lidServo.write(90);
  lidOpen = true;
  lidOpenTime = millis();
}

void closeLid() {
  Serial.println("Closing lid");
  lidServo.write(0);
  lidOpen = false;
}

long readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // Timeout after 30ms
  return duration;
}

void measureWasteLevelAverage() {
  float totalFillPercent = 0;
  int validReadings = 0;

  for (int i = 0; i < 3; i++) {
    long duration = readUltrasonic();
    if (duration > 0) {
      float distanceCm = duration * 0.0343 / 2;

      if (distanceCm > 0 && distanceCm <= BIN_HEIGHT_CM) {
        float wasteHeight = BIN_HEIGHT_CM - distanceCm;
        float fillPercent = (wasteHeight / BIN_HEIGHT_CM) * 100.0;

        // Clamp
        if (fillPercent < 0) fillPercent = 0;
        if (fillPercent > 100) fillPercent = 100;

        totalFillPercent += fillPercent;
        validReadings++;
      }
    }
    delay(50);
  }

  if (validReadings == 0) {
    Serial.println("Error: No valid ultrasonic readings");
    return;
  }

  float avgFillPercent = totalFillPercent / validReadings;

  Serial.print("Average Waste Level: ");
  Serial.print(avgFillPercent, 1);
  Serial.println("% Filled");

  if (avgFillPercent >= 75.0) {
    Serial.println("🔴 ALERT: Dustbin is almost full!");
  }
}