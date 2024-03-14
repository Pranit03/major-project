#include "twilio.hpp"

//SID - ACc68b0b09fd205e69828ec429ffe5f5e3
//Auth - 268ebe6585de1276703799bf8818e02e
// Phone - +16593992953 
// SSID:	Patil_5G

// Set these - but DON'T push them to GitHub!
// static const char *ssid = "Patil_5G";
// static const char *password = "9272735305";

static const char *ssid = "Patil";
static const char *password = "8698437788";

// Values from Twilio (find them on the dashboard)
static const char *account_sid = "ACc68b0b09fd205e69828ec429ffe5f5e3";
static const char *auth_token = "268ebe6585de1276703799bf8818e02e";
// Phone number should start with "+<countrycode>"
static const char *from_number = "+16593992953";

// You choose!
// Phone number should start with "+<countrycode>"
static const char *to_number = "+918698437788";
static const char *message = "Sent from my ESP32";

Twilio *twilio;

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to WiFi network ;");
  Serial.print(ssid);
  Serial.println("'...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting...");
    delay(500);
  }
  Serial.println("Connected!");

  twilio = new Twilio(account_sid, auth_token);

  delay(1000);
  String response;
  bool success = twilio->send_message(to_number, from_number, message, response);
  if (success) {
    Serial.println("Sent message successfully!");
  } else {
    Serial.println(response);
  }
}

void loop() {
}
