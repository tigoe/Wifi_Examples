/*
  Test HTTP Client for WPA2-enterprise on ESP32

  Tested with Nano ESP32 on eduroam at Cornell Tech, 
  and on eduroam at Aalborg University Copenhagen.
  Note that the WiFi radio on the ESP uses 2.4GHz,
  so you should check to see if your institution's 
  version of eduroam still implements 2.4Ghz. At AAU-Copenhagen, for example,
  there is a separate network called eduroam-2.4Ghz. 

  Here's what your arduino_secrets.h file should look like:
  #define EAP_IDENTITY ""      // use identity@institution.domain in for most institutions 
  #define EAP_USERNAME ""      // usually just a repeat of the identity
  #define EAP_PASSWORD ""      // your  password
  #define SECRET_SSID "eduroam"    // eduroam SSID

  Uses the following libraries:
  http://librarymanager/All#ArduinoHttpClient
  
  modified 2 May 2024
  by Tom Igoe
*/

#include <WiFi.h>      //Wifi library
#include "esp_wpa2.h"  //wpa2 library for connections to Enterprise networks
#include "arduino_secrets.h"

#include <ArduinoHttpClient.h>

WiFiClient netSocket;                       // network socket to server
const char server[] = "www.example.com";         // server name
HttpClient request(netSocket, server, 8000);  // make an HTTP client
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
