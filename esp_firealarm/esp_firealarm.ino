#define BLYNK_TEMPLATE_ID TEMPLATE_ID
#define BLYNK_TEMPLATE_NAME TEMPLATE_NAME
#define BLYNK_AUTH_TOKEN AUTH_TOKEN

#include "config.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

SoftwareSerial swSer(5, 4); // D1, D2

void setup() {
  Serial.begin(115200);
  swSer.begin(9600);
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  if (swSer.available() > 0) {
    String data = swSer.readStringUntil('\n');
    data.trim();

    if (data == "ALARM_ON") {
      Blynk.logEvent("fire_alert", "Emergency! Fire detected!");
      Blynk.virtualWrite(V1, 255); // Red LED Widget
    } else if (data == "ALARM_OFF") {
      Blynk.virtualWrite(V1, 0);
    } else {
      // Parse sensor values: "smoke,temp,flame"
      int firstComma = data.indexOf(',');
      int secondComma = data.indexOf(',', firstComma + 1);

      if (firstComma > 0 && secondComma > 0) {
        String s = data.substring(0, firstComma);
        String t = data.substring(firstComma + 1, secondComma);
        String f = data.substring(secondComma + 1);
        Blynk.virtualWrite(V2, s.toInt()); // Smoke Gauge
        Blynk.virtualWrite(V3, t.toInt()); // Temp Gauge
        Blynk.virtualWrite(V4, f.toInt()); // Flame Flash
      }
    }
  }
}