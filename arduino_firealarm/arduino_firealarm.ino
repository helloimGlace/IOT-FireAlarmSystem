#include <Servo.h>
#include <Wire.h>

Servo myServo;

const int flamePin = 6;
const int smokePin = A0;
const int tempPin = A1;
const int pumpPin = 12;
const int buzzerPin = 13;

int pos = 1;
int step = 5;

int smokeVal, tempVal;
bool flameDetected;

void setup() {
  Serial.begin(9600);
  Wire.begin(8); 
  Wire.onRequest(requestEvent); // Register the send function
  
  myServo.attach(9);
  pinMode(flamePin, INPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // Constant scanning
  flameDetected = (digitalRead(flamePin) == LOW);
  smokeVal = analogRead(smokePin);
  tempVal = analogRead(tempPin);

  // Local Logic
  int activeCount = (flameDetected ? 1 : 0) + (smokeVal > 550 ? 1 : 0) + (tempVal*500/1023 > 60 ? 1 : 0);
  
  if (activeCount >= 2) {
    digitalWrite(pumpPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(pumpPin, LOW);
    digitalWrite(buzzerPin, LOW);
    
    // Move Servo when alarm not active
    myServo.write(pos);
    pos += step;
    if (pos <= 0 || pos >= 90) step *= -1;
  }
  
  delay(50);
}

void requestEvent() {
  Serial.print(smokeVal); Serial.print(",");
  Serial.print(tempVal*500/1023); Serial.print(",");
  Serial.println(flameDetected ? 1 : 0);
  // Pack data into bytes to send to Master
  byte data[5];
  data[0] = smokeVal >> 8;      // Smoke High Byte
  data[1] = smokeVal & 0xFF;    // Smoke Low Byte
  data[2] = tempVal >> 8;       // Temp High Byte
  data[3] = tempVal & 0xFF;     // Temp Low Byte
  data[4] = flameDetected ? 1 : 0;
  
  Wire.write(data, 5);
}