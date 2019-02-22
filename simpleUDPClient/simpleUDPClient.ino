/*
 A simple UDP Client

  created 22 Oct 2018
  by Tom Igoe
*/
#include <SPI.h>
#include <WiFiNINA.h> // use this for MKR1010 board
//#include <WiFi101.h>        // use this for the MKR1000 board
#include <WiFiUdp.h>
#include "arduino_secrets.h"

WiFiUDP Udp;           // instance of UDP library
const int port = 8888; // port on which this client receives

void setup()
{
  Serial.begin(9600);
  // while you're not connected to a WiFi AP,
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);          // print the network name (SSID)
    WiFi.begin(SECRET_SSID, SECRET_PASS); // try to connect
    delay(2000);
  }

  // When you're connected, print out the device's network status:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Udp.begin(port);
}

void loop()
{
  if (millis() % 10000 < 2)
  { // if ten seconds have passed:
    String now = String(millis() /10000);
    // start a new packet:
    Udp.beginPacket("192.168.0.6", 8888);
    Udp.println(now);    // add payload to it
    Udp.endPacket();     // finish and send packet
    Serial.println(now);

  }

  // if there's data available, read a packet
  if (Udp.parsePacket() > 0)
  { // parse incoming packet
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
  }
}
