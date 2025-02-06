#include <WiFi.h>
#include <WebServer.h>
#include <esp_now.h>
#include <Preferences.h> // Preferences library for secure storage

WebServer server(80);

Preferences preferences;

// SSID for eduroam
const char* ssid = "eduroam";

// Function to load credentials securely from NVS
void loadCredentials(String& identity, String& username, String& password) {
  preferences.begin("wifi", true); // Open "wifi" namespace in read-only mode
  identity = preferences.getString("identity", "");
  username = preferences.getString("username", "");
  password = preferences.getString("password", "");
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

void handlePing() {
  server.send(200, "text/plain", "Pong");
}

// Callback function to receive ESP-NOW messages
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  char msg[len + 1];
  memcpy(msg, incomingData, len);
  msg[len] = '\0';
  
  Serial.print("Received ESP-NOW message: ");
  Serial.println(msg);

  /*
  if (strcmp(msg, "ping") == 0) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, "http://192.168.X.X/ping");  // Replace with your ESP32’s IP
    int httpCode = http.GET();
    http.end();
  }
  */

}

void setup() {
  Serial.begin(115200);
  connectToEduroam();

  server.on("/ping", handlePing);
  server.begin();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Initialization Failed!");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  server.handleClient();
}
