#include <TinyGPS++.h>
#include <SoftwareSerial.h>
TinyGPSPlus gps;
SoftwareSerial mygps(4,3); // GPS Tx Pin - D4  GPS Rx Pin D3

void setup(){
  Serial.begin(9600);
  mygps.begin(9600);
}

void loop(){
  // This sketch displays information every time a new sentence is correctly encoded.
  while (mygps.available() > 0){
    gps.encode(mygps.read());
    if (gps.location.isUpdated()){
      Serial.print("Latitude= "); 
      Serial.print(gps.location.lat(), 6);
      Serial.print(" Longitude= "); 
      Serial.println(gps.location.lng(), 6);
      }
       
    }
   
}

