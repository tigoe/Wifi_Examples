/*
  A  UDP Relay time tester
  Sends a UDP packet once every ten seconds and waits
  for the remote host to send a reply. Then prints out the
  round trip time.
   Uses the following libraries:
  http://librarymanager/All#WiFi101   // use this for MKR1000
  http://librarymanager/All#WiFiNINA  // use this for MKR1010 or Nano 33 IoT
  http://librarymanager/All#WiFiUDP

  created 22 Oct 2018
  modified 17 Jan 2021
  by Tom Igoe
*/
#include <SPI.h>
#include <WiFiNINA.h> // use this for MKR1010 board
//#include <WiFi101.h>        // use this for the MKR1000 board
#include <WiFiUdp.h>

#include "arduino_secrets.h"

WiFiUDP Udp;           // instance of UDP library
const int receivePort = 8888; // port on which this client receives

// the IP address and destination you will send to:
const char destinationIP[]  = "192.168.1.165";
int destPort = 8888;

// request timestamp in ms:
long lastRequest = 0;
// interval between requests:
int interval = 10000;

long sendTime;

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
  Udp.begin(receivePort);
}

void loop() {
  if (millis() - lastRequest > interval ) {
    sendTime = millis();
    // start a new packet:
    Udp.beginPacket(destinationIP, destPort);
    Udp.println(sendTime);    // add payload to it
    Udp.endPacket();     // finish and send packet
    Serial.println(sendTime);
    lastRequest = millis();
  }

  // if there's data available, read a packet
  if (Udp.parsePacket() > 0)
  { // parse incoming packet
    // calculate round trip time:
    long tripTime = millis() - sendTime;
    String message = "";
    Serial.print("From: "); // print the sender
    Serial.print(Udp.remoteIP());
    Serial.print(" on port: "); // and the port they sent on
    Serial.println(Udp.remotePort());
    while (Udp.available() > 0)
    { // parse the body of the message
      message = Udp.readString();
    }
    Serial.print("msg: " + message); // print it
    // print trip time:
    Serial.print("Round trip time: ");
    Serial.println(tripTime);
  }
}
