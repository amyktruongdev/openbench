#include <WiFi.h>
#include <esp_now.h>

// Structure to receive data
typedef struct {
    char id[10];
    bool active;
} SensorData;

SensorData receivedData;

// Updated callback with new signature
void onDataRecv(const esp_now_recv_info* info, const uint8_t* incomingData, int len) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));

    Serial.printf("Data Received: %s - %s\n", 
                  receivedData.id, receivedData.active ? "Active" : "Idle");

    // Send acknowledgment back to sensor using the correct src_addr field
    esp_now_send(info->src_addr, (uint8_t *)&receivedData, sizeof(receivedData));
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("❌ ESP-NOW Initialization Failed");
        return;
    }

    // Register callback to receive data
    esp_now_register_recv_cb(onDataRecv);
}

void loop() {
    delay(5000);  // Keep the loop running
}
