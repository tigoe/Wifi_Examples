
/*
  Simple connection and reconnection example
  Tries to connect in the setup. If the connection is broken,
  tries to reconnect in the loop.
  Every five seconds in the loop, it prints out the network stats. 
  
  created 9 April 2019
  modified 25 Oct 2020
  by Tom Igoe
*/

#include <SPI.h>        // Include SPI library
//#include <WiFi101.h>  // use this for MKR1000
#include <WiFiNINA.h>   // use this for MKR1010, Nano 33 IoT
#include "arduino_secrets.h"

// interval variables
long lastCheckTime = 0;
int interval = 5000;

void setup() {
  // initialize serial and builtin LED:
  Serial.begin(9600);
  // delay a few seconds for the serial monitor to open:
  delay(3000);
  // use the built-in LED to indicate the state of the connection:
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
// get the board's MAC address:
  Serial.println(getMacAddress());

  // connect:
  connectToNetwork();
}

void loop() {
  // if not connected, attempt to connect:
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    connectToNetwork();
  }
  // Every 5 seconds, get some info about the network:
  if (millis() - lastCheckTime > interval) {
    Serial.print("SSID:");
    Serial.println(WiFi.SSID());
    Serial.print("My MAC address:");
    Serial.println(getMacAddress());
    Serial.print("My IP:");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println("dBm");
    Serial.println();
    lastCheckTime = millis();
  }
}

// format MAC address:
String getMacAddress() {
  String result = "";
  byte mac[6];
  // get MAC address of this device
  WiFi.macAddress(mac);

  // format the MAC address as a hexadecimal string:
  for (int b = 5; b >= 0; b--) {
    if (mac[b] < 16) {
      result += "0";
    }
    result += String(mac[b], HEX);
    if (b > 0) {
      result += ":";
    }
  }
  return result;
}


void connectToNetwork() {
  Serial.print("Attempting to connect to: ");
  Serial.println(SECRET_SSID);
  // try to connect to the network:
  while ( WiFi.status() != WL_CONNECTED) {
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }

  // You're connected, turn on the LED:
  digitalWrite(LED_BUILTIN, HIGH);
}
