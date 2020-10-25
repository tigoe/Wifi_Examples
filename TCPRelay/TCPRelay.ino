/*
  A TCP Client/server relay

  created 25 Oct 2020
  by Tom Igoe
*/
#include <SPI.h>
#include <WiFiNINA.h> // use this for MKR1010 board
//#include <WiFi101.h>        // use this for the MKR1000 board
#include "arduino_secrets.h"

//// Initialize the Wifi client and server
WiFiClient outgoingClient;
WiFiServer server(8889);
int portNumber = 8889;
IPAddress destinationIP = IPAddress(192, 168, 1, 165);
int destinationPort = 8889;

// whether or not the incoming client was connected previously
boolean alreadyConnected = false;

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

  // if you're not connected, try to connect:
  if (!outgoingClient.connected()) {
    if (outgoingClient.connect(destinationIP, destinationPort)) {
      outgoingClient.println("Hello!");
    }
  }
}

void loop() {
  // if you're not connected, try to reconnect:
  if (!outgoingClient.connected()) {
    if (outgoingClient.connect(destinationIP, destinationPort)) {
      outgoingClient.println("Hello!");
    }
  }

  // wait for a new client:
  WiFiClient incoming = server.available();

  // when the client sends the first byte, say hello:
  if (incoming) {
    if (!alreadyConnected) {
      alreadyConnected = incoming.connected();
      // clear out the input buffer:
      incoming.flush();
      Serial.print("We have a new client: ");
      Serial.println(incoming.remoteIP());
      incoming.println("Hello, client!");
    }
    // if the incoming client has something to say, read it
    if (incoming.available() > 0) {
      // read the bytes incoming from the incoming client:
      String msg = incoming.readString();
      // echo the bytes back to the incoming client:
      server.print(msg);
      // echo the bytes to the outgoing client as well:
      Serial.println(msg);
      if (outgoingClient.connected()) {
        outgoingClient.print(msg);
      }
    }
  }
}
