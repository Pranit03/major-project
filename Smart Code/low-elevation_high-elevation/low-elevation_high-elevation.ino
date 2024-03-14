#include <NewPing.h>
#define TRIGGER_PIN 2
#define ECHO_PIN 4
#define BUZZER_PIN 12
#define MAX_DISTANCE 200 

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  delay(50); 
  unsigned int distance = sonar.ping_cm();

  if (distance == 0) {
    Serial.println("Out of range");
  } else if (distance < 20) {
    Serial.println("Low elevation detected"); 
    generateSound(); 
  } else if (distance > 20 && distance < 100) {
    Serial.println("Normal elevation detected"); 
  } else {
    Serial.println("High elevation detected");
  }
}

void generateSound() {
  tone(BUZZER_PIN, 1000, 500);
  delay(1000); 
  noTone(BUZZER_PIN);
}
