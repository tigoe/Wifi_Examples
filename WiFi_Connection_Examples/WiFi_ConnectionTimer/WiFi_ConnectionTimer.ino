
/*
  WiFi Connection timer

  Uses the WiFi.getTime() command to keep track of time on a WiFi-connected board.
  Doesn't use a real-time clock. Checks the time drift of using millis() as the timekeeper.
  
  Uses the following libraries:
  http://librarymanager/All#WiFi101   // use this for MKR1000
  http://librarymanager/All#WiFiNINA  // use this for MKR1010 or Nano 33 IoT

  This example currently compiles for the WiFiS3 library, but the library does not 
  return a response from the getTime function. It does not currently compile for
  The ESP32 WiFi library, because that library is missing the getTime() function. 

  created 13 Dec 2023
  by Tom Igoe
*/

#include <WiFiNINA.h>  // use this for MKR1010 board and Nano 33 IoT
// #include <WiFi101.h>       // use this for the MKR1000 board
// #include <WiFiS3.h>  // use this for the Uno R4 WiFi board
// #include <WiFi.h>  // use this for the Nano ESP32 board

#include "arduino_secrets.h"

// the epoch time, obtained from the network:
unsigned long epoch = 0;
unsigned long lastMillis = 0;
unsigned long lastEpoch = 0;
void setup() {
  // initialize serial and builtin LED:
  Serial.begin(9600);
  // if serial monitor is closed, wait 3 sec:
  if (!Serial) delay(3000);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // if not connected, attempt to connect:
  if (WiFi.status() != WL_CONNECTED) {
    connectToNetwork();
    // exit the loop:
    return;
  }
  // check time drift once an hour:
  if (epoch - lastEpoch > 60*60) getNetworkTime();
  if (millis() > lastMillis + 1000) {
    // increment the epoch:
    epoch++;
    // increment lastMillis:
    lastMillis = millis();
    // print the time:
    Serial.println(getTimeString());
  }
}


// format timeString:
String getTimeString() {
  // calculate seconds, minutes, hours, from the epoch:
  int seconds = epoch % 60;
  int minutes = (epoch % 3600L) / 60;
  int hours = (epoch % 86400L) / 3600;
  String timeString = format2Digits(hours);
  timeString += ":";
  timeString += format2Digits(minutes);
  timeString += ":";
  timeString += format2Digits(seconds);
  timeString += " UTC";
  return timeString;
}

// format digit string:
String format2Digits(int number) {
  String result = "";
  if (number < 10) {
    result += "0";
  }
  result += String(number);
  return result;
}

void connectToNetwork() {
  Serial.print("Attempting to connect to network ");
  Serial.println(SECRET_SSID);
  // try to connect to the network:
  while (WiFi.status() != WL_CONNECTED) {
    // turn off built-in LED:
    digitalWrite(LED_BUILTIN, LOW);

    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  // When you're connected, print out the device's network status:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // turn on the LED:
  digitalWrite(LED_BUILTIN, HIGH);
  getNetworkTime();
}

void getNetworkTime() {
  // set the time from the network:
  // save the current epoch for comparison:
  lastEpoch = epoch;
  do {
    epoch = WiFi.getTime();
    delay(2000);
    Serial.print("NTP request attempt");
  } while (epoch <= 0);
  if (epoch != 0) {
    // print the last calculated time:
    Serial.print("Calculated epoch time: ");
    Serial.println(lastEpoch);
    // print the received time:
    Serial.print("Received epoch time via NTP: ");
    Serial.println(epoch);
    // print the difference:
    Serial.print("Time difference in secs: ");
    Serial.println(epoch - lastEpoch);
  }
}
