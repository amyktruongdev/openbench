#include <WiFi.h>
#include <WebServer.h>
#include <esp_now.h>
#include <Preferences.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "time.h"

WebServer server(80);
Preferences preferences;

// SSID for eduroam
const char* ssid = "eduroam";
const char* mqtt_server = "openbenches.com";
const int mqtt_port = 8883;
const char* mqtt_topic = "sensors/data";
const char* mqtt_client_id = "esp32_gateway";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -28800;
const int daylightOffset_sec = 0;

const char* root_ca PROGMEM = R"EOF(-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----)EOF";

WiFiClientSecure espClient;
PubSubClient client(espClient);

// Structure to receive data from sensor nodes
typedef struct {
    int sensorId; // Sensor ID
    int equipmentId; // Equipment ID
    bool activity; // Activity Boolean
    int battery; // Battery Percentage
    unsigned long timestamp; // Timestamp
} SensorData;

typedef struct {
    unsigned long timestamp;
} TimePacket;

SensorData receivedData;
TimePacket timeData;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // Broadcast to all ESP-NOW nodes

// Function to fetch time from NTP
void getTimeFromNTP() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        timeData.timestamp = time(nullptr);  // Get current Unix timestamp
        Serial.print("Updated Time: ");
        Serial.println(timeData.timestamp);
    } else {
        Serial.println("Failed to get time from NTP");
    }
}

// Function to load credentials securely from NVS
void loadCredentials(String& identity, String& username, String& password) {
    preferences.begin("wifi", true);
    identity = preferences.getString("identity", "");
    username = preferences.getString("username", "");
    password = preferences.getString("password", "");
    preferences.end();
}

// Connect to eduroam
void connectToEduroam() {
    String identity, username, password;
    loadCredentials(identity, username, password);

    if (identity.isEmpty() || username.isEmpty() || password.isEmpty()) {
        Serial.println(F("No credentials stored. Please save them first."));
        return;
    }

    Serial.print(F("Connecting to network: "));
    Serial.println(ssid);

    WiFi.disconnect(true);
    WiFi.begin(ssid, WPA2_AUTH_PEAP, identity.c_str(), username.c_str(), password.c_str());

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(F("."));
    }

    Serial.println(F("\nWiFi is connected!"));
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());

    // Configure NTP time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    getTimeFromNTP();
}

// Send time updates via ESP-NOW
void sendTimeUpdate() {
    getTimeFromNTP();  // Fetch the latest NTP time
    esp_now_send(broadcastAddress, (uint8_t*)&timeData, sizeof(timeData));
    Serial.println("⏳ Time update sent to all sensors via ESP-NOW");
}

// ESP-NOW Callback Function
void onDataRecv(const esp_now_recv_info* info, const uint8_t* incomingData, int len) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    Serial.printf("\n📡 Data Received: Sensor=%d, Equipment=%d, Active=%s, Battery=%d%%, Time=%lu\n", 
                  receivedData.sensorId, 
                  receivedData.equipmentId,
                  receivedData.activity ? "Active" : "Idle",
                  receivedData.battery,
                  receivedData.timestamp);
    
    // Format Data to JSON
    char message[100];
    sprintf(message, "{\"sensorId\":%d,\"equipmentId\":\"%d\",\"activity\":%s,\"battery\":%d,\"time\":%lu}", 
            receivedData.sensorId,
            receivedData.equipmentId,
            receivedData.activity ? "true" : "false", 
            receivedData.battery, 
            receivedData.timestamp);

    // Publish received data to MQTT
    client.publish(mqtt_topic, message);
    Serial.println("📤 Data forwarded to MQTT");
    
    // Send acknowledgment back to sensor
    esp_now_send(info->src_addr, (uint8_t *)&receivedData, sizeof(receivedData));
}


// MQTT Setup
void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(mqtt_client_id)) {
            Serial.println("Connected to MQTT");
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(". Trying again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    connectToEduroam();
    Serial.print("✅ Gateway WiFi Channel: ");
    Serial.println(WiFi.channel());
    delay(5000);
    
    // Setup ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("❌ ESP-NOW Initialization Failed");
        return;
    }
    esp_now_register_recv_cb(onDataRecv);
    
    espClient.setCACert(root_ca);

    // Setup MQTT
    client.setServer(mqtt_server, mqtt_port);

        // Send time update every 10 minutes
    xTaskCreatePinnedToCore(
        [](void* parameter) {
            while (true) {
                sendTimeUpdate();
                vTaskDelay(10 * 60 * 1000 / portTICK_PERIOD_MS);
            }
        },
        "TimeUpdateTask",
        4096,
        NULL,
        1,
        NULL,
        1
    );
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    delay(1000);
}
