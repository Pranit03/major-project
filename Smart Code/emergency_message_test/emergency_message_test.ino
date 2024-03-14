#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial sim800(7, 8); // RX, TX pins for SIM800 module
SoftwareSerial neo6(0, 1);   // RX, TX pins for NEO-6 GPS module
TinyGPSPlus gps;

String emergencyPhoneNumber = "+918698437788"; // Replace with the recipient's phone number

void setup() {
  Serial.begin(9600);
  sim800.begin(9600);
  neo6.begin(9600);
  delay(1000);
  sendATcommand("AT", 1000); // Send AT command to check if SIM800 is responsive
  sendATcommand("AT+CMGF=1", 1000); // Set SMS mode to text
}

void loop() {
  while (neo6.available()) {
    if (gps.encode(neo6.read())) {
      if (gps.location.isUpdated()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        sendEmergencyMessage(latitude, longitude);
      }
    }
  }
}

void sendEmergencyMessage(float latitude, float longitude) {
  String message = "Emergency! Help needed at https://maps.google.com/?q=" + String(latitude, 6) + "," + String(longitude, 6);
  Serial.println("Sending message: " + message);

  sim800.println("AT+CMGS=\"" + emergencyPhoneNumber + "\"");
  delay(1000);
  sim800.print(message);
  sim800.write(26); // End the message with Ctrl+Z
  delay(1000);

  // Wait for a response from the SIM800 module
  waitForResponse(5000); // Adjust the timeout as needed
}

void sendATcommand(String command, unsigned int timeout) {
  String response = "";
  sim800.println(command);
  waitForResponse(timeout);
}

void waitForResponse(unsigned int timeout) {
  unsigned long startTime = millis();
  while (millis() - startTime < timeout) {
    if (sim800.available()) {
      char c = sim800.read();
      Serial.print(c);
    }
  }
  Serial.println();
}

