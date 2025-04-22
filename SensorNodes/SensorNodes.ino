#include <Wire.h>
#include <MPU6050.h>
#include <esp_now.h>
#include <WiFi.h>
#include "esp_sleep.h"
#include "esp_wifi.h"
#include "esp_timer.h"
#include "time.h"

/****************************************************************
                      SETTING UP MPU6050
****************************************************************/
MPU6050 mpu;
#define MPU_INT_PIN 15  // Motion interrupt pin (check actual pin)

/****************************************************************
                        SETTING UP ESP-NOW
****************************************************************/
uint8_t gatewayAddress[] = {0x88, 0x13, 0xBF, 0x82, 0x32, 0xF4}; // MAC of the gateway ESP32

typedef struct {
    int sensorId; // Sensor ID
    int equipmentId; // Equipment ID
    bool activity; // Activity Boolean
    int battery; // Battery Percentage
    unsigned long timestamp; // Timestamp
} SensorData;

SensorData data;

// Callback when ESP-NOW data is sent
void onDataSent(const uint8_t *macAddr, esp_now_send_status_t status) {
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "ESP-NOW Send Success" : "ESP-NOW Send Fail");
}

typedef struct {
    unsigned long unix_time;  // Unix timestamp
} SyncedTime;

// Time Management
unsigned long deviceTime = 0;  // Store Unix time from the gateway

void onTimeReceived(const esp_now_recv_info_t *recv_info, const uint8_t *data, int data_len) {
    // Optional: Access MAC address via recv_info->src_addr
    Serial.print("Received data from: ");
    char macStr[18];
    snprintf(macStr, sizeof(macStr),
             "%02X:%02X:%02X:%02X:%02X:%02X",
             recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
             recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);
    Serial.println(macStr);

    // Handle the received data
    if (data_len == sizeof(SyncedTime)) {
        SyncedTime incomingTime;
        memcpy(&incomingTime, data, sizeof(SyncedTime));
        deviceTime = incomingTime.unix_time;

        Serial.print("✅ Synced time: ");
        Serial.println(deviceTime);
    } else {
        Serial.println("⚠️ Received unknown data size");
    }
}

void setupESPNow() {
    Serial.println("setting up esp now!");
    WiFi.mode(WIFI_STA);

    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(11, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization failed!");
        return;
    }

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, gatewayAddress, 6);
    peerInfo.channel = 11;
    peerInfo.encrypt = false;

    esp_now_register_send_cb(onDataSent);
    
    if (!esp_now_is_peer_exist(gatewayAddress)) {
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            Serial.println("ohno! no peer!");
            return;
        }
    }

    esp_now_register_recv_cb(onTimeReceived);
}

/****************************************************************
                    MOTION DETECTION & SLEEP
****************************************************************/
volatile bool motionDetected = false;
volatile bool readyToSleep = false;
esp_timer_handle_t inactivityTimer;
const uint64_t inactivityTimeout = 10 * 1000000ULL;  // 10 seconds
const unsigned long checkInterval = 10 * 60 * 1000;  // 10 minutes
unsigned long lastCheck = 0;

typedef struct {
    unsigned long timestamp;
} TimePacket;

// Interrupt function for motion detection
void IRAM_ATTR onMotionInterrupt() {
    motionDetected = true;
    readyToSleep = false;
}

// Inactivity timeout callback
void IRAM_ATTR inactivityCallback(void* arg) {
    Serial.println("i sleep");
    readyToSleep = true;
}

void setupMPU6050() {
    Wire.begin();
    mpu.initialize();

    if (!mpu.testConnection()) {
        Serial.println("MPU6050 connection failed!");
        while (1);
    }

    mpu.setIntEnabled(0x40); // Enable Motion Interrupt
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);  // ±4g range, good for most gym movements
    mpu.setMotionDetectionThreshold(100);             // Higher threshold avoids false positives
    mpu.setMotionDetectionDuration(10);              // ~10ms of continuous motion
    mpu.setInterruptLatch(0);
    mpu.setIntMotionEnabled(true);
    mpu.setInterruptLatch(true);        // Latches the INT pin HIGH until cleared
    mpu.setInterruptLatchClear(true);  // Clears latch by reading the INT_STATUS

    pinMode(MPU_INT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(MPU_INT_PIN), onMotionInterrupt, RISING);
}

/****************************************************************
                          POWER MANAGEMENT
****************************************************************/
void enterDeepSleepUntil6AM(int currentHour) {
    int sleepDurationSeconds = (6 - currentHour) * 3600;
    if (sleepDurationSeconds <= 0) {
        sleepDurationSeconds += 24 * 3600;
    }
    Serial.println("Entering Deep Sleep until 6 AM...");
    esp_sleep_enable_timer_wakeup(sleepDurationSeconds * 1000000ULL);
    esp_deep_sleep_start();
}


/****************************************************************
****************************************************************/

void setup() {
    Serial.begin(115200);
    Serial.println("Starting setup!");

    configTime(0, 0, "");  // Prevents time drift
    setenv("TZ", "PST8PDT,M3.2.0,M11.1.0", 1);
    tzset();

    setupMPU6050();
    setupESPNow();

    // Set up some of the values
    data.sensorId = 1;  // Set Sensor ID
    data.equipmentId = 1; // Set Equipment ID
    /****REMOVE BELOW WHEN DONE TESTING****/
    data.battery = random(1, 100);

    // Set up inactivity timer
    esp_timer_create_args_t timerArgs = {
        .callback = &inactivityCallback,
        .name = "inactivityTimer"
    };
    esp_timer_create(&timerArgs, &inactivityTimer);

    lastCheck = millis();

    Serial.println("Setup complete!");
}

void sendData() {
    esp_err_t result = esp_now_send(gatewayAddress, (uint8_t *)&data, sizeof(data));
    if (result == ESP_OK) {
        Serial.println("✅ ESP-NOW Send Success");
    } else {
        Serial.print("❌ ESP-NOW Send Fail, code: ");
        Serial.println(result);
    }
}


void loop() {
    // Check if it's time to sleep
    if (deviceTime != 0) {  // Ensure we have received time from the gateway
        struct tm *timeinfo;
        time_t currentTime = deviceTime;
        timeinfo = localtime(&currentTime);
        int hour = timeinfo->tm_hour;

        // Check every 10 minutes if we should enter deep sleep
        if (millis() - lastCheck >= checkInterval) {
            lastCheck = millis();
            if (hour >= 22 || hour < 6) {
                enterDeepSleepUntil6AM(hour);
            }
        }
    }


    if (motionDetected) {
        esp_timer_stop(inactivityTimer);
        esp_timer_start_once(inactivityTimer, inactivityTimeout);

        Serial.println("motion detected! sending data...");
        data.activity = true;
        data.timestamp = deviceTime;
        delay(30);
        sendData();
        delay(100);  // Prevents rapid-fire sends

        mpu.getIntStatus();  // Clears the interrupt latch
        motionDetected = false;

        mpu.setIntEnabled(0x40);  // Re-enable motion interrupt
    }

    else if (readyToSleep) {
        readyToSleep = false;

        mpu.getIntStatus(); // Clear any prior INT latch

        // Wait for INT pin to go LOW so it can trigger RISING again
        while (digitalRead(MPU_INT_PIN) == HIGH) {
            delay(10);
        }

        esp_sleep_enable_ext0_wakeup((gpio_num_t)MPU_INT_PIN, 1);
        esp_sleep_enable_timer_wakeup(10 * 60 * 1000000ULL); // 10 min

        esp_light_sleep_start();

        Serial.println("i'm now awake");
        setupESPNow();

        attachInterrupt(digitalPinToInterrupt(MPU_INT_PIN), onMotionInterrupt, RISING);
        mpu.setIntEnabled(0x40);  // Re-enable motion interrupt
    }

    delay(100);
}
