/*
  Test HTTPS Client
  Based on the Test HTTP Client from Making Things Talk, 3rd ed.

  makes an HTTPS call and displays the result on an SSD1306 OLED
  Uses the following libraries:
  http://librarymanager/All#WiFi101   // use this for MKR1000
  http://librarymanager/All#WiFiNINA    // use this for MKR1010 or Nano 33 IoT
  http://librarymanager/All#ArduinoHttpClient
  http://librarymanager/All#Adafruit_SSD1306

  created 12 Feb 2018
  modified 17 Jan 2021
  by Tom Igoe
*/

// include required libraries and config files
#include <SPI.h>
#include <WiFiS3.h>  // use this  for Uno R4 WiFi modules
// #include <WiFiNINA.h>           // use this  for MKR1010/Nano 33 IoT modules
//#include <WiFi101.h>          // use this  for MKR1000/WINC1500 modules
//#include <ESP8266WiFi.h>      // use this for ESP8266 modules
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"  // tab for the SSID and password

WiFiSSLClient netSocket;                  // network socket to server
const char server[] = "www.example.com";  // server name
String route = "/";                       // API route
int portNumber = 443;
// request timestamp in ms:
long lastRequest = 0;
// interval between requests:
int interval = 10000;

void setup() {
  Serial.begin(9600);  // initialize serial communication
  while (!Serial) delay(3000);
  // while you're not connected to a WiFi AP,
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("connecting to: " + String(SECRET_SSID));
    WiFi.begin(SECRET_SSID, SECRET_PASS);  // try to connect
    delay(2000);
  }
  // When you're connected, print out the device's network status:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void loop() {
  if (millis() - lastRequest > interval) {
    HttpClient http(netSocket, server, portNumber);  // make an HTTP client
    http.get(route);                                 // make a GET request

    // update display:
    String requestStatus = "requesting " + route;
    Serial.println(requestStatus);

    while (http.connected()) {              // while connected to the server,
      if (http.available()) {               // if there is a response from the server,
        String result = http.readString();  // read it
        // and print it:
        if (result.length() > 0) {
          Serial.println(result);
        }
      }
    }
    // when there's nothing left to the response,
    http.stop();  // close the request
    lastRequest = millis();
  }
}
