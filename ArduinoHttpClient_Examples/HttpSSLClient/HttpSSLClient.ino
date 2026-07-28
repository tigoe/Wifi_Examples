/*
  Test HTTPS Client
  Based on the Test HTTP Client from Making Things Talk, 3rd ed.

  makes an HTTPS call and displays the result in the serial monitor
  Uses the following libraries:
  http://librarymanager/All#WiFi101   // use this for MKR1000
  http://librarymanager/All#WiFiNINA    // use this for MKR1010 or Nano 33 IoT
  http://librarymanager/All#ArduinoHttpClient

  For ESP32-based boards, see 
  https://github.com/tigoe/Wifi_Examples/tree/main/WiFi_ESP_examples/HttpsClientSimple_ESP
  
  created 12 Feb 2018
  modified 28 Jul 2026
  by Tom Igoe
*/

// include required libraries and config files
// #include <WiFi.h>          // use this  for Uno R4 WiFi modules
 #include <WiFiNINA.h>        // use this  for MKR1010/Nano 33 IoT modules
//#include <WiFi101.h>        // use this  for MKR1000/WINC1500 modules

#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"  // tab for the SSID and password

WiFiSSLClient sslClient;                   // WIFi SSL socket connection
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
