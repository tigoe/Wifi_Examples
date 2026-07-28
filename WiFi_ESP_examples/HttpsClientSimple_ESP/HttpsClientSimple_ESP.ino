/*
  Test HTTPS Client for WPA2 on ESP32

  The included arduino_secrets.h file details the credentials you'll need

  Uses the following libraries:
    ESP32 WiFiClientSecure
  ESP32 WiFi
  http://librarymanager/All#ArduinoHttpClient

  modified 28 July 2026
  by Tom Igoe
*/

#include <WiFi.h>               // Wifi library
#include <WiFiClientSecure.h>   // Wifi SSL library
#include <ArduinoHttpClient.h>  // HTTP Client library (same one that works on Nano 33 IoT, etc) 
#include "arduino_secrets.h"

WiFiClientSecure sslClient;                   // WIFi SSL socket connection
const char server[] = "www.example.com";      // server name
String route = "/";                           // API route
HttpClient request(sslClient, server, 443);   // HTTP request object

// request timestamp in ms:
long lastRequest = 0;
// interval between requests:
int interval = 10000;

void setup() {
  if (!Serial) delay(3000);  // wait 3 seconds if serial monitor is not open
  Serial.begin(9600);        // initialize serial communication
  // initialize WiFI:
  WiFi.begin(SECRET_SSID, SECRET_PASSWORD);

  // while you're not connected to a WiFi AP, connect:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);  // print the network name (SSID)
    delay(2000);
  }

  // When you're connected, print out the device's network status:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // set the root certificate for the WiFI SSL client:
  sslClient.setCACert(rootCACertificate);
}

void loop() {
  // if the interval between requests has passed:
  if (millis() - lastRequest > interval) {
    // start a new request:
    Serial.print("Starting request to: ");
    Serial.println(server + route);
    request.get(route);  // make a GET request

    // print the response code and the body when they arrive:
    int statusCode = request.responseStatusCode();
    Serial.print("Status code: ");
    Serial.println(statusCode);
    String response = request.responseBody();
    Serial.print("Response: ");
    Serial.println(response);
    // save the request time for comparison next time:
    lastRequest = millis();
  }
}
