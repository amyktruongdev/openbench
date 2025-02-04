#include <WiFi.h> // WiFi library
#include "time.h" // Time library to retrieve current time.
#include "esp_eap_client.h" // WPA2 library for enterprise networks
#include <Preferences.h> // Preferences library for secure storage
#include <Wire.h>

Preferences preferences;

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

// Timezone settings for Pacific Standard Time (PST)
const long gmtOffset_sec = -28800;   // UTC - 8 hours for PST
const int daylightOffset_sec = 3600; // 1 hour for Daylight Saving Time (DST)
// TIME SERVER TO PING 
const char* ntpServer = "north-america.pool.ntp.org"; 

//CHECK FOR DAYLIGHT SAVINGS 
bool isDST(struct tm timeInfo) {
  int month = timeInfo.tm_mon + 1;  // tm_mon is 0-based
  int day = timeInfo.tm_mday;
  int wday = timeInfo.tm_wday;      // 0 = Sunday

  if (month < 3 || month > 11) return false; // Not DST
  if (month > 3 && month < 11) return true;  // DST is active

  // March: DST starts on the second Sunday
  if (month == 3) {
    int secondSunday = (7 - wday + 7) % 7 + 8; // Second Sunday calculation
    return day >= secondSunday;
  }

  // November: DST ends on the first Sunday
  if (month == 11) {
    int firstSunday = (7 - wday) % 7 + 1; // First Sunday calculation
    return day < firstSunday;
  }

  return false;
}

void syncTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeInfo;

  if (!getLocalTime(&timeInfo)) {
    Serial.println("FAILED");
    return;
  }

  // Adjust for Daylight Saving Time
  if (isDST(timeInfo)) {
    Serial.println("Daylight Saving Time is active (UTC -7)");
  } else {
    Serial.println("Standard Time is active (UTC -8)");
  }

  // Confirm connections
  Serial.println("Connected to PST server.");
}

/*****************************************
          Setting up the MPU6050
******************************************/
// MPU6050 I2C address
const int MPU6050_ADDR = 0x68; // Default I2C address for MPU6050
const int ledPin = 2;          // LED connected to GPIO2
const float movementThreshold = 0.2; // Change threshold in "g"

// Variables to store acceleration values
int16_t accelX, accelY, accelZ;
float prevAx = 0, prevAy = 0, prevAz = 0; // Previous acceleration values
float ax, ay, az; // Current acceleration values
float deltaAx, deltaAy, deltaAz; // Change in acceleration


void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Save credentials (Example: for Cal State University Northridge)
  //saveCredentials("anonymous@my.csun.edu", "", "");
  // Delete this line after the first upload since the ESP32 will remember the info
  // to ensure that your information is secure.
  // Connect to the eduroam network
  connectToEduroam();

  // Initialize MPU6050
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B);  // Power management register
  Wire.write(0);     // Wake up MPU6050
  Wire.endTransmission(true);

  syncTime();

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Turn off the LED
  Serial.println("Setup complete");
}

void loop() {
    // Read raw accelerometer data
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x3B); // Starting register for accelerometer data
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDR, 6, true); // Request 6 bytes

    accelX = Wire.read() << 8 | Wire.read(); // Combine high and low bytes
    accelY = Wire.read() << 8 | Wire.read();
    accelZ = Wire.read() << 8 | Wire.read();

    // Convert raw data to "g" values
    ax = accelX / 16384.0; // Normalize to g
    ay = accelY / 16384.0;
    az = accelZ / 16384.0;

    // Calculate the change in acceleration
    deltaAx = abs(ax - prevAx);
    deltaAy = abs(ay - prevAy);
    deltaAz = abs(az - prevAz);

    // Update previous acceleration values
    prevAx = ax;
    prevAy = ay;
    prevAz = az;


    struct tm timeInfo;

    // Check if the change exceeds the threshold
    if (deltaAx > movementThreshold || deltaAy > movementThreshold || deltaAz > movementThreshold) {
        digitalWrite(ledPin, HIGH);  // Turn on the LED
        if (getLocalTime(&timeInfo)) {
            Serial.printf("Movement detected at: %04d-%02d-%02d %02d:%02d:%02d\n", 
                timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday, 
                timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
        } else {
            Serial.println("FAILURE");
        }
    } else {
        digitalWrite(ledPin, LOW);   // Turn off the LED
    }

    delay(1000);  // Small delay for stability
}
