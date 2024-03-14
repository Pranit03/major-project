#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SIM800.h>

// Define the GPS module pins
static const int RXPin = 2; // Connect NEO-6 RX to Arduino TX (Pin 2)
static const int TXPin = 3; // Connect NEO-6 TX to Arduino RX (Pin 3)
static const uint32_t GPSBaud = 9600;

// Define the GSM module pins
static const int SIM800_RXPin = 4; // Connect SIM800 RX to Arduino TX (Pin 4)
static const int SIM800_TXPin = 5; // Connect SIM800 TX to Arduino RX (Pin 5)

// Define the phone number to send the SMS
char phoneNumber[] = "+1234567890"; // Replace with your phone number

// Define the message to be sent
char smsMessage[160];

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin); // GPS Software Serial
SIM800 gsm(SIM800_RXPin, SIM800_TXPin); // GSM Software Serial

void setup()
{
  Serial.begin(115200); // Debugging serial monitor
  gpsSerial.begin(GPSBaud); // GPS serial communication
  gsm.begin(9600); // GSM serial communication
  delay(1000);
  
  // Initialize GSM module
  gsm.init();
  gsm.powerOn();

  // Wait for the SIM card to be ready
  while (!gsm.checkSIMStatus())
  {
    delay(1000);
  }
}

void loop()
{
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  while (gpsSerial.available() > 0)
  {
    if (gps.encode(gpsSerial.read()))
    {
      if (gps.location.isValid())
      {
        // Create an SMS message with the GPS coordinates
        snprintf(smsMessage, sizeof(smsMessage), "Latitude: %.6f, Longitude: %.6f", gps.location.lat(), gps.location.lng());
        sendSMS(phoneNumber, smsMessage);
      }
    }
  }

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

void sendSMS(const char* recipient, const char* message)
{
  // Send an SMS using the GSM module
  gsm.sendSMS(recipient, message);

  // Wait for a moment to allow the SMS to be sent
  delay(5000);
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}
