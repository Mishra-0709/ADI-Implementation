#include <NewPing.h>      // Library for HC-SR04 ultrasonic sensors
#include <Wire.h>
#include <MPU6050.h>      // Library for MPU-6050 gyro sensor

// HC-SR04 Ultrasonic Sensor Pin Definitions
#define TRIG_PIN_1 2
#define ECHO_PIN_1 3
#define TRIG_PIN_2 4
#define ECHO_PIN_2 5
#define TRIG_PIN_3 6
#define ECHO_PIN_3 7
#define TRIG_PIN_4 8
#define ECHO_PIN_4 9
#define MAX_DISTANCE 200   // Max measurable distance in cm

// Create NewPing objects for each ultrasonic sensor
NewPing sonar1(TRIG_PIN_1, ECHO_PIN_1, MAX_DISTANCE);
NewPing sonar2(TRIG_PIN_2, ECHO_PIN_2, MAX_DISTANCE);
NewPing sonar3(TRIG_PIN_3, ECHO_PIN_3, MAX_DISTANCE);
NewPing sonar4(TRIG_PIN_4, ECHO_PIN_4, MAX_DISTANCE);

// MPU-6050 Gyro Sensor
MPU6050 mpu;
float ax, ay, az;
float myVehicleSpeedX = 0;

// Timing Variables
unsigned long previousUltrasonicMillis = 0;
unsigned long previousGyroMillis = 0;
const long ultrasonicInterval = 1000; // 1-second interval for ultrasonic sensors
const long gyroInterval = 2000;       // 2-second interval for gyro sensor

void setup() {
  Serial.begin(9600);
  
  // Initialize MPU-6050
  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU-6050 not connected.");
    while (1);
  }
  Serial.println("MPU-6050 initialized.");
  
  Serial.println("Starting Combined Sensor Readings...");
}

void loop() {
  unsigned long currentMillis = millis();

  // Ultrasonic Sensor Readings every 1 second
  if (currentMillis - previousUltrasonicMillis >= ultrasonicInterval) {
    previousUltrasonicMillis = currentMillis;

    // Read distances in cm from each ultrasonic sensor
    float distance1 = sonar1.ping_cm();
    float distance2 = sonar2.ping_cm();
    float distance3 = sonar3.ping_cm();
    float distance4 = sonar4.ping_cm();
    
    // Calculate the average distance
    float averageDistance = (distance1 + distance2 + distance3 + distance4) / 4.0;

    // Print ultrasonic distances and average
    Serial.print("Ultrasonic Distances - ");
    Serial.print("D1: "); Serial.print(distance1); Serial.print(" cm, ");
    Serial.print("D2: "); Serial.print(distance2); Serial.print(" cm, ");
    Serial.print("D3: "); Serial.print(distance3); Serial.print(" cm, ");
    Serial.print("D4: "); Serial.print(distance4); Serial.print(" cm, ");
    Serial.print("Avg: "); Serial.print(averageDistance); Serial.println(" cm");
  }

  // Gyro Sensor Reading every 2 seconds
  if (currentMillis - previousGyroMillis >= gyroInterval) {
    previousGyroMillis = currentMillis;

    // Get acceleration data from MPU-6050
    mpu.getAcceleration(&ax, &ay, &az);

    // Calculate speed along the X-axis
    float accelerationX = ax / 16384.0; // Convert raw data to m/s²
    myVehicleSpeedX += accelerationX * (gyroInterval / 1000.0); // Speed in m/s

    // Print gyro acceleration and calculated speed
    Serial.print("Gyro - Acceleration (X-axis): ");
    Serial.print(accelerationX);
    Serial.print(" m/s^2, ");
    Serial.print("Calculated Speed: ");
    Serial.print(myVehicleSpeedX * 100); // Convert to cm/s for consistent units
    Serial.println(" cm/s");
  }
}
