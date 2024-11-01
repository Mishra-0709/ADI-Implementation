#include <Wire.h>
#include <MPU6050.h> // Library for MPU-6050

MPU6050 mpu;

// Variables for acceleration and speed
float ax, ay, az;
float myVehicleSpeedX = 0;
unsigned long previousMillis = 0;
const long interval = 1000; // 1-second interval

void setup() {
  Serial.begin(9600);

  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU-6050 not connected.");
    while (1);
  }

  Serial.println("MPU-6050 initialized.");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Get acceleration data
    mpu.getAcceleration(&ax, &ay, &az);

    // Calculate speed along the X-axis
    float accelerationX = ax / 16384.0; // Scale factor for MPU-6050
    myVehicleSpeedX += accelerationX * (interval / 1000.0); // Speed in m/s

    // Print acceleration and calculated speed
    Serial.print("Acceleration (X-axis): ");
    Serial.print(accelerationX);
    Serial.print(" m/s^2, ");

    Serial.print("Calculated Speed: ");
    Serial.print(myVehicleSpeedX * 100); // Convert to cm/s for consistency
    Serial.println(" cm/s");
  }
}
