#include <WiFi.h>
#include <WebServer.h>
#include <esp_now.h>
#include <Preferences.h>
#include <PubSubClient.h>

WebServer server(80);
Preferences preferences;

// SSID for eduroam
const char* ssid = "eduroam";
const char* mqtt_server = "insert_dns";
const int mqtt_port = 1883;
const char* mqtt_topic = "insert_topic";
const char* mqtt_client_id = "esp32_gateway";

WiFiClient espClient;
PubSubClient client(espClient);

// Structure to receive data from sensor nodes
typedef struct {
    char id[10];
    bool active;
} SensorData;

SensorData receivedData;

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
}

// ESP-NOW Callback Function
void onDataRecv(const esp_now_recv_info* info, const uint8_t* incomingData, int len) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    Serial.printf("\n📡 Data Received: %s - %s\n", receivedData.id, receivedData.active ? "Active" : "Idle");
    
    // Publish received data to MQTT
    char message[50];
    sprintf(message, "{\"id\":\"%s\",\"active\":%s}", receivedData.id, receivedData.active ? "true" : "false");
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
    
    // Setup ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("❌ ESP-NOW Initialization Failed");
        return;
    }
    esp_now_register_recv_cb(onDataRecv);
    
    // Setup MQTT
    client.setServer(mqtt_server, mqtt_port);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    delay(1000);
}
