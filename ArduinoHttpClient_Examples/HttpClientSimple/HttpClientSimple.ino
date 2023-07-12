/*
  Test HTTP Client
  Uses the following libraries:
  http://librarymanager/All#ArduinoHttpClient
  http://librarymanager/All#WiFi101   // use this for MKR1000
  http://librarymanager/All#WiFiNINA  // use this for MKR1010 or Nano 33 IoT
  http://librarymanager/All#WiFiS3    // use this for Uno R4 WiFi

  modified 12 July 2023
  by Tom Igoe
*/
// include required libraries and config files
//#include <WiFi101.h>      // use this for MKR1000 boards
// #include <WiFiNINA.h>  // use this for MKR1010 and Nano 33 IoT boards
#include <WiFiS3.h>  // use this for the Uno R4 WiFi
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"

WiFiSSLClient netSocket;                  // network socket to server
const char server[] = "www.example.com";  // server name
String route = "/";                       // API route
// request timestamp in ms:
long lastRequest = 0;
// interval between requests:
int interval = 10000;
// count of request tries and successes:
int successes = 0;
int tries = 0;

void setup() {
  Serial.begin(9600);        // initialize serial communication
  if (!Serial) delay(3000);  // wait for serial monitor to open

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
}

void loop() {
  if (millis() - lastRequest > interval) {
    long responseStartTime = millis();
    HttpClient request(netSocket, server, 443);  // make an HTTP client
    // set the timeout for the HTTP request, in ms:
    // request.setHttpResponseTimeout(60000);
    Serial.print("Starting request. HTTP response timeout: ");
    Serial.println(request.httpResponseTimeout());
    request.get(route);  // make a GET request

    int statusCode = request.responseStatusCode();
    //convert the statusCode to 1 or -1 and add to successes:
    successes += abs(statusCode) / statusCode;
    // increment tries:
    tries++;
    String response = request.responseBody();
    Serial.println("Tries: " + String(tries));
    Serial.println("Successes: " + String(successes));
    // print the status code and response :
    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);

    // calculate the response time, in ms:
    long responseEndTime = millis() - responseStartTime;
    Serial.print("Response time (ms): ");
    Serial.println(responseEndTime);
    lastRequest = millis();
  }
}
