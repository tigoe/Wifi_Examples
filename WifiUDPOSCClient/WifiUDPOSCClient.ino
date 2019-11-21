/*
  UDP OSC sender
  Works with the MKR1010 and Nano 33 IoT
  Uses the OSC library by Adrian Freed and Yotam Man
  https://github.com/CNMAT/OSC
  Add a new file called arduino_secrets.h to this sketch 
  for your SECRET SSID (network name) and SECRET_PASS (password)

  created 21 Nov 2019
  by Tom Igoe
*/
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "arduino_secrets.h"

WiFiUDP Udp;                 // instance of UDP library
const int remotePort = 7500; // port to which you want to send

//fill in the IP address you want to send to:
char remoteAddress[] = "192.168.43.126";

void setup() {
    Serial.begin(9600);
    //   while you're not connected to a WiFi AP,
    while ( WiFi.status() != WL_CONNECTED) {
      Serial.print("Attempting to connect to Network named: ");
      Serial.println (SECRET_SSID);           // print the network name (SSID)
      WiFi.begin(SECRET_SSID, SECRET_PASS);   // try to connect
      delay(2000);
    }
  
    // When you're connected, print out the device's network status:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    Udp.begin(remotePort);
}

void loop() {
// once every three seconds:
  if (millis() % 3000 < 3) {
    //the message wants an OSC address as first argument
    OSCMessage msg("/analog/0");
    msg.add((int32_t)analogRead(0));
    msg.send(Serial);
    Udp.beginPacket(remoteAddress, remotePort);
    msg.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    msg.empty(); // free space occupied by message
  }
}
