/*
 A simple TCP Client

  created 22 Oct 2018
  by Tom Igoe
*/
#include <SPI.h>
#include <WiFiNINA.h> // use this for MKR1010 board
//#include <WiFi101.h>        // use this for the MKR1000 board
#include "arduino_secrets.h"

//// Initialize the Wifi client library
//WiFiClient client;

WiFiServer server(8889);
boolean alreadyConnected = false; // whether or not the client was connected previously


void setup() {
  Serial.begin(9600);
  delay(3000);
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
  // start the server:
  server.begin();
}

void loop() {

   // wait for a new client:
  WiFiClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {
    if (!alreadyConnected) {
      // clead out the input buffer:
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

  
//  // if you're not connected, try to connect:
//  if (!client.connected()) {
//    client.connect("192.168.0.6", 8080);
//  }
//  // if you're connected, send every ten seconds:
//  else {
//    if (millis() % 10000 < 2)  {
//      String now = String(millis() / 10000);
//      client.println(now);
//      Serial.println(now);
//    }
//  }
//// if there's anything incoming, print it:
//  while (client.available()) {
//    char c = client.read();
//    Serial.write(c);
//  }
}
