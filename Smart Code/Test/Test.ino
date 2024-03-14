#include <SoftwareSerial.h>
  

// Define the pins for the SoftwareSerial communication

#define D2 2//D3
#define D3 3 //D4
#define D4 4 //D4

SoftwareSerial sim800l(D2, D3); // RX, TX

void setup() {
  // Open serial communication with the GPS module
  Serial.begin(9600);
  Serial.println("GPS Module Initialized");

  // Initialize SoftwareSerial for communication with the SIM800L module
  sim800l.begin(9600);

  // Reset the SIM800L module
  pinMode(D4, OUTPUT); // Connect SIM800L RST pin to D4
  digitalWrite(D4, LOW); // Set LOW to enable the module
  delay(1000);
  digitalWrite(D4, HIGH); // Set HIGH to exit reset mode
  delay(3000);

  // Initialize the SIM800L module
  sim800l.println("AT");
  delay(1000);
  sim800l.println("AT+CMGF=1"); // Set SMS text mode
  delay(1000);
}

void loop() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    sendMessage(message);
  }

  if (sim800l.available()) {
    Serial.write(sim800l.read());
  }
}

void sendMessage(String message) {
  // Get GPS coordinates
  String location = getGPSLocation();

  // Construct the message
  String fullMessage = "Emergency! Location: " + location + "\n" + message;

  // Send the SMS
  sim800l.println("AT+CMGS=\"+918698437788\""); // Replace PHONE_NUMBER with the recipient's phone number
  delay(1000);
  sim800l.print(fullMessage);
  delay(100);
  sim800l.write(26); // Send Ctrl+Z to indicate the end of the message
  delay(1000);
}

String getGPSLocation() {
  // Wait for a valid GPS fix
  while (!Serial.available() || Serial.read() != '$') {
    // Wait for the start of a GPS sentence
      Serial.println("Wait for the start of a GPS sentence");
  }

  // Read the GPS data
  String sentence = Serial.readStringUntil('\n');

  if (sentence.startsWith("$GPRMC")) {
    // Parse the sentence to get latitude and longitude
    char lat[12], lon[12];
    if (sscanf(sentence.c_str(), "$GPRMC,%*f,%*c,%*f,%*c,%s,%*f,%s", lat, lon) == 2) {
      return "Lat: " + String(lat) + ", Lon: " + String(lon);
    }
  }

  return "Location not available";
}
