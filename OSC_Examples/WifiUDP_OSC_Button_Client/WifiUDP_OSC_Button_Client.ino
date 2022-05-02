/*
  UDP OSC sender
  Works with the MKR1010 and Nano 33 IoT
  Reads a single button and sends it as an OSC packet.

  Uses the OSC library by Adrian Freed and Yotam Man
  https://github.com/CNMAT/OSC
  Add a new file called arduino_secrets.h to this sketch
  for your SECRET SSID (network name) and SECRET_PASS (password)
  http://librarymanager/All#WiFi101   // use this for MKR1000
  http://librarymanager/All#WiFiNINA    // use this for MKR1010 or Nano 33 IoT
  http://librarymanager/All#WiFiUDP
  http://librarymanager/All#OSCMessage

  created 2 May 2022
  by Tom Igoe
*/
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "arduino_secrets.h"

WiFiUDP Udp;                  // instance of UDP library
const int remotePort = 8000;  // port to which you want to send

//fill in the IP address you want to send to:
char remoteAddress[] = "192.168.1.18";

// variable for the previous state of the button:
int lastButtonState = 0;

void setup() {
  Serial.begin(9600);
  // set button pin mode:
  pinMode(2, INPUT_PULLUP);

  //   while you're not connected to a WiFi AP,
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
  Udp.begin(remotePort);
}

void loop() {
  int buttonState = digitalRead(2);
  
  // send only if the button changes state:
  if (buttonState != lastButtonState) {
    // send OSC message
    OSCMessage msg("/publish/func/");
    // add the button state to the OSC message:
    msg.add(buttonState);
    // presumably you could add other button states here
    // if you wanted to...

    // set up a UDP packet to send the OSC message:
    Udp.beginPacket(remoteAddress, remotePort);
    msg.send(Udp);    // send the message
    Udp.endPacket();  // close the packet and send it
    msg.empty();      // free the space occupied by message

    // set lastButtonState = buttonState for next loop:
    lastButtonState = buttonState;
  }
}
