/*
  Test HTTP Client for WPA2-enterprise on ESP32

  Tested with Nano ESP32 and Xiao ESP32C3 and 
  Xiao ESP32S3.

  Here's what your arduino_secrets.h file should look like:
  // #define SECRET_SSID ""         // your  password
  // #define SECRET_PASSWORD ""    // your SSID

  Uses the following libraries:
  http://librarymanager/All#ArduinoHttpClient
  http://librarymanager/All#WiFi for ESP32
  modified 25 Nov. 2026
  by Tom Igoe
*/

#include <WiFi.h>      //Wifi library for ESP32
#include "arduino_secrets.h"

#include <ArduinoHttpClient.h>

WiFiClient client;                          // network socket to server
const char server[] = "www.example.com";    // server name
HttpClient request(client, server, 80);     // make an HTTP client
String route = "/";                         // API route
// request timestamp in ms:
long lastRequest = 0;
// interval between requests:
int interval = 10000;
// count of request tries and successes:
int successes = 0;
int tries = 0;

void setup() {
  if (!Serial) delay(3000);  // wait for serial monitor to openZ
  Serial.begin(9600);        // initialize serial communication
 // initialize WiFI:
  WiFi.begin(SECRET_SSID, SECRET_PASSWORD);

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
}
void loop() {
  if (millis() - lastRequest > interval) {
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
