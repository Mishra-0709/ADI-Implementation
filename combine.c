#include <Adafruit_MPU6050.h>  // Include MPU6050 library
#include <Adafruit_Sensor.h>   // Include sensor library
#include <Wire.h>              // Include Wire library
#include <WiFi.h>              // Include WiFi library
#include <ThingSpeak.h>        // Include ThingSpeak library
#include <U8g2lib.h>           // Include OLED library

// Constants and Pins
Adafruit_MPU6050 mpu;
const int trigPin1 = 5;  // Ultrasonic Sensor 1 Trigger Pin
const int echoPin1 = 18; // Ultrasonic Sensor 1 Echo Pin
const int trigPin2 = 22; // Ultrasonic Sensor 2 Trigger Pin
const int echoPin2 = 23; // Ultrasonic Sensor 2 Echo Pin
const int ledPin = 4;    // LED Pin
const int buzzerPin = 15;// Buzzer Pin

#define SOUND_SPEED 0.034 // cm/us -> Speed of sound

// WiFi and ThingSpeak Credentials
const char* ssid = "Sesuuu";
const char* password = "nnnnnnnn";
WiFiClient client;
unsigned long myChannelNumber = 2780486;
const char* myWriteAPIKey = "8IVHBFWZ7FF26I9Q";

// OLED Display Initialization
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

long duration1, duration2;
float distanceCm1, distanceM1, distanceCm2;
float acceleration, relativeVelocity;
unsigned long t = 1; // Time interval in seconds

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) delay(10);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Initialize Pins
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Initialize Wi-Fi
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");

  // Initialize OLED
  u8g2.begin();
  u8g2.clearDisplay(); // Clear the display buffer
}

float readUltrasonicSensor(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * SOUND_SPEED / 2; // Distance in cm
}

void loop() {
  // Read Ultrasonic Sensor Data
  distanceCm1 = readUltrasonicSensor(trigPin1, echoPin1); // US1 Distance (cm)
  distanceM1 = distanceCm1 / 100.0;                      // Convert US1 Distance to meters
  distanceCm2 = readUltrasonicSensor(trigPin2, echoPin2); // US2 Distance (cm)

  // Read MPU6050 Sensor Data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  acceleration = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2)) - 9.81;
  relativeVelocity = fabs((distanceM1 / t) - (acceleration * t)); // Relative velocity (m/s)

  // LED and Buzzer Logic
  if ((distanceCm1 > 0 && distanceCm1 <= 10) || (distanceCm2 > 0 && distanceCm2 <= 20)) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  if (relativeVelocity > 5) {
    digitalWrite(buzzerPin, HIGH);
    Serial.println("Forward Collision Detected! Buzzer Activated.");
  } else {
    digitalWrite(buzzerPin, LOW);
  }

  // OLED Display Logic
  u8g2.clearBuffer(); // Clear the buffer before drawing
  u8g2.setFont(u8g2_font_ncenB08_tr);
  if (fabs(relativeVelocity) > 7 && fabs(acceleration) > 12) {
    u8g2.drawStr(0, 10, "Forward Collision");
  } else {
    u8g2.drawStr(0, 10, "System Monitoring");
  }
  u8g2.sendBuffer(); // Send buffer to display

  // Print Data to Serial Monitor
  Serial.print("US1 Distance (m): ");
  Serial.println(distanceM1);
  Serial.print("US2 Distance (cm): ");
  Serial.println(distanceCm2);
  Serial.print("Acceleration (m/s^2): ");
  Serial.println(acceleration);
  Serial.print("Temperature (C): ");
  Serial.println(temp.temperature);
  Serial.print("Relative Velocity (m/s): ");
  Serial.println(relativeVelocity);

  // Send Data to ThingSpeak
  ThingSpeak.setField(1, distanceM1);
  ThingSpeak.setField(2, distanceCm2);
  ThingSpeak.setField(3, acceleration);
  ThingSpeak.setField(4, temp.temperature);
  ThingSpeak.setField(5, relativeVelocity);

  int status = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (status == 200) {
    Serial.println("Data successfully sent to ThingSpeak.");
  } else {
    Serial.println("Error sending data to ThingSpeak: " + String(status));
  }

  delay(1000); // 1-second delay
}
