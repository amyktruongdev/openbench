#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>
#include "esp_eap_client.h"
#include <MPU6050.h>
#include <Preferences.h>

Preferences preferences;

/****************************************************************
                      SETTING UP WIFI
****************************************************************/

// SSID for eduroam
const char* ssid = "eduroam";

// Function to save credentials securely in NVS
void saveCredentials(const char* identity, const char* username, const char* password) {
  preferences.begin("wifi", false); // Open "wifi" namespace in read-write mode
  preferences.putString("identity", identity);
  preferences.putString("username", username);
  preferences.putString("password", password);
  preferences.end(); // Close preferences
}

// Function to load credentials securely from NVS
void loadCredentials(String& identity, String& username, String& password) {
  preferences.begin("wifi", true); // Open "wifi" namespace in read-only mode
  identity = preferences.getString("identity", "");
  username = preferences.getString("username", "");
  password = preferences.getString("password", "");
  preferences.end(); // Close preferences
}

// Function to delete stored credentials (optional)
void deleteCredentials() {
  preferences.begin("wifi", false); // Open "wifi" namespace in read-write mode
  preferences.clear(); // Clear all keys in the namespace
  preferences.end(); // Close preferences
}

// Connect to WiFi using stored credentials
void connectToEduroam() {
  String identity, username, password;
  loadCredentials(identity, username, password); // Load credentials

  if (identity.isEmpty() || username.isEmpty() || password.isEmpty()) {
    Serial.println(F("No credentials stored. Please save them first."));
    return;
  }

  Serial.print(F("Connecting to network: "));
  Serial.println(ssid);

  WiFi.disconnect(true); // Disconnect from WiFi to set a new connection
  WiFi.begin(ssid, WPA2_AUTH_PEAP, identity.c_str(), username.c_str(), password.c_str()); // Connect without a certificate

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println(F("\nWiFi is connected!"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
}

/****************************************************************
                      SETTING UP MPU6050
****************************************************************/
/*
// MPU6050 I2C address
const int MPU6050_ADDR = 0x68; // Default I2C address for MPU6050
const int ledPin = 2;          // LED connected to GPIO2
const float movementThreshold = 0.2; // Change threshold in "g"

// Variables to store acceleration values
int16_t accelX, accelY, accelZ;
float prevAx = 0, prevAy = 0, prevAz = 0; // Previous acceleration values
float ax, ay, az; // Current acceleration values
float deltaAx, deltaAy, deltaAz; // Change in acceleration
*/
/****************************************************************
                      SETTING UP ESP-NOW
****************************************************************/

MPU6050 mpu;
const int movementThreshold = 5000;  // Adjust based on sensitivity
uint8_t gatewayMAC[] = {0xEC, 0x64, 0xC9, 0x5D, 0x37, 0x24};  // Replace with Gateway ESP32 MAC Address


typedef struct {
    uint32_t timestamp;
} SensorData;

esp_now_peer_info_t peerInfo;
SensorData dataToSend;

void onSent(const uint8_t *macAddr, esp_now_send_status_t status) {
    Serial.print("ESP-Now Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}


/****************************************************************
****************************************************************/

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  connectToEduroam();

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
      Serial.println("ESP-Now Init Failed");
      return;
  }

  memcpy(peerInfo.peer_addr, gatewayMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add peer");
      return;
  }

  esp_now_register_send_cb(onSent);
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  int motionMagnitude = abs(ax) + abs(ay) + abs(az);
  if (motionMagnitude > movementThreshold) {
      Serial.println("Movement detected!");
      dataToSend.timestamp = millis();

      esp_err_t result = esp_now_send(gatewayMAC, (uint8_t *)&dataToSend, sizeof(dataToSend));
      if (result == ESP_OK) {
          Serial.println("Data sent successfully");
      } else {
          Serial.println("Error sending data");
      }
  }

  delay(500);  // Adjust as needed
}
