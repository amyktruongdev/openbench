#include <esp_now.h>
#include <WiFi.h>
#include "esp_sleep.h"
#include "esp_wifi.h"
#include "esp_timer.h"
#include "time.h"

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
  int sensorId;
  int equipmentId;
  bool activity;
  int battery;
  unsigned long timestamp;
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

// Time Management
unsigned long deviceTime = 0;

void requestTime() {
  TimeData request;
  request.type = TIME_REQUEST;

  esp_err_t result = esp_now_send(gatewayAddress, (uint8_t *)&request, sizeof(request));
  if (result == ESP_OK) {
    Serial.println("Requesting time...");
  } else {
    Serial.println("Failed time request :(");
  }
}

void onDataReceived(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  if (len < sizeof(TimePacket)) return;

  TimePacket type = *((TimePacket *)incomingData);

  if (type == TIME_RESPONSE) {
    if (len == sizeof(TimeData)) {
      TimeData response;
      memcpy(&response, incomingData, sizeof(response));
      deviceTime = response.timestamp;
      Serial.print("📥 Time received: ");
      Serial.println(response.timestamp);
    }
  }
}

void setupESPNow() {
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(11, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, gatewayAddress, 6);
  peerInfo.channel = 11;
  peerInfo.encrypt = false;

  esp_now_register_send_cb(onDataSent);
  if (!esp_now_is_peer_exist(gatewayAddress)) {
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Peer add failed!");
      return;
    }
  }

  esp_now_register_recv_cb(onDataReceived);
  Serial.println("ESP-NOW ready.");
}

/****************************************************************
                      VIBRATION SENSOR SETUP
****************************************************************/
#define VIBRATION_PIN 23

void setupVibrationSensor() {
  pinMode(VIBRATION_PIN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(VIBRATION_PIN), onMotionInterrupt, RISING);
}

/****************************************************************
                        POWER MANAGEMENT
****************************************************************/
volatile bool motionDetected = false;
volatile bool readyToSleep = false;
esp_timer_handle_t inactivityTimer;
const uint64_t inactivityTimeout = 10 * 1000000ULL;  // 10 sec
const unsigned long checkInterval = 10 * 60 * 1000;
unsigned long lastCheck = 0;

void IRAM_ATTR onMotionInterrupt() {
  motionDetected = true;
  readyToSleep = false;
}

void IRAM_ATTR inactivityCallback(void *arg) {
  Serial.println("Inactivity timeout — preparing for light sleep...");
  readyToSleep = true;
}

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
                      BATTERY CALCULATION
****************************************************************/
const int adcPin = 34;
const float referenceVoltage = 3.3;
const int adcResolution = 4095;
const float voltageDividerRatio = 0.5;

void batteryCalculation() {
  int adcValue = analogRead(adcPin);
  float adcVoltage = (adcValue / (float)adcResolution) * referenceVoltage;
  float batteryVoltage = adcVoltage / voltageDividerRatio;
  float batteryPercent = (batteryVoltage - 3.0) / (4.2 - 3.0) * 100.0;
  batteryPercent = constrain(batteryPercent, 0.0, 100.0);
  data.battery = (int)batteryPercent;
}

/****************************************************************
                          MAIN SETUP
****************************************************************/
void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup...");

  configTime(0, 0, "");
  setenv("TZ", "PST8PDT,M3.2.0,M11.1.0", 1);
  tzset();

  setupVibrationSensor();
  setupESPNow();

  data.sensorId = 1;
  data.equipmentId = 71;

  esp_timer_create_args_t timerArgs = {
    .callback = &inactivityCallback,
    .name = "inactivityTimer"
  };
  esp_timer_create(&timerArgs, &inactivityTimer);

  lastCheck = millis();
  analogReadResolution(12);

  requestTime();
  unsigned long startWait = millis();
  while (deviceTime == 0 && millis() - startWait < 5000) {
    delay(100);
  }

  Serial.println("Setup complete!");
}

/****************************************************************
                          MAIN LOOP
****************************************************************/
void sendData() {
  esp_err_t result = esp_now_send(gatewayAddress, (uint8_t *)&data, sizeof(data));
  Serial.println(result == ESP_OK ? "Sending data..." : "Send failed.");
}

void loop() {
  if (deviceTime != 0) {
    time_t currentTime = deviceTime;
    struct tm *timeinfo = localtime(&currentTime);
    int hour = timeinfo->tm_hour;

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

    Serial.println("Vibration detected! Sending data...");
    data.activity = true;
    data.timestamp = deviceTime;
    batteryCalculation();
    sendData();

    delay(500);  // Keep CPU active for 500ms
    motionDetected = false;
  }

  else if (readyToSleep) {
    readyToSleep = false;

    Serial.println("Sending inactivity packet before sleep...");
    data.activity = false;
    data.timestamp = deviceTime;
    batteryCalculation();
    sendData();

    delay(500);

    esp_sleep_enable_ext0_wakeup((gpio_num_t)VIBRATION_PIN, 1);
    esp_sleep_enable_timer_wakeup(10 * 60 * 1000000ULL);
    esp_light_sleep_start();

    Serial.println("Woke from light sleep");
    setupESPNow();
    attachInterrupt(digitalPinToInterrupt(VIBRATION_PIN), onMotionInterrupt, RISING);
  }

  delay(100);
}
