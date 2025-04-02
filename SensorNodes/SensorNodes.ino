#include <Wire.h>
#include <MPU6050.h>
#include <esp_now.h>
#include <WiFi.h>

/****************************************************************
                      SETTING UP MPU6050
****************************************************************/
MPU6050 mpu;  // Creates an instance of the MPU6050 sensor
#define MOVEMENT_THRESHOLD 0.2  // Adjust based on sensitivity

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
uint8_t gatewayAddress[] = {0xA0, 0xB7, 0x65, 0x21, 0xE5, 0xBC}; // MAC of the gateway ESP32

typedef struct {
    int sensor_id; // Sensor ID
    char equipment_id[20]; // Equipment ID
    bool inUse; // Activity Boolean
    int battery; // Battery Percentage
    unsigned long timestamp; // Timestamp
} SensorData;

SensorData data;

// Callback when ESP-NOW data is sent
void onDataSent(const uint8_t *macAddr, esp_now_send_status_t status) {
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "ESP-NOW Send Success" : "ESP-NOW Send Fail");
}

void setupESPNow() {
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization failed!");
        return;
    }
    esp_now_register_send_cb(onDataSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, gatewayAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
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

    // Set up some of the values
    data.sensor_id = 1;  // Set Sensor ID
    strcpy(data.equipment_id, "BenchPress1"); // Set Equipment ID
    /****REMOVE BELOW WHEN DONE TESTING****/
    data.battery = random(1, 100);
    data.timestamp = millis();

    // Initialize movement state with the first sensor reading
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);

    float accelX = ax / 16384.0;
    float accelY = ay / 16384.0;
    float accelZ = az / 16384.0;
    float accelMagnitude = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);

    data.inUse = accelMagnitude > MOVEMENT_THRESHOLD;
}

void sendData() {
    esp_err_t result = esp_now_send(gatewayAddress, (uint8_t *)&data, sizeof(data));
    if (result != ESP_OK) {
        Serial.println("ESP-NOW send failed");
    }
}

float lastAccelMagnitude = 1.0;  // Assume it starts at rest (~1g)

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
    bool movementDetected = abs(accelMagnitude - lastAccelMagnitude) > MOVEMENT_THRESHOLD;
    if (movementDetected != data.inUse) {  // Send data only if state changes
        data.inUse = movementDetected;
        Serial.println(movementDetected ? "🚨 Movement Detected!" : "💤 No Movement");
        sendData();
    }

    lastAccelMagnitude = accelMagnitude;  // Update last known acceleration
    delay(1000);  // Adjust sampling rate

}