/*
  Test HTTP Client
  Uses the following libraries:
  http://librarymanager/All#ArduinoHttpClient
  http://librarymanager/All#WiFiNINA
 
*/
// include required libraries and config files
//#include <WiFi101.h>      // use this for MKR1000 boards
#include <WiFiNINA.h>       // use this for MKR1010 and Nano 33 IoT boards
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"


WiFiClient netSocket;               // network socket to server
const char server[] = "www.example.com";  // server name
String route = "/";              // API route

void setup() {
  Serial.begin(9600);               // initialize serial communication
  while (!Serial);        // wait for serial monitor to open
  
  // while you're not connected to a WiFi AP,
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);           // print the network name (SSID)
    WiFi.begin(SECRET_SSID, SECRET_PASS);         // try to connect
    delay(2000);
  }

  // When you're connected, print out the device's network status:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void loop() {
  Serial.println("making request");
  HttpClient http(netSocket, server, 80);      // make an HTTP client
  http.get(route);  // make a GET request

  while (http.connected()) {       // while connected to the server,
    if (http.available()) {        // if there is a response from the server,
      String result = http.readString();  // read it
      Serial.print(result);               // and print it
    }
  }
  //  // when there's nothing left to the response,
  http.stop();                     // close the request
  delay(10000);                    // wait 10 seconds
}
