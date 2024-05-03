/*
  UDP OSC sender over eduroam

  Works with the Nano 33 ESP32
  Reads a single button and sends it as an OSC packet.

  Tested with Nano ESP32 on eduroam at Cornell Tech, 
  and on eduroam at Aalborg University Copenhagen.
  Note that the WiFi radio on the ESP uses 2.4GHz,
  so you should check to see if your institution's 
  version of eduroam still implements 2.4Ghz. At AAU-Copenhagen, for example,
  there is a separate network called eduroam-2.4Ghz. 

  Uses the OSC library by Adrian Freed and Yotam Man
  https://github.com/CNMAT/OSC
  http://librarymanager/All#WiFiUDP
  http://librarymanager/All#OSCMessage

  created 2 May 2022
  modified 2 May 2024
  by Tom Igoe
*/
#include <SPI.h>
#include <WiFi.h>
#include "esp_wpa2.h"  //wpa2 library for connections to Enterprise networks
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "arduino_secrets.h"

WiFiUDP Udp;                  // instance of UDP library
const int remotePort = 8000;  // port to which you want to send

//fill in the IP address you want to send to:
char remoteAddress[] = "192.168.0.23";

// variable for the previous state of the button:
int lastButtonState = 0;

void setup() {
  if (!Serial) delay(3000);  // wait for serial monitor to openZ
  Serial.begin(9600);        // initialize serial communication

  // set button pin mode:
  pinMode(2, INPUT_PULLUP);

  WiFi.begin(SECRET_SSID, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);
  // while you're not connected to a WiFi AP,
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);  // print the network name (SSID)
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
    Serial.println("message sent");
    msg.empty();      // free the space occupied by message

    // set lastButtonState = buttonState for next loop:
    lastButtonState = buttonState;
  }
}
