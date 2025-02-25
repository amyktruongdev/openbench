#include <Wire.h>
#include <MPU6050.h>
#include <esp_now.h>
#include <Preferences.h>
#include <WiFi.h>
#include "esp_eap_client.h"

/****************************************************************
                    SETTING UP WIFI CONNECTION
****************************************************************/
Preferences preferences;

// SSID for eduroam
const char* ssid = "eduroam";

// Saves credentials in NVS
void saveCredentials(const char* identity, const char* username, const char* password) {
  preferences.begin("wifi", false); // Open "wifi" namespace in read-write mode
  preferences.putString("identity", identity);
  preferences.putString("username", username);
  preferences.putString("password", password);
  preferences.end(); // Close preferences
}

// Loads credentials from NVS
void loadCredentials(String& identity, String& username, String& password) {
  preferences.begin("wifi", true); // Open "wifi" namespace in read-only mode
  identity = preferences.getString("identity", "");
  username = preferences.getString("username", "");
  password = preferences.getString("password", "");
  preferences.end();
}

// Function to delete stored credentials (optional)
void deleteCredentials() {
  preferences.begin("wifi", false); // Open "wifi" namespace in read-write mode
  preferences.clear(); // Clear all keys in the namespace
  preferences.end(); 
}

// Connects to WiFi using stored credentials
void connectToEduroam() {
  String identity, username, password;
  loadCredentials(identity, username, password); // Loads credentials

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
MPU6050 mpu;  // Creates an instance of the MPU6050 sensor
#define MOVEMENT_THRESHOLD 1.4  // Adjust based on sensitivity

void mpuSetUp() {
    // Initialize MPU6050
    Serial.println("Initializing MPU6050...");
    mpu.initialize();

    // Check if MPU6050 is connected
    if (!mpu.testConnection()) {
        Serial.println("MPU6050 connection failed!");
        while (1);
    }

    Serial.println("MPU6050 initialized!");
}

/****************************************************************
                        SETTING UP ESP-NOW
****************************************************************/
uint8_t gatewayAddress[] = {0xEC, 0x64, 0xC9, 0x5D, 0x37, 0x24}; // MAC of the gateway ESP32

// Sensor Data structure defined to hold data related to sensor.
typedef struct {
    char id[10]; // Sensor node's unique id.
    bool active; // Boolean to represent if in use or not.
} SensorData;

SensorData data;

// Callback when ESP-NOW data is sent.
// Parameters mac address & status of send operation.
void onDataSent(const uint8_t *macAddr, esp_now_send_status_t status) {
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Data sent! YAY." : "IT FAILED. BOOHOO!");
}

void setupESPNow() {
    WiFi.mode(WIFI_STA); // Set to station mode.
    // Check if ESP-NOW initialization failed.
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization failed!");
        return;
    }
    esp_now_register_send_cb(onDataSent); // Register "onDataSent" callback to check if send was good or not

    esp_now_peer_info_t gatewayInfo = {}; // Will hold info about the gateway.
    // Copy gateway's mac addy into gatewayInfo's "peer_addr" field.
    memcpy(gatewayInfo.peer_addr, gatewayAddress, 6); // 6 bytes for each 2-digit hexadecimal value in mac addy
    gatewayInfo.channel = 0;
    gatewayInfo.encrypt = false;
    
    // Check to see if gateway was added as peer in ESP-NOW network.
    if (esp_now_add_peer(&gatewayInfo) != ESP_OK) {
        Serial.println("Failed to add ESP-NOW peer.");
    }
}

/****************************************************************
****************************************************************/

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    mpuSetUp();
    setupESPNow();

    strcpy(data.id, "bench");  // Set equipment ID
}

void sendData() {
    esp_err_t result = esp_now_send(gatewayAddress, (uint8_t *)&data, sizeof(data));
    if (result != ESP_OK) {
        Serial.println("ESP-NOW send failed");
    }
}

void loop() {
    int16_t ax, ay, az; // Raw accelerometer values

    // Read raw accelerometer data
    mpu.getAcceleration(&ax, &ay, &az);

    // Convert raw values to g-force (assuming default ±2G range)
    float accelX = ax / 16384.0;
    float accelY = ay / 16384.0;
    float accelZ = az / 16384.0;

    // Compute total acceleration magnitude
    float accelMagnitude = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);

    // Check if movement is detected
    bool movementDetected = accelMagnitude > MOVEMENT_THRESHOLD;
    if (movementDetected != data.active) {  // Send data only if state changes
        data.active = movementDetected;
        Serial.println(movementDetected ? "🚨 Movement Detected!" : "💤 No Movement");
        sendData();
    }

    delay(1000);  // Adjust sampling rate

}
