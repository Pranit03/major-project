#include <TinyGPS++.h>
#include <SoftwareSerial.h>

SoftwareSerial gpsSerial(10, 11); // RX, TX

TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
}

void loop() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        // Process latitude and longitude data
      }
    }
  }
}


#define TRIGGER_PIN 9
#define ECHO_PIN 8

void setup() {
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  long duration, distance;
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  if (distance < 20) {
    // Obstacle detected, implement appropriate action
  }

  delay(500); // Adjust the delay based on your requirements
}

import requests

def get_directions(api_key, origin, destination):
    url = f'https://maps.googleapis.com/maps/api/directions/json?origin={origin}&destination={destination}&key={api_key}'
    response = requests.get(url)
    data = response.json()
    
    if data['status'] == 'OK':
        steps = data['routes'][0]['legs'][0]['steps']
        for step in steps:
            instruction = step['html_instructions']
            print(f"Instruction: {instruction}")
            # Extract additional information as needed
    else:
        print(f"Error: {data['status']}")

# Example usage
api_key = 'YOUR_GOOGLE_MAPS_API_KEY'
origin = 'Current Location'
destination = 'Destination Location'
get_directions(api_key, origin, destination)


from gtts import gTTS
import os

def text_to_speech(text, language='en'):
    tts = gTTS(text=text, lang=language, slow=False)
    tts.save("output.mp3")
    os.system("mpg321 output.mp3")

# Example usage
text_to_speech("Turn left in 50 meters.")

