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
uint8_t gatewayAddress[] = { 0x88, 0x13, 0xBF, 0x82, 0x32, 0xF4 };  // MAC of the gateway ESP32

enum TimePacket {
  SENSOR_DATA,
  TIME_REQUEST,
  TIME_RESPONSE,
  ACK_PACKET
};

typedef struct {
  TimePacket type;
  int sensorId;             // Sensor ID
  int equipmentId;          // Equipment ID
  bool activity;            // Activity Boolean
  int battery;              // Battery Percentage
  unsigned long timestamp;  // Timestamp
} SensorData;

typedef struct {
  TimePacket type;
  unsigned long timestamp;
} TimeData;

SensorData data;

// Callback when ESP-NOW data is sent
void onDataSent(const uint8_t *macAddr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "send success!" : "data not sent");
}

typedef struct {
  unsigned long unix_time;  // Unix timestamp
} SyncedTime;

// Time Management
void requestTime() {
  TimeData request;
  request.type = TIME_REQUEST;

  esp_err_t result = esp_now_send(gatewayAddress, (uint8_t *)&request, sizeof(request));
  if (result == ESP_OK) {
    Serial.println("requesting time!");

  } else {
    Serial.println("failed time request :(");
  }
}

unsigned long deviceTime = 0;  // Store Unix time from the gateway

void onDataReceived(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  Serial.print("Received data from: ");
  char macStr[18];
  snprintf(macStr, sizeof(macStr),
           "%02X:%02X:%02X:%02X:%02X:%02X",
           recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
           recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);
  Serial.println(macStr);

  // Check minimum size for TimePacket
  if (len < sizeof(TimePacket)) {
    Serial.println("⚠️ Received packet too small!");
    return;
  }

  // Identify packet type
  TimePacket type = *((TimePacket *)incomingData);

  if (type == TIME_RESPONSE) {
    if (len == sizeof(TimeData)) {
      TimeData response;
      memcpy(&response, incomingData, sizeof(response));
      Serial.print("📥 Time received: ");
      Serial.println(response.timestamp);
      deviceTime = response.timestamp;
    } else {
      Serial.println("⚠️ Invalid TIME_RESPONSE size");
    }
  } else if (type == SENSOR_DATA) {
    Serial.println("📡 Sensor data received");
    // Handle sensor data here if needed
  } else if (type == TIME_REQUEST) {
    Serial.println("📡 Time request received");
    // Handle time request here if needed
  } else if (type == ACK_PACKET) {
    Serial.println("✅ Acknowledgment packet received");
    // Handle acknowledgment packet if you implemented it
  } else {
    Serial.print("⚠️ Unknown packet type received: ");
    Serial.println(type);  // Log the unknown packet type for debugging
  }
}



void setupESPNow() {
  WiFi.mode(WIFI_STA);

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(11, WIFI_SECOND_CHAN_NONE);
  // esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);
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

  esp_now_register_recv_cb(onDataReceived);

  Serial.println("esp-now set up complete!");
}

/****************************************************************
                    MOTION DETECTION & SLEEP
****************************************************************/
volatile bool motionDetected = false;
volatile bool readyToSleep = false;
esp_timer_handle_t inactivityTimer;
// const uint64_t inactivityTimeout = 2 * 60 * 1000000ULL;  // inactive time until light sleep: 2 minutes
const uint64_t inactivityTimeout = 10 * 1000000ULL;  // TEST: 10 seconds
const unsigned long checkInterval = 10 * 60 * 1000;  // 10 minutes
unsigned long lastCheck = 0;

// Interrupt function for motion detection
void IRAM_ATTR onMotionInterrupt() {
  motionDetected = true;
  readyToSleep = false;
}

// Inactivity timeout callback
void IRAM_ATTR inactivityCallback(void *arg) {
  Serial.println("i sleep");
  readyToSleep = true;
}

void setupMPU6050() {
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1)
      ;
  }

  mpu.setIntEnabled(0x40);                         // Enable Motion Interrupt
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_4);  // ±4g range, good for most gym movements
  mpu.setMotionDetectionThreshold(10);             // Higher threshold avoids false positives
  mpu.setMotionDetectionDuration(2);               // ~10ms of continuous motion
  mpu.setInterruptLatch(0);
  mpu.setIntMotionEnabled(true);
  mpu.setInterruptLatch(true);       // Latches the INT pin HIGH until cleared
  mpu.setInterruptLatchClear(true);  // Clears latch by reading the INT_STATUS

  pinMode(MPU_INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MPU_INT_PIN), onMotionInterrupt, RISING);

  Serial.println("mpu6050 set up complete!");
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

// battery percentage variables
const int adcPin = 34;                  // ADC pin connected to the middle of the voltage divider
const float referenceVoltage = 3.3;     // ESP32 ADC reference voltage
const int adcResolution = 4095;         // 12-bit ADC
const float voltageDividerRatio = 0.5;  // Due to two 100kΩ resistors

void batteryCalculation() {
  int adcValue = analogRead(adcPin);

  // Convert ADC reading to voltage at pin
  float adcVoltage = (adcValue / (float)adcResolution) * referenceVoltage;
  // Reverse the voltage divider to find actual battery voltage
  float batteryVoltage = adcVoltage / voltageDividerRatio;

  // Calculate battery percentage
  float batteryPercent = (batteryVoltage - 3.0) / (4.2 - 3.0) * 100.0;
  // Clamp between 0% and 100%
  batteryPercent = constrain(batteryPercent, 0.0, 100.0);
  data.battery = (int)batteryPercent;
}

/****************************************************************
****************************************************************/

void setup() {
  Serial.begin(115200);
  Serial.println("starting setup!");

  configTime(0, 0, "");  // Prevents time drift
  setenv("TZ", "PST8PDT,M3.2.0,M11.1.0", 1);
  tzset();

  setupMPU6050();
  setupESPNow();

  // Set up some of the values
  data.sensorId = 1;      // Set Sensor ID
  data.equipmentId = 71;  // Set Equipment ID

  // Set up inactivity timer
  esp_timer_create_args_t timerArgs = {
    .callback = &inactivityCallback,
    .name = "inactivityTimer"
  };
  esp_timer_create(&timerArgs, &inactivityTimer);

  lastCheck = millis();

  analogReadResolution(12);  // Ensure 12-bit ADC resolution

  requestTime();
  unsigned long startWait = millis();
  while (deviceTime == 0 && millis() - startWait < 5000) {
    delay(100); // Wait up to 5 seconds for time
  }

  Serial.println("setup complete!");
}

void sendData() {
  esp_err_t result = esp_now_send(gatewayAddress, (uint8_t *)&data, sizeof(data));
  if (result == ESP_OK) {
    Serial.println("esp-now sending...");
  } else {
    Serial.print("esp-now fail, code: ");
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

    unsigned long startWait = millis();
    while (millis() - startWait < 500) {
      delay(10);  // Keep CPU active for 500ms
    }

    mpu.getIntStatus();  // Clears the interrupt latch
    motionDetected = false;

    mpu.setIntEnabled(0x40);  // Re-enable motion interrupt
  }

  else if (readyToSleep) {
    readyToSleep = false;

    mpu.getIntStatus();  // Clear any prior INT latch

    // Wait for INT pin to go LOW so it can trigger RISING again
    while (digitalRead(MPU_INT_PIN) == HIGH) {
      delay(10);
    }

    // Send inactivity packet before light sleep
    data.activity = false;
    data.timestamp = deviceTime;
    batteryCalculation();
    sendData();

    unsigned long startWait = millis();
    while (millis() - startWait < 500) {
      delay(10);  // Keep CPU active for 500ms
    }

    // Enable wakeup souces
    esp_sleep_enable_ext0_wakeup((gpio_num_t)MPU_INT_PIN, 1);  // Detect motion interrupt
    esp_sleep_enable_timer_wakeup(10 * 60 * 1000000ULL);       // Wake up every time minutes

    esp_light_sleep_start();

    Serial.println("i'm now awake");
    setupESPNow();

    attachInterrupt(digitalPinToInterrupt(MPU_INT_PIN), onMotionInterrupt, RISING);
    mpu.setIntEnabled(0x40);  // Re-enable motion interrupt
  }

  delay(100);
}
