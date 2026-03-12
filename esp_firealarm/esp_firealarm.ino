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

void setup() {
  Serial.begin(9600); // For debugging
  Wire.begin(D1, D2); // SDA = D1 (GPIO4), SCL = D2 (GPIO5)
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, requestData); // Poll every 500ms
}

void loop() {
  Blynk.run();
  timer.run();
  delay(500);
}

void requestData() {
  Wire.requestFrom(8, 5);
  
  if (Wire.available() >= 5) {
    int smoke = (Wire.read() << 8) | Wire.read();
    int temp = (Wire.read() << 8) | Wire.read();
    int flame = Wire.read();

    Serial.print(smoke); Serial.print(",");
    Serial.print(temp*500/1023); Serial.print(",");
    Serial.println(flame);

    // Update Blynk Gauges
    Blynk.virtualWrite(V2, smoke);
    Blynk.virtualWrite(V3, temp*500/1023);
    Blynk.virtualWrite(V4, flame);

    // Alarm Logic for Blynk Notifications
    static bool alarmTriggered = false;
    int activeCount = (flame == 1 ? 1 : 0) + (smoke > 550 ? 1 : 0) + (temp*500/1023 > 60 ? 1 : 0);

    if (activeCount >= 2 && !alarmTriggered) {
      Blynk.logEvent("fire_alert", "Alert: Fire Detected!");
      Blynk.virtualWrite(V1, 255);
      alarmTriggered = true;
    } else if (activeCount < 2 && alarmTriggered) {
      Blynk.virtualWrite(V1, 0);
      alarmTriggered = false;
    }
  }
}
