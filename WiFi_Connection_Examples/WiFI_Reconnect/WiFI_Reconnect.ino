/*
  WiFi reconnect

  This example shows how to check for connection
  continually and re-connect to a network
  if you lose the connection. If connected, it prints
  the signal strength.
  Uses the following libraries:
  http://librarymanager/All#WiFiNINA
  or
  http://librarymanager/All#WiFi101

  created 1 Mar 2018
  updated 11 Jan 2021
  by Tom Igoe
*/
#include <WiFiNINA.h>   // use this for MKR1010 or Nano 33 IoT
//#include <WiFi101.h>  // use this for MKR1000

// put your network SSID and password in
// a tab called arduino_secrets.h:
#include "arduino_secrets.h"

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  connectToNetwork();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToNetwork();
  }
  if (Serial) Serial.print("WiFi RSSI: ");
  if (Serial) Serial.print(WiFi.RSSI());
  if (Serial) Serial.println( " dBm");
}

void connectToNetwork() {
  // try to connect to the network:
  while ( WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);       // print the network name (SSID);
    // Connect to WPA/WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
  }
  digitalWrite(LED_BUILTIN, HIGH);
  // print the SSID of the network you're attached to:
  if (Serial) Serial.print("Connected to: ");
  if (Serial) Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  if (Serial) Serial.print("IP Address: ");
  if (Serial) Serial.println(ip);
}
