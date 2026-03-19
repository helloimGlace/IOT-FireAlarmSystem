#include <Servo.h>
#include <Wire.h>
#include <DHT.h>

Servo servo1;
Servo servo2;
DHT tempPin = DHT(A1, DHT22);

const int flamePin1 = A2;
const int flamePin2 = A3;
const int smokePin = A0;
const int pumpPin = 12;
const int buzzerPin = 13;

unsigned long previousMillis = 0;
unsigned long interval = 5000UL;

int pos = 1;
int step = 1;

int smokeVal, tempVal, flameVal1, flameVal2;
bool flameDetected;
bool alarmTriggered = false;

void setup() {
  Serial.begin(9600);
  Wire.begin(8); 
  Wire.onRequest(requestEvent); // Register the send function
  
  servo1.attach(9);
  servo2.attach(10);
  pinMode(pumpPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  tempPin.begin();
}

void loop() {
  // Constant scanning
  flameVal1 = analogRead(flamePin1);
  flameVal2 = analogRead(flamePin2);
  flameDetected = (flameVal1 <= 150);
  flameDetected = (flameDetected || flameVal2 <= 150);
  smokeVal = analogRead(smokePin);
  tempVal = tempPin.readTemperature();

  // Local Logic
  int activeCount = (flameDetected ? 1 : 0) + (smokeVal > 700 ? 1 : 0) + (tempVal > 55 ? 1 : 0);
  
  if (activeCount >= 2) {
    digitalWrite(pumpPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    alarmTriggered = true;
    previousMillis = millis();
  } else if (millis() - previousMillis > interval) {
    digitalWrite(pumpPin, LOW);
    digitalWrite(buzzerPin, LOW);
    alarmTriggered = false;
    // Move Servo when alarm not active
    servo1.write(pos);
    servo2.write(pos);
    pos += step;
    if (pos <= 0 || pos >= 90) step *= -1;
  }
  
  delay(50);
}

void requestEvent() {
  Serial.print(smokeVal); Serial.print(",");
  Serial.print(tempVal); Serial.print(",");
  Serial.println(flameDetected ? 1 : 0);
  // Pack data into bytes to send to Master
  byte data[10];
  data[0] = smokeVal >> 8;      // Smoke High Byte
  data[1] = smokeVal & 0xFF;    // Smoke Low Byte
  data[2] = tempVal >> 8;       // Temp High Byte
  data[3] = tempVal & 0xFF;     // Temp Low Byte
  data[4] = flameVal1 >> 8;     // Flame 1 High Byte
  data[5] = flameVal1 & 0xFF;   // Flame 1 Low Byte
  data[6] = flameVal2 >> 8;     // Flame 2 High Byte
  data[7] = flameVal2 & 0xFF;   // Flame 2 Low Byte
  data[8] = flameDetected ? 1 : 0;
  data[9] = alarmTriggered ? 1 : 0;
  
  Wire.write(data, 10);
}