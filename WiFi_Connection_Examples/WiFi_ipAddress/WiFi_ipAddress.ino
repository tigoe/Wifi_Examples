
/*
  WiFi IP address converter.
  The IP address in the WiFi libraries is a 4-byte array. Sometimes
  you need it as a String. This example shows how.
  Uses the following libraries:
  http://librarymanager/All#WiFiNINA
  or
  http://librarymanager/All#WiFi101

  created 17 Jan 2021
  by Tom Igoe
*/

#include <WiFiNINA.h>   // use this for MKR1010 or Nano 33 IoT
//#include <WiFi101.h>  // use this for MKR1000
#include "arduino_secrets.h"

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // wait for Serial Monitor to open:
  while (!Serial);
  // use the builtin LED to show network status:
  pinMode(LED_BUILTIN, OUTPUT);
  // connect:
  connectToNetwork();
  
  // now get the IP address, which is a 4-byte array:
  unsigned int ip = WiFi.localIP();
  String ipString = "";
  while  (ip > 0) {
    ipString += String(ip % 256);  // get the last byte as a String
    ip >> 8;                      // shift the higher bits down
    if (ip > 0)  ipString += ".";  // add a dot
  }
  Serial.println(ipString);
  
  // you can also use print to convert the IP address to a String.
  // there's no way to return it though, so you can't combine it 
  // with other text unless you're printing it. 
  Serial.println(WiFi.localIP());
}

void loop() {
  // if not connected, attempt to connect:
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    connectToNetwork();
  }
}

void connectToNetwork() {
  while ( WiFi.status() != WL_CONNECTED) {
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
      digitalWrite(LED_BUILTIN, HIGH);
}
