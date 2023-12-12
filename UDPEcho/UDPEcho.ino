/*
  A UDP Echo Server. Echoes whatever it receives.
  You can test this on a POSIX command line (linux, MacOS, etc) like so:
  $ nc -u arduino-ip-address 8888 
  then type anything and hit enter. Your Arduino should echo back
  anything that you sent it 3 times. 

  Uses the following libraries:
  http://librarymanager/All#WiFi101   // use this for MKR1000
  http://librarymanager/All#WiFiNINA  // use this for MKR1010 or Nano 33 IoT
  http://librarymanager/All#WiFiUDP
  
  For the Arduino Uno R4 WiFi, use
  #include <WiFiS3.h>
  For the Nano ESP32 use 
  #include <WiFi.h>
  
  created 22 Oct 2018
  modified 17 Jan 2021
  by Tom Igoe
*/
#include <SPI.h>
#include <WiFiNINA.h>  // use this for MKR1010 board and Nano 33 IoT
// #include <WiFi101.h>       // use this for the MKR1000 board
// #include <WiFiS3.h>        // use this for the Uno R4 WiFi board
// #include <WiFi.h>             // use this for the Nano ESP32 board
#include <WiFiUdp.h>
#include "arduino_secrets.h"

WiFiUDP Udp;            // instance of UDP library
const int port = 8888;  // port on which this client receives

void setup() {
  Serial.begin(9600);
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
  Udp.begin(port);
}

void loop() {
  // if there's data available, read a packet
  if (Udp.parsePacket() > 0) {
    // parse incoming packet
    String message = "";
    Serial.print("From: ");  // print the sender
    Serial.print(Udp.remoteIP());
    Serial.print(" on port: ");  // and the port they sent on
    Serial.println(Udp.remotePort());
    while (Udp.available() > 0) {  // parse the body of the message
      message = Udp.readString();
    }
    Serial.print("msg: " + message);  // print it
    // send it back three times:
    for (int m = 0; m < 3; m++) {
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.println(message);  // add payload to it
      Udp.endPacket();       // finish and send packet
    }
  }
}
