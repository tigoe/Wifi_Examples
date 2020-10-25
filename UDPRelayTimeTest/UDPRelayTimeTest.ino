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
const int receivePort = 8888; // port on which this client receives

// the IP address and destination you will send to:
IPAddress destinationIP  = IPAddress(192, 168, 1, 239);
int destPort = 8888;

const int startButton = 2;
int lastButtonState = HIGH;
long startTime;

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

  pinMode(startButton, INPUT_PULLUP);
}

void loop() {
  int buttonState = digitalRead(startButton);
  if (buttonState < lastButtonState) {
    // button went low
    startTime = millis();
    // start a new packet:
    Udp.beginPacket(destinationIP, destPort);
    Udp.println(startTime);    // add payload to it
    Udp.endPacket();     // finish and send packet
    Serial.println(startTime);
  }
  // save current button state for comparison next time:
  lastButtonState = buttonState;


  // if there's data available, read a packet
  if (Udp.parsePacket() > 0)
  { // parse incoming packet
    // calculate round trip time:
    long tripTime = millis() - startTime;
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
