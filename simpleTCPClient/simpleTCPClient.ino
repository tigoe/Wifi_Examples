/*
  A simple TCP Client

  Uses the following libraries:
  http://librarymanager/All#WiFi101   // use this for MKR1000
  http://librarymanager/All#WiFiNINA    // use this for MKR1010 or Nano 33 IoT

  created 22 Oct 2018
  updated 17 Jan 2021
  by Tom Igoe
*/
#include <SPI.h>
#include <WiFiNINA.h> // use this for MKR1010 board and Nano 33 IoT
//#include <WiFi101.h>        // use this for the MKR1000 board
#include "arduino_secrets.h"

// Initialize the Wifi client library
WiFiClient client;
// the address and port of the server
const char serverAddress[] = "192.168.1.165";
int port = 8080;

// request timestamp in ms:
long lastRequest = 0;
// interval between requests:
int interval = 10000;


void setup() {
  Serial.begin(9600);
  // while you're not connected to a WiFi AP,
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);          // print the network name (SSID)
    WiFi.begin(SECRET_SSID, SECRET_PASS); // try to connect
    delay(2000);
  }

  // When you're connected, print out the device's network status:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void loop() {
  // if you're not connected, try to connect:
  if (!client.connected()) {
    client.connect(serverAddress, port);
  }
  // if you're connected, send every ten seconds:
  else {
    if (millis() - lastRequest > interval ) {
      String now = String(millis() / 10000);
      client.println(now);
      Serial.println(now);
      lastRequest = millis();
    }
  }
  // if there's anything incoming, print it:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
}
