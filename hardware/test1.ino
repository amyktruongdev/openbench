#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Define LED pin
const int ledPin = 13;
const int motionThreshold = 2000; // Adjust this value to set the sensitivity

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize MPU6050
  Wire.begin();
  mpu.initialize();

  // Check if the MPU6050 is connected
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }
  Serial.println("MPU6050 connected.");

  // Initialize LED pin as output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Start with LED off
}

void loop() {
  // Get accelerometer values
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Print accelerometer values for debugging
  Serial.print("Acceleration: X=");
  Serial.print(ax);
  Serial.print(" | Y=");
  Serial.print(ay);
  Serial.print(" | Z=");
  Serial.println(az);

  // Calculate the "magnitude" of the acceleration vector
  int accelerationMagnitude = sqrt(ax * ax + ay * ay + az * az);

  // Check if motion is detected
  if (accelerationMagnitude > motionThreshold) {
    digitalWrite(ledPin, HIGH);  // Turn on LED
    Serial.println("Motion detected - LED ON");
  } else {
    digitalWrite(ledPin, LOW);   // Turn off LED
  }

  delay(100); // Small delay for stability
}
