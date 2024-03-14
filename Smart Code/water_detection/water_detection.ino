#define WATER_SENSOR_PIN 12

void setup() {
  Serial.begin(115200);
  pinMode(WATER_SENSOR_PIN, INPUT_PULLUP);
}

void loop() {
  int waterStatus = digitalRead(WATER_SENSOR_PIN);

  if (waterStatus == LOW) {
    Serial.println("No water detected.");
  } else {
    Serial.println("Water detected on the surface!");
  }

  delay(1000);
}
