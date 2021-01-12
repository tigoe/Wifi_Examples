/*
  WiFi Web Server LED Blink

  A simple web server that lets you blink an LED via the web.
  This sketch will print the IP address of your WiFi Shield (once connected)
  to the Serial monitor. From there, you can open that address in a web browser
  to turn on and off the LED on pin 6.
  Uses the following libraries:
  http://librarymanager/All#WiFiNINA


  If the IP address of your shield is yourAddress:
  http://yourAddress/H turns the LED on
  http://yourAddress/L turns it off

  This example is written for a network using WPA encryption. For
  WEP or WPA, change the Wifi.begin() call accordingly.

  Circuit:
   WiFi shield attached
   LED attached to the LED_BUILTIN pin

  created 25 Nov 2012
  modified 11 Jan 2021
  by Tom Igoe
*/
// include required libraries and config files
//#include <WiFi101.h>      // use this for MKR1000 boards
#include <WiFiNINA.h>       // use this for MKR1010 and Nano 33 IoT boards
#include "arduino_secrets.h"


WiFiServer server(80);

void setup() {
  Serial.begin(9600);

  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);       // print the network name (SSID);

    // Connect to WPA/WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
  }

  // print the SSID of the network you're attached to:
  Serial.print("Connected to: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  pinMode(LED_BUILTIN, OUTPUT);
  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    if (client.connected()) {            // loop while the client's connected
      while (client.available()) {             // if there's bytes to read from the client,
        currentLine = client.readStringUntil('\n');             // read a byte, then
        currentLine.trim();                 // trim any return characters
        Serial.println(currentLine);                    // print it out the serial monitor

        if (currentLine.length() == 0) {
          sendResponse(client);
          break;
        }
        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H HTTP/1.1")) {
          digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L HTTP/1.1")) {
          digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void sendResponse(WiFiClient client) {
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  // the content of the HTTP response follows the header:
  client.print("Click <a href=\"/H\">here</a> turn the LED on pin 6 on<br>");
  client.print("Click <a href=\"/L\">here</a> turn the LED on pin 6 off<br>");

  // The HTTP response ends with another blank line:
  client.println();

}
