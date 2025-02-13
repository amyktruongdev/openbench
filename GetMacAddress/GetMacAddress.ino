#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_MODE_STA);  // Set to station mode before getting MAC
    delay(100);  // Give it a moment to initialize

    Serial.print("ESP32 MAC Address: ");
    Serial.println(WiFi.macAddress());
}

void loop() {}
