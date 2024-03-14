#include <NewPing.h>

#define TRIG_PIN_1 5  // GPIO pin connected to the TRIG pin of the first ultrasonic sensor
#define ECHO_PIN_1 18 // GPIO pin connected to the ECHO pin of the first ultrasonic sensor

#define TRIG_PIN_2 2  // GPIO pin connected to the TRIG pin of the second ultrasonic sensor
#define ECHO_PIN_2 4 // GPIO pin connected to the ECHO pin of the second ultrasonic sensor

#define BUZZER_PIN 12   // GPIO pin connected to the buzzer
#define MAX_DISTANCE 200 // Maximum distance for obstacle detection in centimeters
#define BUZZER_DELAY 1000 // Duration of the buzzer sound in milliseconds

NewPing sonar1(TRIG_PIN_1, ECHO_PIN_1, MAX_DISTANCE); // Create a NewPing object for the first sensor
NewPing sonar2(TRIG_PIN_2, ECHO_PIN_2, MAX_DISTANCE); // Create a NewPing object for the second sensor

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  unsigned int distance1 = sonar1.ping_cm(); // Get the distance from the first ultrasonic sensor in centimeters
  unsigned int distance2 = sonar2.ping_cm(); // Get the distance from the second ultrasonic sensor in centimeters

  Serial.print("Distance 1: ");
  Serial.print(distance1);
  Serial.println(" cm");

  Serial.print("Distance 2: ");
  Serial.print(distance2);
  Serial.println(" cm");

  if (distance1 <= 30 || distance2 <= 30) { // Adjust this threshold according to your needs
    // An obstacle is detected, activate the buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    delay(BUZZER_DELAY);
    digitalWrite(BUZZER_PIN, LOW);
  }
}