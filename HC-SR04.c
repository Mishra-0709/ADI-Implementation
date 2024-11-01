#include <NewPing.h> // Include the NewPing library

// Pin definitions for each ultrasonic sensor
#define TRIG_PIN_1 2
#define ECHO_PIN_1 3
#define TRIG_PIN_2 4
#define ECHO_PIN_2 5
#define TRIG_PIN_3 6
#define ECHO_PIN_3 7
#define TRIG_PIN_4 8
#define ECHO_PIN_4 9

// Maximum distance to measure (in cm)
#define MAX_DISTANCE 200

// Create NewPing objects for each sensor
NewPing sonar1(TRIG_PIN_1, ECHO_PIN_1, MAX_DISTANCE);
NewPing sonar2(TRIG_PIN_2, ECHO_PIN_2, MAX_DISTANCE);
NewPing sonar3(TRIG_PIN_3, ECHO_PIN_3, MAX_DISTANCE);
NewPing sonar4(TRIG_PIN_4, ECHO_PIN_4, MAX_DISTANCE);

// Variables to store distance data
float distance1, distance2, distance3, distance4;
float averageDistance;
unsigned long previousMillis = 0;
const long interval = 1000; // 1-second interval

void setup() {
  Serial.begin(9600);
  Serial.println("Starting HC-SR04 Ultrasonic Sensor Readings...");
}

void loop() {
  unsigned long currentMillis = millis();

  // Read ultrasonic sensors every 1 second
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Reading distances from each sensor in cm
    distance1 = sonar1.ping_cm();
    distance2 = sonar2.ping_cm();
    distance3 = sonar3.ping_cm();
    distance4 = sonar4.ping_cm();

    // Calculate average distance
    averageDistance = (distance1 + distance2 + distance3 + distance4) / 4.0;

    // Print distances to Serial Monitor
    Serial.print("Distance1: "); Serial.print(distance1); Serial.print(" cm, ");
    Serial.print("Distance2: "); Serial.print(distance2); Serial.print(" cm, ");
    Serial.print("Distance3: "); Serial.print(distance3); Serial.print(" cm, ");
    Serial.print("Distance4: "); Serial.print(distance4); Serial.print(" cm, ");
    Serial.print("Average Distance: "); Serial.print(averageDistance); Serial.println(" cm");
  }
}
