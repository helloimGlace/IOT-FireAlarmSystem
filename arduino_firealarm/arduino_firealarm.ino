#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial espSerial(10, 11); // RX, TX
Servo myServo;

const int flamePin = 6;
const int smokePin = A0;
const int tempPin = A1;
const int pumpPin = 12;
const int buzzerPin = 13;

bool systemActive = false;
unsigned long clearStartTime = 0;
const unsigned long confirmationDelay = 5000; // 5 seconds in milliseconds

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  myServo.attach(9);
  
  pinMode(flamePin, INPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // Read Sensors
  bool flameDetected = (digitalRead(flamePin) == LOW);
  int smokeVal = analogRead(smokePin);
  int tempVal = analogRead(tempPin)*500/1023;

  int activeCount = 0;
  if (flameDetected) activeCount++;
  if (smokeVal > 550) activeCount++;
  if (tempVal > 60) activeCount++;

  // Send real-time data to ESP for Blynk Gauges
  espSerial.print(smokeVal); espSerial.print(",");
  espSerial.print(tempVal); espSerial.print(",");
  espSerial.println(flameDetected ? "1" : "0");

  // Activation Logic
  if (activeCount >= 2) {
    if (!systemActive) {
      digitalWrite(pumpPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      espSerial.println("ALARM_ON");
      systemActive = true;
    }
    clearStartTime = 0; // Reset the "clear" timer because fire is still detected
  } 
  // Prepare for Deactivation
  else if (systemActive && activeCount < 2) {
    if (clearStartTime == 0) {
      clearStartTime = millis(); // Start counting the 5 seconds now
    }
    
    // Check if 5 seconds have passed since the fire was completely put out
    if (millis() - clearStartTime >= confirmationDelay) {
      digitalWrite(pumpPin, LOW);
      digitalWrite(buzzerPin, LOW);
      espSerial.println("ALARM_OFF");
      systemActive = false;
      clearStartTime = 0;
    }
  }

  // Servo movement (only moves if fire is not active to focus on the spot)
  if (!systemActive) {
    static int angle = 0;
    static int step = 5;
    angle += step;
    if (angle <= 0 || angle >= 90) step *= -1;
    myServo.write(angle);
  }

  delay(200);
}