/*
  A simple TCP Client
  Uses the following libraries:
  http://librarymanager/All#WiFi101   // use this for MKR1000
  http://librarymanager/All#WiFiNINA    // use this for MKR1010 or Nano 33 IoT

  For the Arduino Uno R4 WiFi, use
  #include <WiFiS3.h>
  For the Nano ESP32 use 
  #include <WiFi.h>

  created 22 Oct 2018
  updated 11 Dec 2023
  by Tom Igoe
*/
#include <SPI.h>
// #include <WiFiNINA.h>  // use this for MKR1010 board and Nano 33 IoT
// #include <WiFi101.h>       // use this for the MKR1000 board
#include <WiFiS3.h>        // use this for the Uno R4 WiFi board
// #include <WiFi.h>             // use this for the Nano ESP32 board
#include "arduino_secrets.h"

WiFiServer server(8080);
boolean alreadyConnected = false;  // whether or not the client was connected previously

void setup() {
  Serial.begin(9600);
  delay(3000);
  // while you're not connected to a WiFi AP,
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);           // print the network name (SSID)
    WiFi.begin(SECRET_SSID, SECRET_PASS);  // try to connect
    delay(2000);
  }

  // When you're connected, print out the device's network status:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // start the server:
  server.begin();
}

void loop() {

  // wait for a new client:
  WiFiClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {
    if (!alreadyConnected) {
      // clear out the input buffer:
      client.flush();
      Serial.println("We have a new client");
      client.println("Hello, client!");
      alreadyConnected = true;
    }

    if (client.available() > 0) {
      // read the bytes incoming from the client:
      char thisChar = client.read();
      // echo the bytes back to the client:
      server.write(thisChar);
      // echo the bytes to the server as well:
      Serial.write(thisChar);
    }
  }
}
