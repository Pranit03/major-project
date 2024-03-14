#include <SoftwareSerial.h>
#include <TinyGPS++.h>

const int totalPhoneNo = 5;
String phoneNo[totalPhoneNo] = {"", "", "", "", ""};
int offsetPhone[totalPhoneNo] = {0, 13, 26, 39, 52};
String tempPhone = "";

#define rxGPS 0 //D1
#define txGPS 1 //D2
SoftwareSerial neogps(rxGPS, txGPS);
TinyGPSPlus gps;

#define rxGSM 3 //D3
#define txGSM 4 //D4
SoftwareSerial sim800(rxGSM, txGSM);

String smsStatus;
String senderNumber;
String receivedDate;
String msg;

boolean DEBUG_MODE = true;

void setup() {
  Serial.begin(115200);
  Serial.println("NodeMCU USB serial initialized");
  sim800.begin(9600);
  Serial.println("SIM800L serial initialized");
  neogps.begin(9600);
  Serial.println("NEO6M serial initialized");
  sim800.listen();
  neogps.listen();
  
  for (int i = 0; i < totalPhoneNo; i++) {
    phoneNo[i] = readFromFlash(offsetPhone[i]);
    if (phoneNo[i].length() != 13) {
      phoneNo[i] = "";
      Serial.println(String(i + 1) + ": empty");
    } else {
      Serial.println(String(i + 1) + ": " + phoneNo[i]);
    }
  }
  
  smsStatus = "";
  senderNumber = "";
  receivedDate = "";
  msg = "";
  
  delay(9000);
  sim800.println("AT+CMGF=1"); // SMS text mode
  delay(1000);
  sim800.println("AT+CLIP=1"); // Enable Caller ID
  delay(500);
}

void loop() {
  while (sim800.available()) {
    parseData(sim800.readString());
  }

  while (Serial.available()) {
    sim800.println(Serial.readString());
  }
}

void parseData(String buff) {
  Serial.println(buff);

  if (buff.indexOf("RING") > -1) {
    boolean flag = false;
    String callerID = "";

    if (buff.indexOf("+CLIP:")) {
      unsigned int index, index1;

      index = buff.indexOf("\"");
      index1 = buff.indexOf("\"", index + 1);
      callerID = buff.substring(index + 2, index1);
      callerID.trim();

      if (callerID.length() == 13) {
        flag = comparePhone(callerID);
      } else if (callerID.length() == 10) {
        callerID = "0" + callerID;
        flag = compareWithoutCountryCode(callerID);
      }
    }

    if (flag) {
      sim800.println("ATH");
      delay(1000);
      sendLocation(callerID);
    } else {
      sim800.println("ATH");
      debugPrint("The phone number is not registered.");
    }
    return;
  }

  unsigned int len, index;
  index = buff.indexOf(":");
  String cmd = buff.substring(0, index);
  cmd.trim();
  buff.remove(0, index + 2);

  if (cmd == "+CMTI") {
    index = buff.indexOf(",");
    String temp = buff.substring(index + 1, buff.length());
    temp = "AT+CMGR=" + temp + "\r";
    sim800.println(temp);
  } else if (cmd == "+CMGR") {
    extractSms(buff);
    if (msg.equals("r") && phoneNo[0].length() == 13) {
      writeToFlash(offsetPhone[0], senderNumber);
      phoneNo[0] = senderNumber;
      String text = "Number is Registered: ";
      text = text + senderNumber;
      debugPrint(text);
      Reply("Number is Registered", senderNumber);
    }
    if (comparePhone(senderNumber)) {
      doAction(senderNumber);
    }
  }
}

void doAction(String phoneNumber) {
  if (msg == "send location") {
    sendLocation(phoneNumber);
  } else if (msg.startsWith("r")) {
    int phoneIndex = msg[1] - '1';
    if (phoneIndex >= 0 && phoneIndex < totalPhoneNo) {
      tempPhone = msg.substring(3, 16);
      msg = "r" + String(phoneIndex + 1);
      writeToFlash(offsetPhone[phoneIndex], tempPhone);
      phoneNo[phoneIndex] = tempPhone;
      String text = "Phone" + String(phoneIndex + 1) + " is Registered: " + tempPhone;
      Reply(text, phoneNumber);
    }
  } else if (msg == "list") {
    String text = "";
    for (int i = 0; i < totalPhoneNo; i++) {
      if (phoneNo[i].length() > 0) {
        text += phoneNo[i] + "\r\n";
      }
    }
    debugPrint("List of Registered Phone Numbers: \r\n" + text);
    Reply(text, phoneNumber);
  } else if (msg.startsWith("del=")) {
    int phoneIndex = msg[4] - '1';
    if (phoneIndex >= 0 && phoneIndex < totalPhoneNo) {
      writeToFlash(offsetPhone[phoneIndex], "");
      phoneNo[phoneIndex] = "";
      Reply("Phone" + String(phoneIndex + 1) + " is deleted.", phoneNumber);
    }
  } else if (msg == "del=all") {
    for (int i = 0; i < totalPhoneNo; i++) {
      writeToFlash(offsetPhone[i], "");
      phoneNo[i] = "";
      offsetPhone[i] = NULL;
    }
    debugPrint("All phone numbers are deleted.");
    Reply("All phone numbers are deleted.", phoneNumber);
  }

  smsStatus = "";
  senderNumber = "";
  receivedDate = "";
  msg = "";
  tempPhone = "";
}

void extractSms(String buff) {
  unsigned int index;

  index = buff.indexOf(",");
  smsStatus = buff.substring(1, index - 1);
  buff.remove(0, index + 2);

  senderNumber = buff.substring(0, 13);
  buff.remove(0, 19);

  receivedDate = buff.substring(0, 20);
  buff.remove(0, buff.indexOf("\r"));
  buff.trim();

  index = buff.indexOf("\n\r");
  buff = buff.substring(0, index);
  buff.trim();
  msg = buff;
  buff = "";
  msg.toLowerCase();
}

void Reply(String text, String phone) {
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\"" + phone + "\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); // ASCII code for Ctrl-Z
  delay(1000);
  Serial.println("SMS Sent Successfully.");
  
  smsStatus = "";
  senderNumber = "";
  receivedDate = "";
  msg = "";
  tempPhone = "";
}

void sendLocation(String phoneNumber) {
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;) {
    while (neogps.available()) {
      if (gps.encode(neogps.read())) {
        newData = true;
        break;
      }
    }
  }

  if (newData) {
    newData = false;
    String latitude = String(gps.location.lat(), 6);
    String longitude = String(gps.location.lng(), 6);
    String text = "Latitude= " + latitude + "\n\r";
    text += "Longitude= " + longitude + "\n\r";
    text += "Speed= " + String(gps.speed.kmph()) + " km/h" + "\n\r";
    text += "Altitude= " + String(gps.altitude.meters()) + " meters" + "\n\r";
    text += "http://maps.google.com/maps?q=loc:" + latitude + "," + longitude;

    debugPrint(text);
    Reply(text, phoneNumber);
  }
}

void writeToFlash(int addrOffset, const String &strToWrite) {
  if (strToWrite.length() != 13) {
    return;
  }

  for (int i = 0; i < 13; i++) {
    ESP.flashWrite(addrOffset + i, strToWrite[i]);
  }
}

String readFromFlash(int addrOffset) {
  char data[14];
  for (int i = 0; i < 13; i++) {
    data[i] = ESP.flashRead(addrOffset + i);
  }
  data[13] = '\0';
  return String(data);
}

boolean comparePhone(String number) {
  for (int i = 0; i < totalPhoneNo; i++) {
    if (phoneNo[i] == number) {
      return true;
    }
  }
  return false;
}

boolean compareWithoutCountryCode(String number) {
  for (int i = 0; i < totalPhoneNo; i++) {
    String temp = phoneNo[i].substring(3);
    if (temp == number) {
      return true;
    }
  }
  return false;
}

void debugPrint(String text) {
  if (DEBUG_MODE) {
    Serial.println(text);
  }
}
