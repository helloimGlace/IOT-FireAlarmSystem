#define BLYNK_TEMPLATE_ID TEMPLATE_ID
#define BLYNK_TEMPLATE_NAME TEMPLATE_NAME
#define BLYNK_AUTH_TOKEN AUTH_TOKEN

#include "config.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
// #include <SoftwareSerial.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

BlynkTimer timer;

int currentAlarm = 0;

void setup() {
  Serial.begin(9600); // For debugging
  Wire.begin(D1, D2); // SDA = D1 (GPIO4), SCL = D2 (GPIO5)
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, requestData); // Poll every 1000ms
}

void loop() {
  Blynk.run();
  timer.run();
}

void requestData() {
  Wire.requestFrom(8, 10);
  
  if (Wire.available() >= 10) {
    int smoke = (Wire.read() << 8) | Wire.read();
    int temp = (Wire.read() << 8) | Wire.read();
    int flame1 = (Wire.read() << 8) | Wire.read();
    int flame2 = (Wire.read() << 8) | Wire.read();
    int flame = Wire.read();
    int alarmTriggered = Wire.read();

    Serial.print(smoke); Serial.print(",");
    Serial.print(temp); Serial.print(",");
    Serial.print(flame1); Serial.print(",");
    Serial.print(flame2); Serial.print(",");
    Serial.println(flame);
    Serial.println(alarmTriggered);

    // Update Blynk Gauges
    Blynk.virtualWrite(V2, smoke);
    Blynk.virtualWrite(V3, temp);
    Blynk.virtualWrite(V4, flame);
    Blynk.virtualWrite(V5, flame1);
    Blynk.virtualWrite(V6, flame2);
    Blynk.virtualWrite(V1, alarmTriggered);

    // Alarm Logic for Blynk Notifications
    if (alarmTriggered == 1 && currentAlarm != alarmTriggered) {
      Blynk.logEvent("fire_alert", "Alert: Fire Detected!");
    } else if (alarmTriggered != 1 && currentAlarm != alarmTriggered) {
      Blynk.logEvent("fire_off", "Fire has been put out!");
    }
    
    currentAlarm = alarmTriggered;
  }
}
